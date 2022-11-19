#pragma once

#include <string>

#define SPOTIFY_API_PLAY_URL                       "https://api.spotify.com/v1/me/player/play"
#define SPOTIFY_API_GET_USERS_PLAYLISTS    "https://api.spotify.com/v1/me/playlists"

class API {
 public:
    API() {}
    void set_token(std::string _token);
    void play_by_uri(std::string uri, uint32_t offset_pos, uint32_t position_ms);
    int get_current_users_playlists(uint8_t **buf, uint16_t limit, uint16_t offset);
 private:
    std::string token;
};
