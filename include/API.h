#pragma once

#include <string>

#define SPOTIFY_PLAYLIST_HEADER            "spotify:playlist:"
#define SPOTIFY_TRACK_FETCH_CHUNK_SIZE     50
#define SPOTIFY_PLAYLIST_FETCH_CHUNK_SIZE  15
#define SPOTIFY_PLAYLIST_FIELDS            "items(track(name,artists(name))),next"

#define SPOTIFY_API_PLAY_URL               "https://api.spotify.com/v1/me/player/play"
#define SPOTIFY_API_GET_USERS_PLAYLISTS    "https://api.spotify.com/v1/me/playlists"
#define SPOTIFY_API_GET_PLAYLIST_ITEMS_s   "https://api.spotify.com/v1/playlists/"
#define SPOTIFY_API_GET_PLAYLIST_ITEMS_e   "/tracks"
#define SPOTIFY_API_GET_AVAILABLE_DEVICES  "https://api.spotify.com/v1/me/player/devices"

class API {
 public:
    API() {}
    void set_token(std::string _token);
    void play_by_uri(std::string uri, uint32_t offset_pos, uint32_t position_ms);
    int get_current_users_playlists(uint8_t **buf, uint16_t limit, uint16_t offset);
    int get_playlist_items(uint8_t **buf, std::string playlist_id, std::string fields, uint16_t limit, uint16_t offset);
    int get_available_devices(uint8_t **buf);

 private:
    std::string token;
};
