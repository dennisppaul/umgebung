// Copyright (c) 2024. Leonard Puhl

//
// Created by LMBU on 11.11.24.
//

#ifdef DISABLE_GRAPHICS
#error "DISABLE_GRAPHICS must be set to ON"
#endif

#include "InitIMGui.h"

#if defined(ENABLE_IMGUI) && !defined(DISABLE_GRAPHICS)
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"
#endif


namespace umgebung {
#if defined(ENABLE_IMGUI) && !defined(DISABLE_GRAPHICS)

    void setup_default_style() {
        auto& colors               = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg]  = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
        colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // Border
        colors[ImGuiCol_Border]       = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
        colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

        // Text
        colors[ImGuiCol_Text]         = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
        colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

        // Headers
        colors[ImGuiCol_Header]        = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
        colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        colors[ImGuiCol_HeaderActive]  = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // Buttons
        colors[ImGuiCol_Button]        = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
        colors[ImGuiCol_ButtonHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        colors[ImGuiCol_ButtonActive]  = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        colors[ImGuiCol_CheckMark]     = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

        // Popups
        colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

        // Slider
        colors[ImGuiCol_SliderGrab]       = ImVec4{0.44f, 0.37f, 0.61f, 0.54f};
        colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

        // Frame BG
        colors[ImGuiCol_FrameBg]        = ImVec4{0.13f, 0.13, 0.17, 1.0f};
        colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        colors[ImGuiCol_FrameBgActive]  = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // Tabs
        colors[ImGuiCol_Tab]                = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        colors[ImGuiCol_TabHovered]         = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
        colors[ImGuiCol_TabActive]          = ImVec4{0.2f, 0.22f, 0.27f, 1.0f};
        colors[ImGuiCol_TabUnfocused]       = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // Title
        colors[ImGuiCol_TitleBg]          = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        colors[ImGuiCol_TitleBgActive]    = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg]          = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
        colors[ImGuiCol_ScrollbarGrab]        = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
        colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

        // Seperator
        colors[ImGuiCol_Separator]        = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
        colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
        colors[ImGuiCol_SeparatorActive]  = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

        // Resize Grip
        colors[ImGuiCol_ResizeGrip]        = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
        colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
        colors[ImGuiCol_ResizeGripActive]  = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

        // Docking
        colors[ImGuiCol_DockingPreview] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};

        auto& style             = ImGui::GetStyle();
        style.TabRounding       = 4;
        style.ScrollbarRounding = 9;
        style.WindowRounding    = 7;
        style.GrabRounding      = 3;
        style.FrameRounding     = 3;
        style.PopupRounding     = 4;
        style.ChildRounding     = 4;
    }

    void imgui_init(APP_WINDOW* window, SDL_GLContext glContext, const int dpi) {
        // Setup Dear ImGui context
        std::cout << "create imgui context"
                  << "\n";
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Dear ImGui style
        // ImGui::StyleColorsDark();
        ImFontConfig config;
        config.RasterizerDensity = dpi;
        io.Fonts->AddFontFromFileTTF("../JetBrainsMono-Regular.ttf", 18, &config);

        // ImGui::StyleColorsLight();
        setup_default_style();
        auto& style = ImGui::GetStyle();

        // scale paddings and margins
        style.ScaleAllSizes(1.0f);

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(window, glContext);
        ImGui_ImplOpenGL2_Init();
    }

    void imgui_destroy() {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void imgui_prerender() {
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void imgui_postrender() {
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        ImGui::Render();
        // glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            SDL_Window*   backup_current_window  = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }


    void imgui_processevent(const SDL_Event& event) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
#else
    void imgui_init(APP_WINDOW* window, SDL_GLContext glContext, int dpi) {}
    void imgui_destroy() {}
    void imgui_prerender() {}
    void imgui_postrender() {}
    void imgui_processevent(const SDL_Event& event) {}
#endif
} // namespace umgebung
