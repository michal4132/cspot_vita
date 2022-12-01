#include "Utils.h"
#include <curl/curl.h>
#include <psp2/net/netctl.h>
#include <psp2/sysmodule.h>
#include <psp2/io/stat.h>
#include <Logger.h>
#include "Config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"


struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        CSPOT_LOG(error, "not enough memory (realloc returned NULL)");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

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

int download(const char *url, uint8_t **return_buffer, const char *method, std::string post_data, Headers headers) {
    CURL *curl_handle;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = (char *) malloc(1);
    chunk.size = 0;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, method);

    if (post_data.size() != 0) {
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, -1L);
    }

    struct curl_slist *headerchunk = NULL;
    for (auto it : headers) {
        headerchunk = curl_slist_append(headerchunk, (it.first + ": " + it.second).c_str());
    }
    res = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerchunk);

    // Perform the request
    res = curl_easy_perform(curl_handle);
    int httpresponsecode = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &httpresponsecode);
    if (httpresponsecode != 200) {
        CSPOT_LOG(debug, "response code: %d", httpresponsecode);
    }

    if (res != CURLE_OK) {
        CSPOT_LOG(error, "curl_easy_perform() failed: %s", curl_easy_strerror(res));
        // TODO(michal4132): need free chunk.memory???
        *return_buffer = NULL;
        chunk.size = 0;
    } else {
        CSPOT_LOG(debug, "%zu bytes retrieved", chunk.size);
        *return_buffer = (uint8_t *) chunk.memory;
    }
    curl_easy_cleanup(curl_handle);
    return chunk.size;
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
    sceSysmoduleLoadModule(SCE_SYSMODULE_JSON);
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
