#pragma once

#include "AudioSink.h"

void vita_clear_buffer();

class VitaAudioSink : public AudioSink {
 public:
    VitaAudioSink();
    ~VitaAudioSink();
    void feedPCMFrames(const uint8_t *buffer, size_t bytes);
    void volumeChanged(uint16_t volume);
 private:
    int threadid;
};
