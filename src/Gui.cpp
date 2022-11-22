#include "Gui.h"
#include "GuiUtils.h"
#include "Utils.h"
#include <Logger.h>
#include "Font.h"
#include "PlaybackScreen.h"
#include "LoginScreen.h"

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

    icon_font = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 48.0f, NULL, ranges);
    playback_icon_font = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 96.0f, NULL, playback_ranges);
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
    delete playback_screen;
}
