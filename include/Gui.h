#pragma once

#include <imgui_vita.h>
#include <atomic>
#include "Utils.h"
#include "API.h"
#include <functional>
#include <string>
#include <Logger.h>
#include "Screen.h"

class GUI {
 public:
    ~GUI();
    void init();
    void start();
    std::atomic<bool> isRunning = true;
    ImFont *font;
    ImFont *playback_icon_font;
    ImFont *icon_font;
    ImFont *log_font;
    Screen *login_screen = nullptr;
    Screen *playback_screen = nullptr;
    void set_screen(Screen *s) { screen = s; }

    bool cspot_started = false;

    // Spotify API
    API api;

    // CSpot control
    std::function<void()> nextCallback;
    std::function<void()> prevCallback;
    std::function<void()> playToggleCallback;
    std::function<void()> activateDevice;

 private:
    Screen *screen;
};

int init_gui();
