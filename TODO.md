# Umgebung / TODO + NOTES

this is a VERY unsorted todo list and a note pad.

## Graphics

- [ ] @umgebung add exporters ( `saveImage()`, `savePDF()`, `saveOBJ()` )
- [ ] in `PGraphicsDefault2D` implement 3D by manually transforming points onto 2D 
- [ ] @umgebung add camera ( see https://chatgpt.com/share/67dfbe72-855c-8004-8b37-28d10d5c7ab3 )
- [ ] @umgebung add normals to sphere and box shapes
- [ ] `GL_POINTS` might need some shader love ( i.e `gl_PointSize` + `gl_PointCoord` for point sprites ) @later
- [ ] @umgebung add `PShape`-based fonts
- [ ] separate transparent + non-transparent primitives
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
- [ ] @umgebung maybe add an option to return the generated triangles with endShape()
- [ ] @umgebung add lighting ( see processing shaders `$HOME/Documents/dev/processing/git/processing4/core/src/processing/opengl/shaders` )

## Audio

- [ ] @umgebung basic audio classes 
    - [x] oscillator ( wavetable )
    - [x] sampler
    - [x] filter ( low, high, band )
    - [x] envelope + adsr
    - [ ] trigger/beat

## Environment

- [ ] @umgebung ==add option to run audio in own thread== ( see https://chatgpt.com/share/67dfc699-1d34-8004-a9a9-40716713ba2f )
- [ ] @umgebung add `set_window_title` with `SDL_SetWindowTitle(window, “TITLE”);` in subsystem
    - [ ] @umgebung fix set window title ( default to `$PROJECT_NAME`from CMake )
- [ ] @umgebung @maybe iterate in reverse order through subsytems so that graphics is last to be exectued in `draw_post` … same for `setup_post`

## Building + Platforms

- [ ] add `brew install sdl3_ttf`
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
    - iOS manual https://wiki.libsdl.org/SDL3/README/ios

### OpenGL ES

- [ ] @umgebung try to run this with OpenGL ES window https://github.com/ravbug/sdl3-sample
- [ ] @umgebung what’s with ANGLE?
    - @research test OpenGL ES emulation ANGLE with SDL https://gist.github.com/SasLuca/307a523d2c6f2900af5823f0792a8a93

## Documentation

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

## Examples

style:

```C
background(0.85f);        // grey  (0xD8D8D8)
fill(0.5f, 0.85f, 1.0f);  // blue  (0x7FD8FF)
fill(1.0f, 0.25f, 0.35f); // red   (0xFF3F59)
stroke(0.0f);             // black (0x000000)
```

- [ ] @umgebung start a series of *educated* examples
- [ ] @umgebung start dedicated font example
- [ ] @umgebung try with font [Inter](https://rsms.me/inter/)
- [ ] @umgebung @example add nice stroke example with rotierender box, square, open-closed-shape + line 0.5,0.85,1.0 style
- [ ] @umgebung @example add example for library
- [ ] @umgebung add example that moves around the application window @maybe

## Community

- [ ] @umgebung on discord https://d3-is.de/umgebung ( https://discord.gg/hrckzRaW7g )
- [ ] @umgebung ask people for support and feature request etcetera … on github ( + discord )

## Libraries

- [ ] gamepad ( e.g XBox controller as library or subsytem )

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

- [ ] @umgebung rename PFont, PImage, PVector etcetera to UFont, UImage, UVector
- [ ] @umgebung PVector vs glm::vec3 at least some functions to convert between the two
