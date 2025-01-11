// Copyright (c) 2024. Leonard Puhl

//
// Created by LMBU on 11.11.24.
//

#ifndef IMGUI_H
#define IMGUI_H

#include "Umgebung.h"
#include "imgui.h"

namespace umgebung {
    void        imgui_init(APP_WINDOW* window, SDL_GLContext glContext, int dpi);
    void        imgui_destroy();
    void        imgui_prerender();
    void        imgui_postrender();
    void        imgui_processevent(const SDL_Event& event);
    bool        imgui_is_keyboard_captured();
    bool        imgui_is_mouse_captured();
    std::string imgui_font();
    void        imgui_font(const std::string& file_path, float font_size);

} // namespace umgebung

#endif //IMGUI_H
