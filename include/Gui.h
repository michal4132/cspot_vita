#pragma once

#include <imgui_vita.h>
#include <atomic>
#include <string>
#include "Utils.h"
#include <functional>
#include <Logger.h>

#define MAX_CREDENTIAL_LEN            256

class TextInput {
 public:
    TextInput() {}
    TextInput(ImVec2 text_align, float align, const char *prompt, const char *hint, bool password = false)
     : text_align(text_align), align(align), prompt(prompt), hint(hint), password(password) {
        strncpy(buffer, hint, MAX_CREDENTIAL_LEN);
    }
    void draw(ImVec2 size);
    char buffer[MAX_CREDENTIAL_LEN];
    bool password;
    bool set = false;
    ImVec2 text_align;
    float align;
    const char *prompt;
    const char *hint;
};

class GUI;

class Screen {
 public:
    explicit Screen(GUI *gui) : gui(gui) {}
    virtual void draw() {}
 protected:
    GUI *gui;
};

class LoginScreen: public Screen {
 public:
    explicit LoginScreen(GUI *gui);
    ~LoginScreen();
    void draw();
 private:
    TextInput username;
    TextInput password;
    int logo_width = 0;
    int logo_height = 0;
    GLuint logo_tex = 0;
};

class PlaybackScreen: public Screen {
 public:
    explicit PlaybackScreen(GUI *gui);
    ~PlaybackScreen() {}
    void draw();
    void drawPlayer();
    void drawSubmenu();
    void drawButtons();
    void setCoverArt(std::string url);
 private:
    enum class Submenu {
      LOG,
      PLAYLISTS,
      SETTINGS,
      SEARCH,
    };
    Submenu submenu = Submenu::LOG;
    int cover_art_width = 0;
    int cover_art_height = 0;
    GLuint cover_art_tex = 0;
    uint8_t *cover_art_png = NULL;
    int32_t cover_art_png_len = 0;
};

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

    // Player status
    std::string name;
    std::string album;
    std::string artist;
    std::string imageUrl;
    bool isPaused;
    bool cspot_started = false;

    void setPause(bool _isPaused) {
      isPaused = _isPaused;
    }
    void setTrack(std::string _name, std::string _album, std::string _artist, std::string _imageUrl) {
      name = _name;
      album = _album;
      artist = _artist;
      if (imageUrl != _imageUrl) {
          imageUrl = _imageUrl;
          ((PlaybackScreen*) playback_screen)->setCoverArt(imageUrl);
      }
    }

    // CSpot control
    std::function<void()> nextCallback;
    std::function<void()> prevCallback;
    std::function<void()> playToggleCallback;

 private:
    Screen *screen;
};

bool ButtonCenteredOnLine(const char* label, float alignment, ImVec2 btn_size);
int init_gui();
