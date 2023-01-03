#include "VitaAudioSink.h"
#include "Logger.h"

#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/appmgr.h>
#include <cstring>

#define BUFFER_SIZE       3840
#define VITA_DECODE_SIZE (BUFFER_SIZE / 4)

static uint8_t g_buffer[2][BUFFER_SIZE];
static uint32_t data_len[2] = {0};
static uint8_t end_flag = 0;
static uint8_t read_buffer = 0;
static uint8_t write_buffer = 0;
static int port;

static int feedBlocking() {
    while (end_flag == 0) {
        if (data_len[read_buffer] == BUFFER_SIZE) {
            int res = sceAudioOutOutput(port, g_buffer[read_buffer]);
            if (res < 0) {
                 CSPOT_LOG(error, "sceAudioOutOutput error");
            }
            data_len[read_buffer] = 0;

            read_buffer = (read_buffer ? 0:1);
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
            uint32_t to_write = std::min(buffer_space, aval_data);

            memcpy(g_buffer[write_buffer]+data_len[write_buffer], buffer+pos, to_write);
            pos += to_write;

            data_len[write_buffer] += to_write;

            if (data_len[write_buffer] == BUFFER_SIZE) {
                write_buffer = (write_buffer ? 0:1);
            }
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

