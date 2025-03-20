# Umgebung / TODO

## Graphics

- [ ] separate transparent + non-transparent primitives
- [ ] in `PGraphicsDefault2D` implement 3D by manually transforming points onto 2D 
- [ ] add `PShape` with VAO/VBOs
- [ ] `GL_POINTS` might need some shader love ( i.e `gl_PointSize` + `gl_PointCoord` for point sprites )

### Lines

- [ ] since `glLineWidth` does not work reliably maybe use geometry shader:
    ```C
    #version 330 core
    layout(lines) in; // Input: a single line segment (two vertices)
    layout(triangle_strip, max_vertices = 4) out; // Output: a quad (four vertices)
    
    uniform float uLineWidth; // Line width in screen space
    uniform mat4 uProjection;
    
    void main() {
        vec2 direction = normalize(gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy);
        vec2 normal = vec2(-direction.y, direction.x); // Perpendicular vector
        normal *= uLineWidth * 0.5; // Scale by half the width
    
        vec4 p0 = gl_in[0].gl_Position;
        vec4 p1 = gl_in[1].gl_Position;
    
        gl_Position = uProjection * vec4(p0.xy + normal, p0.z, p0.w);
        EmitVertex();
    
        gl_Position = uProjection * vec4(p0.xy - normal, p0.z, p0.w);
        EmitVertex();
    
        gl_Position = uProjection * vec4(p1.xy + normal, p1.z, p1.w);
        EmitVertex();
    
        gl_Position = uProjection * vec4(p1.xy - normal, p1.z, p1.w);
        EmitVertex();
    
        EndPrimitive();
    }
    ```
- [ ] check out processing s line shader … it turns out it *encodes* additional line information ( e.g direction ) to be expanded in the shader … hmmmm, research further

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

- [ ] @umgebung move imgui to submodule
- [ ] @umgebung start a series of *educated* examples
- [ ] @umgebung design logo ( or check existing one )
- [ ] @umgebung make a step-by-step guide of how to create a library ( send to leo )
- [ ] @umgebung on discord https://d3-is.de/umgebung ( https://discord.gg/hrckzRaW7g )
- [ ] @umgebung check if `GLEW_ARB_buffer_storage` may be an option for fast data transfer ( i.e dynamic vertex data )
    ```C
    if (!GLEW_ARB_buffer_storage) {
        std::cerr << "Persistent mapped buffers not supported on this hardware!" << std::endl;
    }
    ```

## Research

- [ ] @umgebung test with plugdata … again https://electro-smith.us18.list-manage.com/track/click?u=b473fcb56373b822c9fef8190&id=ff45d69a66&e=734f91f8ec

## Weekend Projects

- [ ] Density Particle dingdong ( as shader and as normal )
- [ ] Radiance Cascades lighting shader 
