#include "gui.h"
#include "keyboard.h"
#include "utils.h"
#include <Logger.h>
#include "Font.h"

#define INPUT_USERNAME_HINT           "Username"
#define INPUT_PASSWORD_HINT           "Password"
#define ENTER_USERNAME_PROMPT         "Enter username"
#define ENTER_PASSWORD_PROMPT         "Enter password"

#define PLAY_BUTTON_BACKGROUND        IM_COL32(255,255,255,255)
#define INPUT_USED_COLOR              IM_COL32(190,190,190,255)
#define INPUT_HINT_COLOR              IM_COL32(82,82,82,255)
#define TEXT_INPUT_BACKGROUND         IM_COL32(20,20,20,255)
#define TEXT_INPUT_BACKGROUND_HOVERED IM_COL32(20,20,20,255)
#define BACKGROUND_COLOR              ImVec4(0.09f, 0.09f, 0.15f, 1.00f)

static ImGuiTextBuffer     Buf;
bool ScrollToBottom;

// override printf for cspot
int print_to_menu(const char* fmt, ...) {
    if(Buf.size() > 6096) {
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
    if(Buf.size() > 6096) {
        Buf.clear();
    }
    Buf.appendfv(fmt, args);
    ScrollToBottom = true;
    return 0;
}

ImFont* AddDefaultFont( float pixel_size ) {
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig config;
    config.SizePixels = pixel_size;
    config.OversampleH = config.OversampleV = 1;
    config.PixelSnapH = true;

     static const ImWchar ranges[] = {
        0x0020, 0x017F, // Basic Latin + Latin-1 Supplement
        0,
    };

    ImFont *font = io.Fonts->AddFontFromFileTTF("ProggyVector.ttf", pixel_size, NULL, ranges);
    // ImFont *font = io.Fonts->AddFontDefault(&config);
    return font;
}

bool textInput(const char *prompt, const char *hint, char *buffer, size_t len, bool password = false) {
    std::string text = Keyboard::GetText(prompt, password);
    if(text.length() == 0 && hint) {
        strncpy(buffer, hint, len);
        return false;
    }
    strncpy(buffer, text.c_str(), len);
    return true;
}

void AlignForWidth(float width, float alignment = 0.5f)
{
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

    if(set) {
        ImGui::PushStyleColor(ImGuiCol_Text, INPUT_USED_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, INPUT_HINT_COLOR);
    }
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, text_align);

    if (ButtonCenteredOnLine(buffer, align, size)) {
        set = textInput(prompt, hint, buffer, MAX_CREDENTIAL_LEN, password);
    }
    ImGui::PopStyleColor(); // ImGuiCol_Text
    ImGui::PopStyleVar(); // ImGuiStyleVar_ButtonTextAlign
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button
}

PlaybackScreen::PlaybackScreen(GUI *gui) : Screen(gui) {
    LoadTextureFromFile("app0:cover_art.png", &cover_art_tex, &cover_art_width, &cover_art_height);
    gui->name = "Track name";
    gui->artist = "Artist";
}

void PlaybackScreen::setCoverArt(std::string url) {
    if(is_cover_cached(url)) {
        CSPOT_LOG(info, "Load cached cover art");
        LoadTextureFromFile(cover_art_path(url).c_str(), &cover_art_tex, &cover_art_width, &cover_art_height);
    } else {
        CSPOT_LOG(info, "Download cover art");
        cover_art_png_len = download_image(url.c_str(), &cover_art_png);
        if(cover_art_png_len > 0) {
            LoadTextureFromMemory(cover_art_png, cover_art_png_len, &cover_art_tex, &cover_art_width, &cover_art_height);
            cache_cover_art(url, cover_art_png, cover_art_png_len);
            sce_paf_free(cover_art_png);
            cover_art_png = NULL;
        }
    }
}

void PlaybackScreen::drawPlayer() {
    ImGui::PushFont(gui->icon_font);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(30.0f, 0.0f));

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
    ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);
    if(ImGui::Button(ICON_FA_STEP_BACKWARD,  ImVec2(100.0f, 100.0f))) {
        gui->prevCallback();
    }
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button
    
    ImGui::SameLine();

    // Play pause button
    ImGui::PushStyleColor(ImGuiCol_Button, PLAY_BUTTON_BACKGROUND);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, PLAY_BUTTON_BACKGROUND);
    ImGui::PushStyleColor(ImGuiCol_Text, BACKGROUND_COLOR);
    const char *playback_icon = gui->isPaused ? ICON_FA_PLAY "###playpause" : ICON_FA_PAUSE "###playpause";
    if(ImGui::Button(playback_icon, ImVec2(100.0f, 100.0f))) {
        gui->playToggleCallback();
    }
    ImGui::PopStyleColor(); // ImGuiCol_Text
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button

    ImGui::SameLine();

    // Forward button
    ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);
    if(ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(100.0f, 100.0f))) {
        gui->nextCallback();
    }
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button

    ImGui::PopStyleVar(); // ImGuiStyleVar_FrameRounding
    ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing

    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    // track name text
    auto avail3 = ImGui::GetContentRegionAvail().x;
    auto textWidth2   = ImGui::CalcTextSize(gui->name.c_str()).x;
    ImGui::SetCursorPosX((avail3 - textWidth2) * 0.5f);
    ImGui::Text(gui->name.c_str()); 

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    // artist text
    auto avail = ImGui::GetContentRegionAvail().x;
    auto textWidth   = ImGui::CalcTextSize(gui->artist.c_str()).x;
    ImGui::SetCursorPosX((avail - textWidth) * 0.5f);
    ImGui::Text(gui->artist.c_str()); 
}

