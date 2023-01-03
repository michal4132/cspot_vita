#include "PlaybackScreen.h"
#include "Font.h"
#include "Gui.h"
#include "GuiUtils.h"
#include <JSONObject.h>
#include "Utils.h"
#include "Config.h"


PlaybackScreen::PlaybackScreen(GUI *gui) : Screen(gui) {
    LoadTextureFromFile("app0:cover_art.png", &cover_art_tex, &cover_art_width, &cover_art_height);
    name = "Track name";
    artist = "Artist";
}

void PlaybackScreen::setPause(bool _isPaused) {
    isPaused = _isPaused;
}
void PlaybackScreen::setTrack(std::string _name, std::string _album, std::string _artist, std::string _imageUrl) {
    name = _name;
    album = _album;
    artist = _artist;
    if (imageUrl != _imageUrl) {
        imageUrl = _imageUrl;
        setCoverArt(imageUrl);
    }
}

void PlaybackScreen::setCoverArt(std::string url) {
    if (is_cover_cached(url)) {
        CSPOT_LOG(info, "Load cached cover art");
        LoadTextureFromFile(cover_art_path(url).c_str(), &cover_art_tex, &cover_art_width, &cover_art_height);
    } else {
        CSPOT_LOG(info, "Download cover art");
        cover_art_png_len = download(url.c_str(), &cover_art_png);
        if (cover_art_png_len > 0) {
            LoadTextureFromMemory(cover_art_png, cover_art_png_len,
                                  &cover_art_tex, &cover_art_width, &cover_art_height);
            cache_cover_art(url, cover_art_png, cover_art_png_len);
            free(cover_art_png);
            cover_art_png = NULL;
        }
    }
}

void PlaybackScreen::drawPlayer() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(30.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.00f, 0.00f));
    ImGui::Dummy(ImVec2(0.0f, 28.0f));

    // Cover art
    auto avail2 = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX((avail2 - 256) * 0.5f);
    ImGui::Image((void*)(intptr_t)cover_art_tex, ImVec2(256, 256));

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    // Buttons
    ImGuiStyle& style = ImGui::GetStyle();
    float width = 0.0f;
    width += 100.0f;
    width += style.ItemSpacing.x;
    width += 100.0f;
    width += style.ItemSpacing.x;
    width += 100.0f;
    AlignForWidth(width);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 50.0f);

    // Backward button
    ImGui::PushFont(gui->icon_font);
    if (StyleButton(ICON_FA_STEP_BACKWARD,  ImVec2(100.0f, 100.0f))) {
        if (gui->cspot_started) {
            gui->prevCallback();
        }
    }
    ImGui::PopFont();

    ImGui::SameLine();

    // Play pause button
    ImGui::PushFont(gui->playback_icon_font);
    const char *playback_icon = isPaused ? ICON_FA_PLAY_CIRCLE "###playpause" : ICON_FA_PAUSE_CIRCLE "###playpause"; //NOLINT
    if (StyleButton(playback_icon, ImVec2(100.0f, 100.0f))) {
        if (gui->cspot_started) {
            gui->playToggleCallback();
        }
    }
    ImGui::PopFont();

    ImGui::SameLine();

    // Forward button
    ImGui::PushFont(gui->icon_font);
    if (StyleButton(ICON_FA_STEP_FORWARD, ImVec2(100.0f, 100.0f))) {
        if (gui->cspot_started) {
            gui->nextCallback();
        }
    }
    ImGui::PopFont();

    ImGui::PopStyleVar();  // ImGuiStyleVar_ItemSpacing
    ImGui::PopStyleVar();  // ImGuiStyleVar_FrameRounding
    ImGui::PopStyleVar();  // ImGuiStyleVar_ItemSpacing

    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    TextCentered(name);
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    TextCentered(artist);
}

void PlaybackScreen::getTracks(uint16_t index) {
    CSPOT_LOG(debug, "Get tracks for playlist: %d", index);
    std::string uri = playlists[index].uri;

    if (!uri.starts_with(SPOTIFY_PLAYLIST_HEADER)) {
        return;
    }
    uri.erase(0, strlen(SPOTIFY_PLAYLIST_HEADER));
    playlists[index].tracks.clear();

    bool next = true;
    uint32_t pos = 0;

    while (next) {
        uint8_t *json_data;
        size_t json_len = gui->api.get_playlist_items(&json_data, uri, SPOTIFY_PLAYLIST_FIELDS,
                                                                    SPOTIFY_TRACK_FETCH_CHUNK_SIZE, pos);

        if (json_len <= 0) {
            CSPOT_LOG(error, "error requesting songs from playlist");
            playlists[index].tracks_loaded = true;
            playlists[index].tracks.push_back("No tracks");
            return;
        }

        cJSON *root = cJSON_Parse((const char *) json_data);
        if (!cJSON_HasObjectItem(root, "items")) {
            playlists[index].tracks_loaded = true;
            playlists[index].tracks.push_back("No tracks");
            return;
        }
        cJSON *json_next = cJSON_GetObjectItem(root, "next");
        cJSON *json_items = cJSON_GetObjectItem(root, "items");
        uint32_t tracks_in_chunk = cJSON_GetArraySize(json_items);

        for (uint32_t i = 0; i < tracks_in_chunk; i++) {
            cJSON *item = cJSON_GetArrayItem(json_items, i);
            cJSON *track = cJSON_GetObjectItem(item, "track");
            playlists[index].tracks.push_back(std::string(cJSON_GetObjectItem(track, "name")->valuestring));
            pos++;
        }

        next = !cJSON_IsNull(json_next);
        cJSON_Delete(root);
        free(json_data);
    }
    playlists[index].tracks_loaded = true;
    CSPOT_LOG(debug, "Got %d tracks", pos);
}

