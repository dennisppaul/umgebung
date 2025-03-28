# Umgebung / TODO + NOTES

this is a VERY unsorted todo list and a note pad.

## graphics

- [ ] @umgebung add exporters ( `saveImage()`, `savePDF()`, `saveOBJ()` )
- [ ] separate transparent + non-transparent primitives
- [ ] in `PGraphicsDefault2D` implement 3D by manually transforming points onto 2D 
- [ ] @umgebung try PGraphics for offscreen rendering
- [ ] @umgebung add camera ( see https://chatgpt.com/share/67dfbe72-855c-8004-8b37-28d10d5c7ab3 )
- [ ] @umgebung add normals to sphere and box shapes
- [ ] @umgebung OpenGL version for different plattforms ( from imgui example ):
    ```C
    // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100 (WebGL 1.0)
        const char* glsl_version = "#version 100";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #elif defined(IMGUI_IMPL_OPENGL_ES3)
        // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
        const char* glsl_version = "#version 300 es";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        const char* glsl_version = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif
    ```
- [ ] `GL_POINTS` might need some shader love ( i.e `gl_PointSize` + `gl_PointCoord` for point sprites ) @later
- [ ] @umgebung add `PShape`-based fonts
- [ ] @umgebung implement extra buffer for transparent primitives
    - [ ] WB OIT https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended
    - [ ] https://www.khronos.org/opengl/wiki/Transparency_Sorting
- [ ] @umgebung add `curve` ( see chatgpt )
- [ ] @umgebung add to OpenGL2.0
    ```C
    void upload_image(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override;
    void download_image(PImage* img, bool restore_texture = true) override;
    ```
- [ ] @umgebung add function to `PFont` to generate a `PImage` with a static text as texture
- [ ] @umgebung remove all references to graphics subsystem from PGraphics ( and derived classes )

## audio

- [ ] @umgebung basic audio classes 
    - oscillator ( wavetable )
    - sampler
    - filter ( low, high, band )
    - envelope + adsr
    - trigger/beat

## environment

- [ ] @umgebung ==rename `loop` to `update` to comply with processing naming ( of `loop()` `noLoop()` )==
- [ ] @umgebung ==add option to run audio in own thread== ( see https://chatgpt.com/share/67dfc699-1d34-8004-a9a9-40716713ba2f )
- [ ] @umgebung add `set_window_title` with `SDL_SetWindowTitle(window, “TITLE”);` in subsystem
- [ ] @umgebung @maybe iterate in reverse order through subsytems so that graphics is last to be exectued in `draw_post` … same for `setup_post`

## credits + licenses


## building + platforms

- [ ] add `brew install sdl3_ttf`
- [ ] add `brew install glm`
- [ ] fetch mit cmake?!?
    ```cmake
    include(FetchContent)
    FetchContent_Declare(
        SDL3_ttf
        URL https://github.com/libsdl-org/SDL_ttf/releases/latest/download/SDL3_ttf-3.2.2.tar.gz
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    target_link_libraries(my_project PRIVATE SDL3_ttf::SDL3_ttf)
    ```
- [ ] add `stb_image_write.h`
- [ ] add `FreeType2` and `Harfbuzz`
- [ ] add iOS version of ffmpeg ( see https://github.com/kewlbear/FFmpeg-iOS-build-script ) and do somthing like this in CMake:
    ```cmake
    if(APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "iOS")
        # macOS: Use Homebrew's dynamically linked FFmpeg
        find_package(FFmpeg REQUIRED)
        target_link_libraries(${EXECUTABLE_NAME} PUBLIC FFmpeg::FFmpeg)
    elseif(IOS)
        # iOS: Use precompiled static FFmpeg libraries
        set(FFMPEG_IOS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ffmpeg-ios")
        target_include_directories(${EXECUTABLE_NAME} PRIVATE "${FFMPEG_IOS_PATH}/include")
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE 
            "${FFMPEG_IOS_PATH}/lib/libavcodec.a"
            "${FFMPEG_IOS_PATH}/lib/libavformat.a"
            "${FFMPEG_IOS_PATH}/lib/libavutil.a"
            "${FFMPEG_IOS_PATH}/lib/libswscale.a"
            "${FFMPEG_IOS_PATH}/lib/libswresample.a"
        )
    endif()
    ```
- [ ] @umgebung this is the gold standard for multi platform building https://github.com/ravbug/sdl3-sample
    - above is with cmake 
    - iOS manual https://wiki.libsdl.org/SDL3/README/ios

### OpenGL ES

- [ ] @umgebung try to run this with OpenGL ES window https://github.com/ravbug/sdl3-sample
- [ ] @umgebung what’s with ANGLE?
    - @research test OpenGL ES emulation ANGLE with SDL https://gist.github.com/SasLuca/307a523d2c6f2900af5823f0792a8a93

## weekend Projects

- [ ] Density Particle dingdong ( as shader and as normal )
- [ ] Radiance Cascades lighting shader 

## documentation

- [ ] @umgebung what’s new in umgebung? audio, `loadOBJ` mesh
- [ ] @umgebung start coding style doc
- [ ] @umgebung documentation :: differences in shader handling ( set_uniforms )
- [ ] @umgebung make a point of not supporting `colorMode(mode)`
 ```
                     + -> emit_shape_stroke_line_strip -> triangulate, transform to world/screen space
 begin-end-shape ->  |
                     + -> emit_shape_fill_triangles -> buffer, transform to world/screen space
 ```

## examples

- [ ] @umgebung start a series of *educated* examples
- [ ] @umgebung start dedicated font example
- [ ] @umgebung try with font [Inter](https://rsms.me/inter/)
- [ ] @umgebung @example add nice stroke example with rotierender box, square, open-closed-shape + linen im 0.5,0.85,1.0 style
- [ ] @umgebung @example add example for library

### example style:

```C
background(0.85f);        // grey
fill(0.5f, 0.85f, 1.0f);  // blue
fill(1.0f, 0.25f, 0.35f); // red
stroke(0.0f);             // black
```

## community

- [ ] @umgebung post on dev blog
- [ ] @umgebung on discord https://d3-is.de/umgebung ( https://discord.gg/hrckzRaW7g )
- [ ] @umgebung ask people for support and feature request etcetera … on github ( + discord )

## libraries

- [ ] gamepad ( e.g XBox controller as library or subsytem )

### ImGui

- [ ] @umgebung move imgui to `library`
- [ ] @umgebung revive imgui
    - [ ] https://github.com/ocornut/imgui/tree/master/examples/example_apple_opengl2
    - [ ] https://github.com/ocornut/imgui/tree/master/examples/example_sdl3_opengl3 
    ```C
       // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
    
        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);
    
        // Load Fonts ?!? see example
            
        // in loop
        {
            // event
            SDL_Event event;
            ImGui_ImplSDL3_ProcessEvent(&event);
    
            // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
            if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
            {
                SDL_Delay(10);
                continue;
            }
    
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();
    
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;
    
                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
    
                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
                ImGui::Checkbox("Another Window", &show_another_window);
    
                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
    
                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);
    
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }
    
    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
    
            // Rendering
            ImGui::Render();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);
        }
    ```
- [ ] @umgebung @imgui plot samples
    ```C
    // Generate samples and plot them
    float samples[100];
    for (int n = 0; n < 100; n++)
        samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
    ImGui::PlotLines("Samples", samples, 100);
    ```

### Ollama Library

- [ ] @umgebung add llm support with ollama see https://chatgpt.com/share/67da673d-56cc-8004-bcd2-3ece7ec1c260
    ```C
    #include <iostream>
    #include <string>
    #include <curl/curl.h>
    
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append((char*)contents, totalSize);
        return totalSize;
    }
    
    std::string queryOllama(const std::string& model, const std::string& prompt) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            return "Curl init failed";
        }
    
        std::string url = "http://localhost:11434/api/generate";
        std::string jsonPayload = R"({"model":")" + model + R"(","prompt":")" + prompt + R"("})";
        std::string response;
    
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
    
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            response = "Curl request failed: " + std::string(curl_easy_strerror(res));
        }
    
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return response;
    }
    
    int main() {
        std::string response = queryOllama("mistral", "Hello, how are you?");
        std::cout << "Ollama Response: " << response << std::endl;
        return 0;
    }
    ```

### CameraSDL Library

- [ ] @umgebung @research test SDL_camera
- [ ] @umgebung test SDL camera with PImage ( https://chatgpt.com/share/67da6661-ce48-8004-97e5-e3ed384011c8 ):
    ```c
    // create camera
    
    SDL_CameraID *cameras;
    int num_cameras = 0;
    cameras = SDL_GetCameras(&num_cameras);
    if (num_cameras == 0) {
        fprintf(stderr, "No cameras found.\n");
        SDL_Quit();
        return -1;
    }
    
    SDL_CameraSpec desired_spec = {0};
    desired_spec.format = SDL_PIXELFORMAT_RGBA32;
    desired_spec.width = 640;
    desired_spec.height = 480;
    desired_spec.fps = 30;
    
    SDL_Camera *camera = SDL_OpenCamera(cameras[0], &desired_spec);
    if (!camera) {
        fprintf(stderr, "SDL_OpenCamera Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    // wait for approval
    
    while (SDL_GetCameraPermissionState(camera) == SDL_CAMERA_PERMISSION_PENDING) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_CAMERA_DEVICE_APPROVED) {
                break;
            } else if (event.type == SDL_EVENT_CAMERA_DEVICE_DENIED) {
                fprintf(stderr, "Camera access denied.\n");
                SDL_CloseCamera(camera);
                SDL_Quit();
                return -1;
            }
        }
        SDL_Delay(100);
    }
    
    // loop update
    
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
    
        Uint64 timestamp;
        SDL_Surface *frame = SDL_AcquireCameraFrame(camera, &timestamp);
        if (frame) {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->w, frame->h, GL_RGBA, GL_UNSIGNED_BYTE, frame->pixels);
            SDL_ReleaseCameraFrame(camera, frame);
        }
    
        // Render the textured quad here
        // ...
    
        SDL_GL_SwapWindow(window);
    }
    ```

## research

- [ ] test with plugdata … again https://electro-smith.us18.list-manage.com/track/click?u=b473fcb56373b822c9fef8190&id=ff45d69a66&e=734f91f8ec

## unsorted

- [ ] (re-)design logo ( or check existing one )
- [ ] @umgebung rename PFont, PImage, PVector etcetera to UFont, UImage, UVector
- [ ] @umgebung maybe add an option to return the generated triangles with endShape()
- [ ] @umgebung design neon font ![](IMG_3102.jpeg)
- [ ] @umgebung add lighting ( see processing shaders `$HOME/Documents/dev/processing/git/processing4/core/src/processing/opengl/shaders` )
- [ ] @umgebung release end of the week ( need more examples until then )
- [ ] @umgebung PVector vs glm::vec3 at least some functions to convert between the two
- [ ] @umgebung byte the bullet and test offscreen rendering in `Advanced`

## Archived

- [x] make a step-by-step guide of how to create a library ( send to leo ) @archived(2025-03-28) @from(Umgebung / TODO + NOTES > documentation) @done(2025-03-28)
