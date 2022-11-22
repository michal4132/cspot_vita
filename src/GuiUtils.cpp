#include <mutex>  // NOLINT
#include "GuiUtils.h"
#include "Keyboard.h"

// Logger
static ImGuiTextBuffer Buf;
static bool ScrollToBottom;
FILE *logger_fp;
std::mutex log_mutex;

ImGuiTextBuffer *getBuf() {
    return &Buf;
}

bool getScrollToBottom() {
    return ScrollToBottom;
}

void setScrollToBottom(bool v) {
    ScrollToBottom = v;
}

void init_logger() {
    logger_fp = fopen("ux0:data/cspot/log.txt", "w");
}

void flush_logger() {
    fflush(logger_fp);
}

// override printf for cspot
int print_to_menu(const char* fmt, ...) {
    std::lock_guard<std::mutex> guard(log_mutex);
    if (Buf.size() > 6096) {
        Buf.clear();
        flush_logger();
    }
    va_list args, args2;
    va_start(args, fmt);
    va_copy(args2, args);
    Buf.appendfv(fmt, args);
    if (logger_fp != NULL) {
        vfprintf(logger_fp, fmt, args);
    }
    va_end(args);
    va_end(args2);
    ScrollToBottom = true;
    return 0;
}

// override vprintf for cspot
int vprint_to_menu(const char* fmt, va_list args) {
    std::lock_guard<std::mutex> guard(log_mutex);
    if (Buf.size() > 6096) {
        Buf.clear();
        flush_logger();
    }
    va_list args2;
    va_copy(args2, args);
    Buf.appendfv(fmt, args);
    if (logger_fp != NULL) {
        vfprintf(logger_fp, fmt, args2);
    }
    va_end(args2);
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

void AlignForWidth(float width, float alignment) {
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

bool StyleButton(const char* label, ImVec2 btn_size, bool active) {
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
        std::string text = Keyboard::GetText(prompt, password);
        if (text.length() == 0 && hint) {
            strncpy(buffer, hint, MAX_CREDENTIAL_LEN);
            set = false;
        } else {
            strncpy(buffer, text.c_str(), MAX_CREDENTIAL_LEN);
            set = true;
        }
    }
    ImGui::PopStyleColor();  // ImGuiCol_Text
    ImGui::PopStyleVar();  // ImGuiStyleVar_ButtonTextAlign
    ImGui::PopStyleColor();  // ImGuiCol_ButtonHovered
    ImGui::PopStyleColor();  // ImGuiCol_Button
}
