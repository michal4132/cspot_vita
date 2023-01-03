#pragma once

#include <imgui_vita.h>
#include <vitaGL.h>
#include <string>

#define INPUT_USED_COLOR              IM_COL32(190, 190, 190, 255)
#define INPUT_HINT_COLOR              IM_COL32(82, 82, 82, 255)
#define TEXT_INPUT_BACKGROUND         IM_COL32(20, 20, 20, 255)
#define TEXT_INPUT_BACKGROUND_HOVERED IM_COL32(20, 20, 20, 255)
#define PLAY_BUTTON_BACKGROUND        IM_COL32(255, 255, 255, 255)
#define BACKGROUND_COLOR              ImVec4(0.09f, 0.09f, 0.15f, 1.00f)
#define WINDOW_FLAGS                  (ImGuiWindowFlags_NoTitleBar      \
                                     | ImGuiWindowFlags_NoMove          \
                                     | ImGuiWindowFlags_NoResize        \
                                     | ImGuiWindowFlags_NoCollapse)
#define MENU_BUTTON_SIZE              ImVec2(68.0f, 68.0f)

#define MAX_CREDENTIAL_LEN            256

class TextInput {
 public:
    TextInput() {}
    TextInput(ImVec2 text_align, float align, const char *prompt, const char *hint, bool password = false)
     : text_align(text_align), align(align), prompt(prompt), hint(hint), password(password) {
        strncpy(buffer, hint, MAX_CREDENTIAL_LEN);
    }
    void draw(ImVec2 size);
    char buffer[MAX_CREDENTIAL_LEN];
    bool password;
    bool set = false;
    ImVec2 text_align;
    float align;
    const char *prompt;
    const char *hint;
};

// ImGUI helper functions
bool getScrollToBottom();
void setScrollToBottom(bool v);
bool ButtonCenteredOnLine(const char* label, float alignment, ImVec2 btn_size);
ImFont* AddDefaultFont(float pixel_size);
bool StyleButton(const char* label, ImVec2 btn_size, bool active = false);
void AlignForWidth(float width, float alignment = 0.5f);
void TextCentered(std::string text);
ImGuiTextBuffer *getBuf();
void init_logger();
void flush_logger();

