#pragma once

#include "Screen.h"
#include "GuiUtils.h"

#define INPUT_USERNAME_HINT           "Username"
#define INPUT_PASSWORD_HINT           "Password"
#define ENTER_USERNAME_PROMPT         "Enter username"
#define ENTER_PASSWORD_PROMPT         "Enter password"

class LoginScreen: public Screen {
 public:
    explicit LoginScreen(GUI *gui);
    ~LoginScreen();
    void draw();
 private:
    TextInput username;
    TextInput password;
    int logo_width = 0;
    int logo_height = 0;
    GLuint logo_tex = 0;
};

