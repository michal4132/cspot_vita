#include "VitaAudioSink.h"
#include "CircularBuffer.h"
#include "Logger.h"

#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/appmgr.h>
#include <cstring>

#define ONE_BUFFER_SIZE       2048
#define CIRCULAR_BUFFER_SIZE  ONE_BUFFER_SIZE * 2
#define VITA_DECODE_SIZE      (ONE_BUFFER_SIZE / 4)

static int port;
static int end_flag = 0;
static CircularBuffer buffer(CIRCULAR_BUFFER_SIZE);

void vita_clear_buffer() {
    buffer.emptyBuffer();
}

static int feedBlocking() {
    static uint8_t current_buffer[ONE_BUFFER_SIZE];
    while (end_flag == 0) {
        if (buffer.size() >= ONE_BUFFER_SIZE) {
            auto readNumber = buffer.read(current_buffer, ONE_BUFFER_SIZE);
            if (readNumber != ONE_BUFFER_SIZE) {
                CSPOT_LOG(error, "buffer error");
            }

            int res = sceAudioOutOutput(port, current_buffer);
            if (res < 0) {
                 CSPOT_LOG(error, "sceAudioOutOutput error");
            }
        } else {
            sceKernelDelayThread(10000);
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

void VitaAudioSink::feedPCMFrames(const uint8_t *buf, size_t bytes) {
    size_t bytesWritten = 0;
    while (bytesWritten < bytes) {
        auto bwrite = buffer.write(buf + bytesWritten, bytes - bytesWritten);
        bytesWritten += bwrite;

        if (bwrite == 0) {
            sceKernelDelayThread(10000);
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