void PlaybackScreen::drawSubmenu() {
    ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);

    if(enable_log_window) {
        ImGui::PushFont(gui->log_font);
        ImGui::TextUnformatted(Buf.begin());
        ImGui::PopFont();
        if (ScrollToBottom)
            ImGui::SetScrollHere(1.0f);
        ScrollToBottom = false;
    } else if(enable_playlists_window) {
        for (int i = 0; i < 5; i++) {
            if (ImGui::TreeNode((void*)(intptr_t)i, "Playlist %d", i)) {
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

    }

    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button
}

void PlaybackScreen::drawButtons() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);

    // Buttons
    ImGuiStyle& style = ImGui::GetStyle();
    float width = 0.0f;
    width += 68.0f;
    width += style.ItemSpacing.x;
    width += 68.0f;
    width += style.ItemSpacing.x;
    width += 68.0f;
    width += style.ItemSpacing.x;
    width += 68.0f;
    AlignForWidth(width);

    ImGui::PushFont(gui->icon_font);

    if (enable_log_window) {
        ImGui::PushStyleColor(ImGuiCol_Button, PLAY_BUTTON_BACKGROUND);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, PLAY_BUTTON_BACKGROUND);
        ImGui::PushStyleColor(ImGuiCol_Text, BACKGROUND_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);
        ImGui::PushStyleColor(ImGuiCol_Text, PLAY_BUTTON_BACKGROUND);
    }

    if (ImGui::Button(ICON_FA_BOOK, ImVec2(68.0f, 68.0f))) {
        enable_log_window = true;
        enable_playlists_window = false;
    }

    ImGui::PopStyleColor(); // ImGuiCol_Text
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button
    ImGui::SameLine();

    if (enable_playlists_window) {
        ImGui::PushStyleColor(ImGuiCol_Button, PLAY_BUTTON_BACKGROUND);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, PLAY_BUTTON_BACKGROUND);
        ImGui::PushStyleColor(ImGuiCol_Text, BACKGROUND_COLOR);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);
        ImGui::PushStyleColor(ImGuiCol_Text, PLAY_BUTTON_BACKGROUND);
    }

    if (ImGui::Button(ICON_FA_MUSIC, ImVec2(68.0f, 68.0f))) {
        enable_playlists_window = true;
        enable_log_window = false;
    }

    ImGui::PopStyleColor(); // ImGuiCol_Text
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button

    ImGui::PushStyleColor(ImGuiCol_Button, BACKGROUND_COLOR);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BACKGROUND_COLOR);

    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_COG, ImVec2(68.0f, 68.0f))) {
        enable_log_window = false;
        enable_playlists_window = false;
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_SEARCH, ImVec2(68.0f, 68.0f))) {
        enable_log_window = false;
        enable_playlists_window = false;
    }
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button

    ImGui::PopFont();
    ImGui::PopStyleVar(); // ImGuiStyleVar_FrameRounding
    ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing
}

void PlaybackScreen::draw() {
    ImGui::BeginChild("player", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.0f), false, ImGuiWindowFlags_NavFlattened);
    this->drawPlayer();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("right_panel", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, ImGuiWindowFlags_NavFlattened);
    {
        ImGui::BeginChild("submenu", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.85f), false, ImGuiWindowFlags_NavFlattened);
        this->drawSubmenu();
        ImGui::EndChild();

        ImGui::BeginChild("buttons", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f), false, ImGuiWindowFlags_NavFlattened);
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
    ImGui::Image((void*)(intptr_t)logo_tex, ImVec2(logo_width, logo_height));
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    username.draw(ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.0f));
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    password.draw(ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(30,215,96,255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(16,117,52,255));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 24.0f);

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if (ButtonCenteredOnLine("LOG IN", 0.5f, ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, 50.0f))) {
        gui->set_screen(gui->playback_screen);
        login_cspot(username.buffer, password.buffer);
        start_cspot_thread(gui);
    }

    ImGui::PopStyleVar(); // ImGuiStyleVar_FrameRounding
    ImGui::PopStyleColor(); // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor(); // ImGuiCol_Button
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

    ImWchar ranges[] = {
        0xf04b, 0xf04b, // play icon
        0xf04c, 0xf04c, // pause icon
        0xf048, 0xf048, // backward icon
        0xf051, 0xf051, // forward icon
        0xf013, 0xf013, // cog (settings) icon
        0xf02d, 0xf02d, // book (log) icon
        0xf002, 0xf002, // search icon
        0xf001, 0xf001, // music icon
        0,
    };

    icon_font = io.Fonts->AddFontFromFileTTF( FONT_ICON_FILE_NAME_FAS, 48.0f, NULL, ranges);
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

#define WINDOW_FLAGS (ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)

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