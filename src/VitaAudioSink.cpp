#include "VitaAudioSink.h"
#include "Logger.h"

#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/appmgr.h>
#include <cstring>

#define BUFFER_SIZE       2048
#define VITA_DECODE_SIZE (BUFFER_SIZE / 4)
#define BUFFER_COUNT     16

static uint8_t g_buffer[BUFFER_COUNT][BUFFER_SIZE];
static uint32_t data_len[BUFFER_COUNT] = {0};
static int end_flag = 0;
static int read_buffer = 0;
static int write_buffer = 0;
static int port;

static int feedBlocking() {
    while (end_flag == 0) {
        if (data_len[read_buffer] == BUFFER_SIZE) {
            sceAudioOutOutput(port, g_buffer[read_buffer]);
            data_len[read_buffer] = 0;

            read_buffer++;
            if (read_buffer >= BUFFER_COUNT) read_buffer = 0;
        } else {
            sceKernelDelayThread(50000);
        }
    }
    return 0;
}

VitaAudioSink::VitaAudioSink() {
    softwareVolumeControl = false;

    sceAppMgrReleaseBgmPort();
    sceAppMgrAcquireBgmPort();

    port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, VITA_DECODE_SIZE, 44100, SCE_AUDIO_OUT_MODE_STEREO);

    threadid = sceKernelCreateThread("audio output", (SceKernelThreadEntry)feedBlocking, 0x10000100, 0x100, 0, 0, NULL);
    sceKernelStartThread(threadid, 0, NULL);
}

VitaAudioSink::~VitaAudioSink() {
    end_flag = 1;
    sceAudioOutReleasePort(port);
    sceKernelWaitThreadEnd(threadid, NULL, NULL);
}

void VitaAudioSink::feedPCMFrames(const uint8_t *buffer, size_t bytes) {
    uint32_t pos = 0;
    while (pos < bytes) {
        if (data_len[write_buffer] < BUFFER_SIZE) {
            uint32_t buffer_space = BUFFER_SIZE - data_len[write_buffer];
            uint32_t aval_data = bytes - pos;
            uint32_t to_write;
            if (aval_data > buffer_space) {
                to_write = buffer_space;
            } else {
                to_write = aval_data;
            }

            memcpy(g_buffer[write_buffer]+data_len[write_buffer], buffer+pos, to_write);
            pos += to_write;

            // set buffer length after making a copy to ensure this
            // value won't be changed from another thread during comparison
            uint32_t buffer_len = data_len[write_buffer] + to_write;
            data_len[write_buffer] = buffer_len;

            if (buffer_len == BUFFER_SIZE) {
                write_buffer++;
                if (write_buffer >= BUFFER_COUNT) write_buffer = 0;
            }
        } else {
            // CSPOT_LOG(debug, "buffer[%d] len: %d", write_buffer, data_len[write_buffer]);
            sceKernelDelayThread(50000);
        }
    }
}

void VitaAudioSink::volumeChanged(uint16_t volume) {
    volume = volume / 2;
    if (volume > SCE_AUDIO_VOLUME_0DB) {
        volume = SCE_AUDIO_VOLUME_0DB;
    }
    int vol[2] = {volume, volume};
    sceAudioOutSetVolume(port, (SceAudioOutChannelFlag) (SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), vol);
}

