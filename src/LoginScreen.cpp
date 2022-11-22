#include "LoginScreen.h"
#include "Gui.h"

void start_cspot_thread(GUI *gui);
void login_cspot(const char *user, const char *password);

void LoginScreen::draw() {
    // top spacer
    ImGui::Dummy(ImVec2(0.0f, 42.0f));

    // cspot logo
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + logo_width);
    ImGui::Image(reinterpret_cast<void*>(logo_tex), ImVec2(logo_width, logo_height));
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

LoginScreen::LoginScreen(GUI *gui) : Screen(gui) {
    bool ret = LoadTextureFromFile("icon_alpha.png", &logo_tex, &logo_width, &logo_height);

    username = TextInput(ImVec2(0.0f, 0.5f), 0.5f, ENTER_USERNAME_PROMPT, INPUT_USERNAME_HINT);
    password = TextInput(ImVec2(0.0f, 0.5f), 0.5f, ENTER_PASSWORD_PROMPT, INPUT_PASSWORD_HINT, true);
}

LoginScreen::~LoginScreen() {
    // free cspot logo texture
    glDeleteTextures(1, &logo_tex);
}
