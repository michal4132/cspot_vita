#pragma once

class GUI;
class Screen {
 public:
    explicit Screen(GUI *gui) : gui(gui) {}
    virtual void draw() {}
 protected:
    GUI *gui;
};
