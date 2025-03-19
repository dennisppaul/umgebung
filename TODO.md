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
- [ ] check out processing s line shader 

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

## Archived

- [x] enable smooth lines: @archived(2025-03-18) @from(Umgebung / TODO > Graphics > Lines) @done(2025-03-18)
    ```C
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    ```
- [x] check maximum line width: @archived(2025-03-18) @from(Umgebung / TODO > Graphics > Lines) @done(2025-03-18)
    ```C
    GLfloat lineWidthRange[2] = {0.0f, 0.0f};
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
    std::cout << "Supported line width range: " << lineWidthRange[0] << " to " << lineWidthRange[1] << std::endl;
    ```
- [x] use unordered lists to collect renderbatches: @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
    ```C
    std::unordered_map<GLuint, std::vector<Vertex>> render_vertex_batches;
    for (const auto& [texture_id, vertices] : render_vertex_batches) {
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    ```
- [x] create a VBO for every primitve: @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
    | Primitive         | Description                                                                           | num min vertices |
    | ----------------- | ------------------------------------------------------------------------------------- | ---------------- |
    | GL_POINTS         | individual points.                                                                    | 1                |
    | GL_LINES          | disconnected line segments (each pair of vertices forms a line)                       | 2                |
    | GL_TRIANGLES      | independent triangles (every 3 vertices form a triangle)                              | 3                |
    | GL_LINE_STRIP     | continuous line connecting all vertices                                               | begin-end?       |
    | GL_LINE_LOOP      | see GL_LINE_STRIP, but closes the loop                                                | begin-end?       |
    | GL_TRIANGLE_STRIP | strip of connected triangles (each new vertex forms a triangle with the previous two) | begin-end?       |
    | GL_TRIANGLE_FAN   | fan-shaped triangle mesh (all triangles share the first vertex).                      | begin-end?       |
    
    maybe a quad primitive would be helpful with GL_TRIANGLE_FAN. the processing library supports the following primitve shapes in begin-end-shape: POINTS, LINES, TRIANGLES, TRIANGLE_FAN, TRIANGLE_STRIP, QUADS, and QUAD_STRIP
- [x] and this updates the line vertices: @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
    ```C
    void line(float x1, float y1, float z1, float x2, float y2, float z2) {
        // TODO multiply transformation matrix first?
        // Update the vertex data
        glm::vec3 newLineVertices[] = {
            glm::vec3(x1, y1, z1),
            glm::vec3(x2, y2, z2)
        };
        
        // Bind VBO and update only the vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newLineVertices), newLineVertices);
        
        // Draw the updated line
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 2);
    }
    ```
- [x] texture coordinates currently go from `[0...1]` in processing  @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
- [x] emulate `GL_LINES` + `GL_LINE_STRIP` in  @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
    - `IM_render_end_shape`
    - `IM_render_line`
    - `IM_render_rect` 
    - `IM_render_ellipse`
- [x] remove all reference to OpenGL in `PImage` @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
- [x] in *immediate mode* expand lines into quads ( or triangles ) and render them as begin-end-shapes @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
- [x] add [`libtess2`](https://github.com/memononen/libtess2) ( fast, 3D ) or [`earcut.hpp`](https://github.com/mapbox/earcut.hpp) ( faster, 2D only ) + `glm` to instructions and brew install file @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
- [x] rework option to draw lines as lines primitives  @archived(2025-03-18) @from(Umgebung / TODO > Graphics) @done(2025-03-18)
    - [x] @maybe always draw *opaque* + `strokeWeight==1` as `GL_LINES`
    - [x] add option to draw smooth lines
    - [x] check *OpenGL ES* behavior
- [x] add line caps to lines rendered as quads @archived(2025-03-17) @from(Umgebung / TODO > Graphics) @done(2025-03-17)
- [x] @maybe for large begin-end-shapes consider using the shader-based model transform i.e `uModelMatrix` @archived(2025-03-03) @from(Umgebung / TODO > Graphics) @done(2025-03-03)
- [x] add begin-end-shape and replace as many of the drawing functions with this … for now @archived(2025-03-03) @from(Umgebung / TODO > Graphics) @done(2025-03-03)
- [x] add *dirty* flag for *model matrix* ( checking against identy for now ) @archived(2025-03-03) @from(Umgebung / TODO > Graphics) @done(2025-03-03)
- [x] add an ( inefficient ) immediate mode that draws primitives. e.g for lines: @archived(2025-03-03) @from(Umgebung / TODO > Graphics) @done(2025-03-03)
    ```C
    GLuint vao, vbo;
    glm::vec3 lineVertices[] = {
        glm::vec3(-0.5f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.0f, 0.0f)
    };
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 2);
    ```
- [x] @optimize add begin-end-lines to optimze and beautify shapes made up of multiple lines ( e.g `rect(...)` or `bezier(...)` ) @archived(2025-03-03) @from(Umgebung / TODO > Graphics) @done(2025-03-03)
- [x] fix `uModelMatrix` vs `currentMatrix` double application issue @archived(2025-03-03) @from(Umgebung / TODO > Graphics) @done(2025-03-03)