void PlaybackScreen::getPlaylists() {
    CSPOT_LOG(debug, "Get playlists");
    playlists.clear();

    bool next = true;
    uint32_t pos = 0;

    while (next) {
        uint8_t *json_data;
        size_t json_len = gui->api.get_current_users_playlists(&json_data, SPOTIFY_PLAYLIST_FETCH_CHUNK_SIZE, pos);

        if (json_len <= 0) {
            CSPOT_LOG(error, "error requesting playlists");
            return;
        }

        cJSON *root = cJSON_Parse((const char *) json_data);
        if (!cJSON_HasObjectItem(root, "items")) {
            return;
        }
        cJSON *json_next = cJSON_GetObjectItem(root, "next");
        cJSON *json_items = cJSON_GetObjectItem(root, "items");
        uint32_t playlists_in_chunk = cJSON_GetArraySize(json_items);

        for (uint32_t i = 0; i < playlists_in_chunk; i++) {
            cJSON *item = cJSON_GetArrayItem(json_items, i);
            playlists.push_back({std::string(cJSON_GetObjectItem(item, "name")->valuestring),
                                    std::string(cJSON_GetObjectItem(item, "uri")->valuestring), {}, false});
            pos++;
        }

        next = !cJSON_IsNull(json_next);
        cJSON_Delete(root);
        free(json_data);
    }

    CSPOT_LOG(debug, "Got %d playlists", playlists.size());
}

void PlaybackScreen::drawSubmenu() {
    ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);

    switch (submenu) {
        case Submenu::LOG:
            ImGui::PushFont(gui->log_font);
            ImGui::TextUnformatted(getBuf()->begin());
            ImGui::PopFont();
            if (getScrollToBottom())
                ImGui::SetScrollHere(1.0f);
            setScrollToBottom(false);
            break;
        case Submenu::PLAYLISTS:
            for (uint16_t i = 0; i < playlists.size(); i++) {
                if (ImGui::TreeNode((void*)(intptr_t)i, playlists[i].name.c_str())) {
                    if (!playlists[i].tracks_loaded) {
                        CSPOT_LOG(debug, "request track list for playlist: %s", playlists[i].name.c_str());
                        getTracks(i);
                    }

                    // play button
                    if (ImGui::Button("Play")) {
                        gui->activateDevice();
                        gui->api.play_by_uri(playlists[i].uri, 0, 0);
                    }

                    // each song in playlist
                    for (uint32_t track_offset = 0; track_offset < playlists[i].tracks.size(); track_offset++) {
                        if (ImGui::Button(playlists[i].tracks[track_offset].c_str())) {
                            gui->activateDevice();
                            gui->api.play_by_uri(playlists[i].uri, track_offset, 0);
                        }
                    }
                    ImGui::TreePop();
                }
            }
            break;
        case Submenu::SETTINGS:
            if (ImGui::Button("Refresh playlists")) {
                playlists.clear();
            }
            if (ImGui::Button("Refresh tracks")) {
                for (std::vector<Playlist>::iterator it = playlists.begin(); it != playlists.end(); ++it) {
                    it->tracks.clear();
                }
            }
            if (ImGui::Button("Exit")) {
                gui->isRunning = false;
            }
            if (ImGui::Button("Logout")) {
                gui->isRunning = false;
                remove(CREDENTIALS_FILE_NAME);
            }

            break;
        default:
            break;
    }

    ImGui::PopStyleColor();  // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor();  // ImGuiCol_Button
}

void PlaybackScreen::drawButtons() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);

    // Buttons
    ImGuiStyle& style = ImGui::GetStyle();
    float width = 0.0f;
    width += MENU_BUTTON_SIZE.x;
    width += style.ItemSpacing.x;
    width += MENU_BUTTON_SIZE.x;
    width += style.ItemSpacing.x;
    width += MENU_BUTTON_SIZE.x;
    width += style.ItemSpacing.x;
    width += MENU_BUTTON_SIZE.x;
    AlignForWidth(width);

    ImGui::PushFont(gui->icon_font);

    if (StyleButton(ICON_FA_BOOK, MENU_BUTTON_SIZE, submenu == Submenu::LOG)) {
        submenu = Submenu::LOG;
    }
    ImGui::SameLine();

    if (StyleButton(ICON_FA_MUSIC, MENU_BUTTON_SIZE, submenu == Submenu::PLAYLISTS)) {
        if (playlists.size() == 0) {
            getPlaylists();
        }
        submenu = Submenu::PLAYLISTS;
    }
    ImGui::SameLine();

    if (StyleButton(ICON_FA_COG, MENU_BUTTON_SIZE, submenu == Submenu::SETTINGS)) {
        submenu = Submenu::SETTINGS;
    }
    ImGui::SameLine();

    if (StyleButton(ICON_FA_SEARCH, MENU_BUTTON_SIZE, submenu == Submenu::SEARCH)) {
        submenu = Submenu::SEARCH;
    }

    ImGui::PopFont();
    ImGui::PopStyleVar();  // ImGuiStyleVar_FrameRounding
    ImGui::PopStyleVar();  // ImGuiStyleVar_ItemSpacing
}

void PlaybackScreen::draw() {
    ImGui::BeginChild("player", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.0f),
                      false, ImGuiWindowFlags_NavFlattened);
    this->drawPlayer();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("right_panel", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                      false, ImGuiWindowFlags_NavFlattened);
    {
        ImGui::BeginChild("submenu", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.85f),
                          false, ImGuiWindowFlags_NavFlattened);
        this->drawSubmenu();
        ImGui::EndChild();

        ImGui::BeginChild("buttons", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f),
                          false, ImGuiWindowFlags_NavFlattened);
        this->drawButtons();
        ImGui::EndChild();
    }
    ImGui::EndChild();
}
