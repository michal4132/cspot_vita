#include <codecvt>
#include <cstring>
#include <locale>
#include <psp2/kernel/clib.h>
#include <psp2/ime_dialog.h>
#include <vitaGL.h>

#include "keyboard.h"

/*
    Based off of libkdbvita by usineur -> https://github.com/usineur/libkbdvita/blob/master/kbdvita.c
*/

namespace Keyboard {
    static bool running = false;
    static const int SCE_COMMON_DIALOG_STATUS_CANCELLED = 3;
    static uint16_t buffer[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
    std::string text = std::string();

    int Init(const std::string &title) {
        if (running)
            return -1;

        // Clear our text buffer
        text.clear();

        // UTF8 -> UTF16
        std::u16string title_u16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(title.data());

        SceImeDialogParam param;
        sceImeDialogParamInit(&param);

        param.supportedLanguages = SCE_IME_LANGUAGE_ENGLISH;
        param.languagesForced = SCE_TRUE;
        param.type = SCE_IME_TYPE_DEFAULT;
        param.option = 0;
            
        param.title = reinterpret_cast<const SceWChar16 *>(title_u16.c_str());
        param.maxTextLength = SCE_IME_DIALOG_MAX_TEXT_LENGTH;
        param.inputTextBuffer = buffer;
        
        int ret = sceImeDialogInit(&param);
        if(ret < 0) {
            return ret;
        }

        running = true;
        return 0;
    }

    SceCommonDialogStatus Update(void) {
        if (!running)
            return SCE_COMMON_DIALOG_STATUS_NONE;
        
        SceCommonDialogStatus status = sceImeDialogGetStatus();
        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED) {
            SceImeDialogResult result;
            sceClibMemset(&result, 0, sizeof(SceImeDialogResult));
            sceImeDialogGetResult(&result);

            if ((result.button == SCE_IME_DIALOG_BUTTON_CLOSE) || (result.button == SCE_IME_DIALOG_BUTTON_ENTER)) {
                std::u16string buffer_u16 = reinterpret_cast<char16_t *>(buffer);
                text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(buffer_u16.data());
            }
            else
                status = static_cast<SceCommonDialogStatus>(SCE_COMMON_DIALOG_STATUS_CANCELLED);
            
            sceImeDialogTerm();
            running = false;
        }

        return status;
    }

    std::string GetText(const std::string &title) {
        if(Init(title) != 0) {
            return std::string();
        }
        
        bool done = false;
        do {
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(0, 0, 0, 1);

            SceCommonDialogStatus status = Update();
            if (status == SCE_COMMON_DIALOG_STATUS_FINISHED)
                done = true;
            else if (status != SCE_COMMON_DIALOG_STATUS_CANCELLED)
                done = false;

            vglSwapBuffers(GL_TRUE);
        } while(!done);

        return text;
    }
}