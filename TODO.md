# Umgebung / TODO + NOTES

this is a VERY unsorted todo list and a note pad.

## graphics

- [ ] @umgebung add exporters ( `saveImage()`, `savePDF()`, `saveOBJ()` )
- [ ] separate transparent + non-transparent primitives
- [ ] in `PGraphicsDefault2D` implement 3D by manually transforming points onto 2D 
- [ ] @umgebung add camera ( see https://chatgpt.com/share/67dfbe72-855c-8004-8b37-28d10d5c7ab3 )
- [ ] @umgebung add normals to sphere and box shapes
- [ ] `GL_POINTS` might need some shader love ( i.e `gl_PointSize` + `gl_PointCoord` for point sprites ) @later
- [ ] @umgebung add `PShape`-based fonts
- [ ] @umgebung implement extra buffer for transparent primitives
    - [ ] WB OIT https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended
    - [ ] https://www.khronos.org/opengl/wiki/Transparency_Sorting
- [ ] @umgebung add to OpenGL2.0
    ```C
    void upload_image(PImage* img, const uint32_t* pixel_data, int width, int height, int offset_x, int offset_y, bool mipmapped) override;
    void download_image(PImage* img, bool restore_texture = true) override;
    ```
- [ ] @umgebung add function to `PFont` to generate a `PImage` with a static text as texture
- [ ] @umgebung remove all references to graphics subsystem from PGraphics ( and derived classes )
- [ ] @umgebung fix set window title ( default to `$PROJECT_NAME`from CMake )

## audio

- [ ] @umgebung basic audio classes 
    - oscillator ( wavetable )
    - sampler
    - filter ( low, high, band )
    - envelope + adsr
    - trigger/beat
- [ ] @umgebung ring buffer for audio
    ```c
    
    #include <vector>
    #include <iostream>
    #include <algorithm>
    
    class ChunkedCircularBuffer {
    public:
        ChunkedCircularBuffer(size_t size) : buffer(size), max_size(size), head(0), count(0) {}
    
        void push(const std::vector<float>& chunk) {
            size_t chunk_size = chunk.size();
            if (chunk_size >= max_size) {
                // If chunk is larger than buffer, only keep the last max_size elements
                std::copy(chunk.end() - max_size, chunk.end(), buffer.begin());
                head = 0;
                count = max_size;
                return;
            }
    
            // If the chunk would overflow the buffer, adjust `count`
            if (count + chunk_size > max_size) {
                count = max_size;
            } else {
                count += chunk_size;
            }
    
            // Insert chunk into buffer
            for (size_t i = 0; i < chunk_size; ++i) {
                buffer[head] = chunk[i];
                head = (head + 1) % max_size;
            }
        }
    
        // Get the most recent `chunk_size` elements as a contiguous block into `latestChunk`
        void getLatestChunk(size_t chunk_size, std::vector<float>& latestChunk) const {
            if (chunk_size > count) {
                chunk_size = count;  // Clamp to available data
            }
    
            latestChunk.resize(chunk_size);  // Ensure the vector is the correct size
            size_t start = (head + max_size - chunk_size) % max_size;
    
            // Check if the requested chunk wraps around
            if (start + chunk_size <= max_size) {
                // Contiguous case: Direct copy
                std::copy(buffer.begin() + start, buffer.begin() + start + chunk_size, latestChunk.begin());
            } else {
                // Wrap-around case: Copy in two parts
                size_t first_part = max_size - start;
                std::copy(buffer.begin() + start, buffer.end(), latestChunk.begin());
                std::copy(buffer.begin(), buffer.begin() + (chunk_size - first_part), latestChunk.begin() + first_part);
            }
        }
    
        void printBuffer() const {
            for (size_t i = 0; i < count; ++i) {
                size_t index = (head + max_size - count + i) % max_size;
                std::cout << buffer[index] << " ";
            }
            std::cout << "\n";
        }
    
    private:
        std::vector<float> buffer;
        size_t max_size;
        size_t head;
        size_t count;
    };
    
    int main() {
        ChunkedCircularBuffer cb(5);
        std::vector<float> latestChunk;
    
        cb.push({1.0, 2.0, 3.0});
        cb.printBuffer();  // Expected: 1.0 2.0 3.0
    
        cb.push({4.0, 5.0, 6.0});
        cb.printBuffer();  // Expected: 2.0 3.0 4.0 5.0 6.0
    
        cb.getLatestChunk(3, latestChunk);
        for (float f : latestChunk) {
            std::cout << f << " ";  // Expected: 4.0 5.0 6.0
        }
        std::cout << "\n";
    
        cb.push({7.0, 8.0});
        cb.getLatestChunk(4, latestChunk);
        for (float f : latestChunk) {
            std::cout << f << " ";  // Expected: 5.0 6.0 7.0 8.0
        }
        std::cout << "\n";
    
        return 0;
    }
    ```

## environment

- [ ] @umgebung ==rename `loop` to `update` to comply with processing naming ( of `loop()` `noLoop()` )==
- [ ] @umgebung ==add option to run audio in own thread== ( see https://chatgpt.com/share/67dfc699-1d34-8004-a9a9-40716713ba2f )
- [ ] @umgebung add `set_window_title` with `SDL_SetWindowTitle(window, “TITLE”);` in subsystem
- [ ] @umgebung @maybe iterate in reverse order through subsytems so that graphics is last to be exectued in `draw_post` … same for `setup_post`

## credits + licenses

- [ ] @umgebung add `tinyfiledialogs` to credtis
- [ ] @umgebung add `SimplexNoise` to credits
- [ ] @umgebung add `glm`
- [ ] @umgebung add `stb_image_write`
- [ ] @umgebung add `FreeType2` + `Harfbuzz`

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
- [ ] make a step-by-step guide of how to create a library ( send to leo )
 ```
                     + -> emit_shape_stroke_line_strip -> triangulate, transform to world/screen space
 begin-end-shape ->  |
                     + -> emit_shape_fill_triangles -> buffer, transform to world/screen space
 ```

## examples

style:

```C
background(0.85f);        // grey
fill(0.5f, 0.85f, 1.0f);  // blue
fill(1.0f, 0.25f, 0.35f); // red
stroke(0.0f);             // black
```

- [ ] @umgebung start a series of *educated* examples
- [ ] @umgebung start dedicated font example
- [ ] @umgebung try with font [Inter](https://rsms.me/inter/)
- [ ] @umgebung @example add nice stroke example with rotierender box, square, open-closed-shape + linen im 0.5,0.85,1.0 style
- [ ] @umgebung @example add example for library
- [ ] @umgebung add example that moves around the application window @maybe

## community

- [ ] @umgebung post on dev blog
- [ ] @umgebung on discord https://d3-is.de/umgebung ( https://discord.gg/hrckzRaW7g )
- [ ] @umgebung ask people for support and feature request etcetera … on github ( + discord )

## libraries

- [ ] gamepad ( e.g XBox controller as library or subsytem )

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
- [ ] @umgebung PVector vs glm::vec3 at least some functions to convert between the two

## Archived

- [x] @umgebung byte the bullet and test offscreen rendering in `Advanced` @archived(2025-04-01) @from(Umgebung / TODO + NOTES > unsorted) @done(2025-04-01)
- [x] @umgebung release end of the week ( need more examples until then ) @archived(2025-04-01) @from(Umgebung / TODO + NOTES > unsorted) @done(2025-04-01)
- [x] @umgebung add `curve` @archived(2025-04-01) @from(Umgebung / TODO + NOTES > graphics) @done(2025-04-01)
- [x] @umgebung try PGraphics for offscreen rendering @archived(2025-04-01) @from(Umgebung / TODO + NOTES > graphics) @done(2025-04-01)
