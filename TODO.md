# Umgebung / TODO

## Graphics

- [ ] separate transparent + non-transparent primitives
- [ ] in `PGraphicsDefault2D` implement 3D by manually transforming points onto 2D 

## Build

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

## Unsorted

- [ ] start a series of *educated* examples
- [ ] design logo ( or check existing one )
- [ ] make a step-by-step guide of how to create a library ( send to leo )

## Research

- [ ] test with plugdata … again https://electro-smith.us18.list-manage.com/track/click?u=b473fcb56373b822c9fef8190&id=ff45d69a66&e=734f91f8ec

## Weekend Projects

- [ ] Density Particle dingdong ( as shader and as normal )
- [ ] Radiance Cascades lighting shader 

## Archived

- [x] check if `GLEW_ARB_buffer_storage` may be an option for fast data transfer ( i.e dynamic vertex data ) @note("it s not available in all OpenGL ( i.e `ARB` ), also it apparently can introduce sync issues … maybe leave it for special cases then") @archived(2025-03-23) @from(Umgebung / TODO > Unsorted) @done(2025-03-23)
    ```C
    if (!GLEW_ARB_buffer_storage) {
        std::cerr << "Persistent mapped buffers not supported on this hardware!" << std::endl;
    }
    ```
- [x] check out processing s line shader … it turns out it *encodes* additional line information ( e.g direction ) to be expanded in the shader … hmmmm, research further @archived(2025-03-23) @from(Umgebung / TODO > Graphics > Lines) @done(2025-03-23)
- [x] add `PShape` with VAO/VBOs @archived(2025-03-23) @from(Umgebung / TODO > Graphics) @done(2025-03-23) @note("not adding PShape for now it is too much work")
