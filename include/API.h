#pragma once

#include <string>

#define CLIENT_ID_ANDROID "65b708073fc0480ea92a077233ca87bd"
#define DEVICE_ID         "142137fd329622137a14901634264e6f332e2411"
#define SCOPES            "user-read-playback-state,user-modify-playback-state,playlist-read-private,playlist-read-collaborative"  // NOLINT

#define SPOTIFY_PLAYLIST_HEADER            "spotify:playlist:"
#define SPOTIFY_TRACK_FETCH_CHUNK_SIZE     50
#define SPOTIFY_PLAYLIST_FETCH_CHUNK_SIZE  15
#define SPOTIFY_PLAYLIST_FIELDS            "items(track(name,artists(name))),next"

#define SPOTIFY_API_PLAY_URL               "https://api.spotify.com/v1/me/player/play"
#define SPOTIFY_API_GET_USERS_PLAYLISTS    "https://api.spotify.com/v1/me/playlists"
#define SPOTIFY_API_GET_PLAYLIST_ITEMS_s   "https://api.spotify.com/v1/playlists/"
#define SPOTIFY_API_GET_PLAYLIST_ITEMS_e   "/tracks"

class API {
 public:
    API() {}
    void set_token(std::string _token);
    void play_by_uri(std::string uri, uint32_t offset_pos, uint32_t position_ms);
    int get_current_users_playlists(uint8_t **buf, uint16_t limit, uint16_t offset);
    int get_playlist_items(uint8_t **buf, std::string playlist_id, std::string fields, uint16_t limit, uint16_t offset);

 private:
    std::string token;
};
