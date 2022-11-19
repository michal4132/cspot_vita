#include "Gui.h"
#include "Keyboard.h"
#include "Utils.h"
#include <Logger.h>
#include "Font.h"

#define INPUT_USERNAME_HINT           "Username"
#define INPUT_PASSWORD_HINT           "Password"
#define ENTER_USERNAME_PROMPT         "Enter username"
#define ENTER_PASSWORD_PROMPT         "Enter password"

#define PLAY_BUTTON_BACKGROUND        IM_COL32(255, 255, 255, 255)
#define INPUT_USED_COLOR              IM_COL32(190, 190, 190, 255)
#define INPUT_HINT_COLOR              IM_COL32(82, 82, 82, 255)
#define TEXT_INPUT_BACKGROUND         IM_COL32(20, 20, 20, 255)
#define TEXT_INPUT_BACKGROUND_HOVERED IM_COL32(20, 20, 20, 255)
#define BACKGROUND_COLOR              ImVec4(0.09f, 0.09f, 0.15f, 1.00f)
#define WINDOW_FLAGS                  (ImGuiWindowFlags_NoTitleBar      \
                                     | ImGuiWindowFlags_NoMove          \
                                     | ImGuiWindowFlags_NoResize        \
                                     | ImGuiWindowFlags_NoCollapse)
#define MENU_BUTTON_SIZE              ImVec2(68.0f, 68.0f)

using namespace sce::Json; // NOLINT

class Allocator : public MemAllocator {
 public:
    Allocator() {}

    void* allocateMemory(size_t size, void *unk) override {
        return malloc(size);
    }
    void freeMemory(void *ptr, void *unk) override {
        free(ptr);
    }
};

// Logger buffer
static ImGuiTextBuffer Buf;
bool ScrollToBottom;

// override printf for cspot
int print_to_menu(const char* fmt, ...) {
    if (Buf.size() > 6096) {
        Buf.clear();
    }
    va_list args;
    va_start(args, fmt);
    Buf.appendfv(fmt, args);
    va_end(args);
    ScrollToBottom = true;
    return 0;
}

// override vprintf for cspot
int vprint_to_menu(const char* fmt, va_list args) {
    if (Buf.size() > 6096) {
        Buf.clear();
    }
    Buf.appendfv(fmt, args);
    ScrollToBottom = true;
    return 0;
}

ImFont* AddDefaultFont(float pixel_size ) {
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig config;
    config.SizePixels = pixel_size;
    config.OversampleH = config.OversampleV = 1;
    config.PixelSnapH = true;

     static const ImWchar ranges[] = {
        0x0020, 0x017F,  // Basic Latin + Latin-1 Supplement
        0,
    };

    ImFont *font = io.Fonts->AddFontFromFileTTF("ProggyVector.ttf", pixel_size, NULL, ranges);
    // ImFont *font = io.Fonts->AddFontDefault(&config);
    return font;
}

bool textInput(const char *prompt, const char *hint, char *buffer, size_t len, bool password = false) {
    std::string text = Keyboard::GetText(prompt, password);
    if (text.length() == 0 && hint) {
        strncpy(buffer, hint, len);
        return false;
    }
    strncpy(buffer, text.c_str(), len);
    return true;
}

void AlignForWidth(float width, float alignment = 0.5f) {
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void TextCentered(std::string text) {
    auto avail = ImGui::GetContentRegionAvail().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((avail - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

bool StyleButton(const char* label, ImVec2 btn_size, bool active = false) {
    if (active) {
        ImGui::PushStyleColor(ImGuiCol_Button, PLAY_BUTTON_BACKGROUND);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, PLAY_BUTTON_BACKGROUND);
        ImGui::PushStyleColor(ImGuiCol_Text, BACKGROUND_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);
        ImGui::PushStyleColor(ImGuiCol_Text, PLAY_BUTTON_BACKGROUND);
    }
    bool ret = ImGui::Button(label, btn_size);

    ImGui::PopStyleColor();  // ImGuiCol_Text
    ImGui::PopStyleColor();  // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor();  // ImGuiCol_Button
    return ret;
}

bool ButtonCenteredOnLine(const char* label, float alignment, ImVec2 btn_size) {
    ImGuiStyle& style = ImGui::GetStyle();

    float size = btn_size.x;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label, btn_size);
}

void TextInput::draw(ImVec2 size) {
    ImGui::PushStyleColor(ImGuiCol_Button, TEXT_INPUT_BACKGROUND);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, TEXT_INPUT_BACKGROUND_HOVERED);

    if (set) {
        ImGui::PushStyleColor(ImGuiCol_Text, INPUT_USED_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, INPUT_HINT_COLOR);
    }
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, text_align);

    if (ButtonCenteredOnLine(buffer, align, size)) {
        set = textInput(prompt, hint, buffer, MAX_CREDENTIAL_LEN, password);
    }
    ImGui::PopStyleColor();  // ImGuiCol_Text
    ImGui::PopStyleVar();  // ImGuiStyleVar_ButtonTextAlign
    ImGui::PopStyleColor();  // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor();  // ImGuiCol_Button
}

