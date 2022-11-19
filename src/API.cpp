#include "API.h"
#include "Utils.h"
#include <Logger.h>

void API::set_token(std::string _token) {
    token = _token;
}

void API::play_by_uri(std::string uri, uint32_t offset_pos, uint32_t position_ms) {
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
    int len = download(SPOTIFY_API_PLAY_URL, &buf, SCE_HTTP_METHOD_PUT, post_data, headers);
    if (len != 0) {
        CSPOT_LOG(info, "play_by_uri response: %s", buf);
        sce_paf_free(buf);
    }
}

// TODO(michal4132): limit, offset
int API::get_current_users_playlists(uint8_t **buf, uint16_t limit, uint16_t offset) {
    Headers headers = { {"Accept", "application/json"},
                        {"Content-Type", "application/json"},
                        {"Authorization", "Bearer " + token} };

    int len = download(SPOTIFY_API_GET_USERS_PLAYLISTS, buf, SCE_HTTP_METHOD_GET, "", headers);
    if (len <= 0) {
        buf = NULL;
        return 0;
    }

    CSPOT_LOG(info, "get_current_users_playlists response: %s", &buf);
    return len;
}
