#pragma once

#include <vector>
#include <fstream>
#include "AudioSink.h"

class VitaAudioSink : public AudioSink
{
public:
    VitaAudioSink();
    ~VitaAudioSink();
    void feedPCMFrames(const uint8_t *buffer, size_t bytes);
    void volumeChanged(uint16_t volume);
private:
    int threadid;
};
