#pragma once

#include <imgui_vita.h>
#include <vector>
#include <string>
#include "Screen.h"

class PlaybackScreen: public Screen {
 public:
    explicit PlaybackScreen(GUI *gui);
    ~PlaybackScreen() {}
    void draw();
    void drawPlayer();
    void drawSubmenu();
    void drawButtons();
    void getTracks(uint16_t index);
    void getPlaylists();
    void setCoverArt(std::string url);
    void setPause(bool _isPaused);
    void setTrack(std::string _name, std::string _album, std::string _artist, std::string _imageUrl);

 private:
    enum class Submenu {
      LOG,
      PLAYLISTS,
      SETTINGS,
      SEARCH,
    };
     // Player status
    std::string name;
    std::string album;
    std::string artist;
    std::string imageUrl;
    bool isPaused;

    std::vector<std::string> playlists;
    std::vector<std::string> playlist_uri;
    std::vector<std::vector<std::string>> tracks;
    std::vector<bool> tracks_loaded;
    Submenu submenu = Submenu::LOG;
    int cover_art_width = 0;
    int cover_art_height = 0;
    GLuint cover_art_tex = 0;
    uint8_t *cover_art_png = NULL;
    int32_t cover_art_png_len = 0;
};