PlaybackScreen::PlaybackScreen(GUI *gui) : Screen(gui) {
    LoadTextureFromFile("app0:cover_art.png", &cover_art_tex, &cover_art_width, &cover_art_height);
    gui->name = "Track name";
    gui->artist = "Artist";
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
            sce_paf_free(cover_art_png);
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
    const char *playback_icon = gui->isPaused ? ICON_FA_PLAY_CIRCLE "###playpause" : ICON_FA_PAUSE_CIRCLE "###playpause"; //NOLINT
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

    TextCentered(gui->name);
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    TextCentered(gui->artist);
}

void PlaybackScreen::getPlaylists() {
    uint8_t *json_data;
    size_t json_len = gui->api.get_current_users_playlists(&json_data, 10, 0);

    // Create a MemAllocator derivative.
    Allocator* alloc = new Allocator();

    InitParameter params;
    params.allocator = alloc;
    params.bufSize = 100;

    Initializer init = Initializer();
    int ret = init.initialize(&params);  // Initialization at last!

    Value val = Value();  // Creating our Root value.
    Parser::parse(val, (const char*) json_data, json_len);

    for (int i = 0; i < val.count(); i++) {
        String s = String();
        const Value& v = val.getValue(i);
        CSPOT_LOG(info, "Child %d", i);

        switch (v.getType()) {
            case ValueType::BoolValue:
            case ValueType::IntValue:
            case ValueType::UIntValue:
            case ValueType::RealValue:
                v.toString(s);
                CSPOT_LOG(info, "Value: %s", s.c_str());
                break;
            case ValueType::StringValue:
                CSPOT_LOG(info, "Value: %s", v.getString().c_str());
                break;
            case ValueType::ArrayValue: {
                CSPOT_LOG(info, "Child %d is an Array. Iterating though values.", i);
                const Array& arr = v.getArray();
                int j = 0;
                for (Array::iterator it = arr.begin(); it != arr.end(); it++) {
                    Value& va = *it;
                    switch (va.getType()) {
                        case ValueType::ObjectValue: {
                                const Object& obj = va.getObject();
                                for (Object::Pair& pair : obj) {
                                    if (pair.key == "name") {
                                        CSPOT_LOG(info, "Track: %s", pair.value.getString().c_str());
                                        playlists.push_back(std::string(pair.value.getString().c_str()));
                                    } else if (pair.key == "uri") {
                                        playlist_uri.push_back(std::string(pair.value.getString().c_str()));
                                    }
                                }
                            }
                            break;
                        case ValueType::BoolValue:
                        case ValueType::IntValue:
                        case ValueType::UIntValue:
                        case ValueType::RealValue:
                            va.toString(s);
                            CSPOT_LOG(info, "Value %d: %s", j, s.c_str());
                            break;
                        case ValueType::StringValue:
                            CSPOT_LOG(info, "Value %d: %s", j, va.getString().c_str());
                            break;
                        default:
                            CSPOT_LOG(info, "Value is null");
                        }
                        s.clear();
                        ++j;
                    }
                }
                break;
            default:
                CSPOT_LOG(info, "Value is null");
        }
    }
    init.terminate();
    sce_paf_free(json_data);
}

