#include "Utils.h"
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <psp2/io/stat.h>
#include <Logger.h>

#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"

#define USER_AGENT "Spotify/8.6.84 iOS/15.1 (iPhone11,8)"

bool init_downloader() {
    int res, tpl, conn, req;
    SceUInt64 length = 0;

    res = sceHttpInit(0x200000);
    if (res < 0) {
        CSPOT_LOG(error, "sceHttpInit failed (0x%X)\n", res);
        return false;
    }

    res = sceSslInit(0x200000);
    if (res < 0) {
        CSPOT_LOG(error, "sceSslInit failed (0x%X)\n", res);
        sceHttpTerm();
        return false;
    }
    CSPOT_LOG(error, "Init ok");
    return true;
}

std::string cover_art_path(std::string url) {
    size_t found = url.find_last_of("/\\");
    std::string path = "ux0:data/cspot/cache/" + url.substr(found+1);
    return path;
}

bool is_cover_cached(std::string url) {
    SceIoStat stat;
    std::string path = cover_art_path(url);
    return sceIoGetstat(path.c_str(), &stat) == 0;
}

bool cache_cover_art(std::string url, uint8_t *buffer, uint32_t length) {
    std::string path = cover_art_path(url);
    int fd = sceIoOpen(path.c_str(), SCE_O_TRUNC | SCE_O_CREAT | SCE_O_WRONLY, 0666);
    if (fd < 0) {
        return false;
    }

    sceIoWrite(fd, buffer, length);
    sceIoClose(fd);
    return true;
}

int download_image(const char *url, uint8_t **return_buffer) {
    int res, tpl, conn, req;
    SceUInt64 length = 0;

    tpl = sceHttpCreateTemplate(USER_AGENT, 2, 1);
    if (tpl < 0) {
        CSPOT_LOG(error, "sceHttpCreateTemplate failed (0x%X)\n", tpl);
        return 0;
    }

    conn = sceHttpCreateConnectionWithURL(tpl, url, 0);
    if (conn < 0) {
        CSPOT_LOG(error, "sceHttpCreateConnectionWithURL failed (0x%X)", conn);
        goto http_del_temp;
    }

    req = sceHttpCreateRequestWithURL(conn, 0, url, 0);
    if (req < 0) {
        CSPOT_LOG(error, "sceHttpCreateRequestWithURL failed (0x%X)", req);
        goto http_del_conn;
    }

    res = sceHttpSendRequest(req, NULL, 0);
    if (res < 0) {
        CSPOT_LOG(error, "sceHttpSendRequest failed (0x%X)", res);
        goto http_del_req;
    }

    res = sceHttpGetResponseContentLength(req, &length);
    CSPOT_LOG(info, "sceHttpGetResponseContentLength=0x%X", res);

    SceUID fd;
    void *recv_buffer;

    if (res < 0) {
        CSPOT_LOG(error, "No length");  // TODO(michal4132)
        length = 0;
        // recv_buffer = sce_paf_memalign(0x40, 0x40000);
        // if(recv_buffer == NULL){
            // CSPOT_LOG(error, "sce_paf_memalign return to NULL");
            // goto http_abort_req;
        // }
        // do {
        //  res = sceHttpReadData(req, recv_buffer, 0x40000);
        //  if(res > 0){
        //      res = sceIoWrite(fd, recv_buffer, res);
        //  }
        // } while(res > 0);
    } else {
        recv_buffer = sce_paf_memalign(0x40, (SceSize)length);
        if (recv_buffer == NULL) {
            CSPOT_LOG(error, "sce_paf_memalign return to NULL. length=0x%08X\n", (SceSize)length);
            goto http_abort_req;
        }

        res = sceHttpReadData(req, recv_buffer, (SceSize)length);
        if (res > 0) {
            *return_buffer = (uint8_t *) recv_buffer;
        } else {
            length = 0;
        }
    }

http_abort_req:
    sceHttpAbortRequest(req);
http_del_req:
    sceHttpDeleteRequest(req);
http_del_conn:
    sceHttpDeleteConnection(conn);
http_del_temp:
    sceHttpDeleteTemplate(tpl);
    return length;
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // Same
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromMemory(const uint8_t* buffer, uint32_t length,
                           GLuint* out_texture, int* out_width, int* out_height) {
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(buffer, length, &image_width, &image_height, NULL, 4);

    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // Same
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

int is_dir(const char *path) {
    SceIoStat stat = {0};
    if (sceIoGetstat(path, &stat) < 0) {
        return 0;
    }
    return SCE_S_ISDIR(stat.st_mode);
}

void init_network() {
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

    int res;
    SceUInt32 paf_init_param[6];
    SceSysmoduleOpt sysmodule_opt;

    paf_init_param[0] = 0x4000000;
    paf_init_param[1] = 0;
    paf_init_param[2] = 0;
    paf_init_param[3] = 0;
    paf_init_param[4] = 0x400;
    paf_init_param[5] = 1;

    res = ~0;
    sysmodule_opt.flags  = 0;
    sysmodule_opt.result = &res;

    sceSysmoduleLoadModuleInternalWithArg(SCE_SYSMODULE_INTERNAL_PAF,
                                          sizeof(paf_init_param), &paf_init_param, &sysmodule_opt);

    sceSysmoduleLoadModule(SCE_SYSMODULE_SSL);
    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);


    SceNetInitParam net_init_param;
    net_init_param.size = 0x200000;
    net_init_param.flags = 0;

    SceUID memid = sceKernelAllocMemBlock("SceNetMemory", 0x0C20D060, net_init_param.size, NULL);
    if (memid < 0) {
        CSPOT_LOG(error, "sceKernelAllocMemBlock failed (0x%X)\n", memid);
        return;
        // return memid;
    }

    sceKernelGetMemBlockBase(memid, &net_init_param.memory);

    res = sceNetInit(&net_init_param);
    if (res < 0) {
        CSPOT_LOG(error, "sceNetInit failed (0x%X)\n", res);
        return;
        // goto free_memblk;
    }

    res = sceNetCtlInit();
    if (res < 0) {
        CSPOT_LOG(error, "sceNetCtlInit failed (0x%X)\n", res);
        return;
        // goto net_term;
    }

    init_downloader();
}

void term_network() {
    sceNetTerm();
    sceNetCtlTerm();
}
