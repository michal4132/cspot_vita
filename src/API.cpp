#include "API.h"
#include "Utils.h"
#include <Logger.h>

void API::set_token(std::string _token) {
    token = _token;
}

void API::play_by_uri(std::string uri, uint32_t offset_pos, uint32_t position_ms) {
    if (token.size() == 0) {
        return;
    }

    std::string url = SPOTIFY_API_PLAY_URL;
    url += "?device_id=";
    url += DEVICE_ID;

    // TODO(michal4132): Replace with json object
    std::string post_data = "{\"context_uri\": \"";
    post_data += uri;
    post_data += "\",\"offset\": {\"position\": ";
    post_data += std::to_string(offset_pos);
    post_data += "},\"position_ms\": ";
    post_data += std::to_string(position_ms);
    post_data += "}";
    uint8_t *buf;
    Headers headers = { {"Accept", "application/json"},
                        {"Content-Type", "application/json"},
                        {"Authorization", "Bearer " + token} };
    int len = download(url.c_str(), &buf, "PUT", post_data, headers);
    if (len > 0) {
        CSPOT_LOG(info, "play_by_uri response: %.*s", len, buf);
        sce_paf_free(buf);
    }
}

// TODO(michal4132): limit, offset
int API::get_current_users_playlists(uint8_t **buf, uint16_t limit, uint16_t offset) {
    if (token.size() == 0) {
        return -1;
    }

    Headers headers = { {"Accept", "application/json"},
                        {"Content-Type", "application/json"},
                        {"Authorization", "Bearer " + token} };

    std::string url = SPOTIFY_API_GET_USERS_PLAYLISTS;
    url += "?limit=";
    url += std::to_string(limit);
    url += "&offset=";
    url += std::to_string(offset);

    int len = download(url.c_str(), buf, "GET", "", headers);
    if (len <= 0) {
        buf = NULL;
        return 0;
    }

    CSPOT_LOG(info, "get_current_users_playlists response: %.*s", len, *buf);
    return len;
}

int API::get_playlist_items(uint8_t **buf, std::string playlist_id, std::string fields,
                                        uint16_t limit, uint16_t offset) {
    if (token.size() == 0) {
        return -1;
    }

    Headers headers = { {"Accept", "application/json"},
                        {"Content-Type", "application/json"},
                        {"Authorization", "Bearer " + token} };

    std::string url = "";
    if (playlist_id.starts_with("https://api.spotify.com/v1/playlists/")) {
        url += playlist_id;

    } else {
        url += SPOTIFY_API_GET_PLAYLIST_ITEMS_s;
        url += playlist_id;
        url += SPOTIFY_API_GET_PLAYLIST_ITEMS_e;
    }
    url += "?fields=";
    url += fields;
    url += "&limit=";
    url += std::to_string(limit);
    url += "&offset=";
    url += std::to_string(offset);

    int len = download(url.c_str(), buf, "GET", "", headers);
    if (len <= 0) {
        buf = NULL;
        return 0;
    }

    CSPOT_LOG(info, "get_playlist_items response: %.*s", len, *buf);
    return len;
}