void PlaybackScreen::drawSubmenu() {
    ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);

    switch (submenu) {
        case Submenu::LOG:
            ImGui::PushFont(gui->log_font);
            ImGui::TextUnformatted(Buf.begin());
            ImGui::PopFont();
            if (ScrollToBottom)
                ImGui::SetScrollHere(1.0f);
            ScrollToBottom = false;
            break;
        case Submenu::PLAYLISTS:
            for (uint16_t i = 0; i < playlists.size(); i++) {
                if (ImGui::TreeNode((void*)(intptr_t)i, playlists[i].c_str())) {
                    if (ImGui::Button("Play")) {
                        gui->activateDevice();
                        gui->api.play_by_uri(playlist_uri[i], 0, 0);
                    }
                    ImGui::Button("Track 1");
                    ImGui::Button("Track 2");
                    ImGui::Button("Track 3");
                    ImGui::Button("Track 4");
                    ImGui::Button("Track 5");
                    ImGui::Button("Track 6");
                    ImGui::Button("Track 7");
                    ImGui::TreePop();
                }
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
        getPlaylists();
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

LoginScreen::LoginScreen(GUI *gui) : Screen(gui) {
    bool ret = LoadTextureFromFile("icon_alpha.png", &logo_tex, &logo_width, &logo_height);

    username = TextInput(ImVec2(0.0f, 0.5f), 0.5f, ENTER_USERNAME_PROMPT, INPUT_USERNAME_HINT);
    password = TextInput(ImVec2(0.0f, 0.5f), 0.5f, ENTER_PASSWORD_PROMPT, INPUT_PASSWORD_HINT, true);
}

void start_cspot_thread(GUI *gui);
void login_cspot(const char *user, const char *password);

void LoginScreen::draw() {
    // top spacer
    ImGui::Dummy(ImVec2(0.0f, 42.0f));

    // cspot logo
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + logo_width);
    ImGui::Image(reinterpret_cast<void*>(logo_tex),
                    ImVec2(logo_width, logo_height));
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    username.draw(ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.0f));
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    password.draw(ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(30, 215, 96, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(16, 117, 52, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 24.0f);

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if (ButtonCenteredOnLine("LOG IN", 0.5f,
        ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, 50.0f))) {
        gui->set_screen(gui->playback_screen);
        login_cspot(username.buffer, password.buffer);
        start_cspot_thread(gui);
    }

    ImGui::PopStyleVar();  // ImGuiStyleVar_FrameRounding
    ImGui::PopStyleColor();  // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor();  // ImGuiCol_Button
}

LoginScreen::~LoginScreen() {
    // free cspot logo texture
    glDeleteTextures(1, &logo_tex);
}

void GUI::init() {
    vglInitExtended(0, 960, 544, 0x800000, SCE_GXM_MULTISAMPLE_4X);
    vglUseVram(GL_TRUE);

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplVitaGL_Init();
    io.MouseDrawCursor = false;

    font = AddDefaultFont(26);
    log_font = AddDefaultFont(12);

    // Add icon font
    ImFontConfig icons_config;
    icons_config.OversampleH = icons_config.OversampleV = 1;
    icons_config.PixelSnapH = true;

    ImWchar playback_ranges[] = {
        0xf144, 0xf144,  // play icon
        0xf28b, 0xf28b,  // pause icon
        0,
    };

    ImWchar ranges[] = {
        0xf048, 0xf048,  // backward icon
        0xf051, 0xf051,  // forward icon
        0xf013, 0xf013,  // cog (settings) icon
        0xf02d, 0xf02d,  // book (log) icon
        0xf002, 0xf002,  // search icon
        0xf001, 0xf001,  // music icon
        0,
    };

    icon_font = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS,
                                            48.0f, NULL, ranges);
    playback_icon_font = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS,
                                            96.0f, NULL, playback_ranges);
    io.Fonts->Build();

    // Setup style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = BACKGROUND_COLOR;
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 0.0f;

    ImGui_ImplVitaGL_TouchUsage(true);
    ImGui_ImplVitaGL_UseIndirectFrontTouch(false);
    ImGui_ImplVitaGL_UseRearTouch(false);
    ImGui_ImplVitaGL_GamepadUsage(true);
    ImGui_ImplVitaGL_MouseStickUsage(false);

    login_screen = new LoginScreen(this);
    playback_screen = new PlaybackScreen(this);
}

void GUI::start() {
    while (isRunning) {
        ImGui_ImplVitaGL_NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(960.0f, 544.0f), ImGuiCond_Once);

        if (ImGui::Begin("CSpot", nullptr, WINDOW_FLAGS)) {
            screen->draw();

            // ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            // bool show = true;
            // ImGui::ShowDemoWindow(&show);

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplVitaGL_RenderDrawData(ImGui::GetDrawData());
        vglSwapBuffers(GL_FALSE);
    }

    // ImGui_ImplVitaGL_Shutdown();
    ImGui::DestroyContext();
    vglEnd();
}

GUI::~GUI() {
    delete login_screen;
}
