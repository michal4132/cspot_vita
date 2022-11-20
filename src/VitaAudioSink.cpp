#include "VitaAudioSink.h"
#include "Logger.h"

#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/appmgr.h>
#include <cstring>

#define BUFFER_SIZE       2048
#define VITA_DECODE_SIZE (BUFFER_SIZE / 4)
#define BUFFER_COUNT     32

static uint8_t g_buffer[BUFFER_COUNT][BUFFER_SIZE];
static uint32_t data_len[BUFFER_COUNT] = {0};
static int end_flag = 0;
static int read_buffer = 0;
static int write_buffer = 0;
static int port;

static int feedBlocking() {
    while (1) {
        if (end_flag == 1) break;

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
        if (data_len[write_buffer] > BUFFER_SIZE) {
            CSPOT_LOG(error, "broken buffer[%d] = %d", write_buffer, data_len[write_buffer]);
        }

        if (data_len[write_buffer] < BUFFER_SIZE) {
            uint32_t buffer_space = BUFFER_SIZE - data_len[write_buffer];
            uint32_t aval_data = bytes - pos;
            if (aval_data > buffer_space) {
                memcpy(g_buffer[write_buffer]+data_len[write_buffer], buffer+pos, buffer_space);
                pos += buffer_space;
                data_len[write_buffer] += buffer_space;

                if (buffer_space == 0) {
                    CSPOT_LOG(error, "Buffer full");
                }

                // buffer full - switch to next
                write_buffer++;
                if (write_buffer >= BUFFER_COUNT) write_buffer = 0;
            } else {
                memcpy(g_buffer[write_buffer]+data_len[write_buffer], buffer+pos, aval_data);
                pos += aval_data;
                data_len[write_buffer] += aval_data;

                if (data_len[write_buffer] == BUFFER_SIZE) {
                    write_buffer++;
                    if (write_buffer >= BUFFER_COUNT) write_buffer = 0;
                }
            }
        } else {
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
