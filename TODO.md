# Umgebung / TODO

## Graphics

- [ ] separate transparent + non-transparent primitives
- [ ] add line caps to lines rendered as quads
- [ ] rework option to draw lines as lines primitives 
    - [ ] @maybe always draw *opaque* + `strokeWeight==1` as `GL_LINES`
    - [ ] add option to draw smooth lines
    - [ ] check *OpenGL ES* behavior
- [ ] in `PGraphicsDefault2D` implement 3D by manually transforming points onto 2D 
- [ ] add [`libtess2`](https://github.com/memononen/libtess2) ( fast, 3D ) or [`earcut.hpp`](https://github.com/mapbox/earcut.hpp) ( faster, 2D only ) + `glm` to instructions and brew install file
- [ ] use unordered lists to collect renderbatches:
    ```C
    std::unordered_map<GLuint, std::vector<Vertex>> render_vertex_batches;
    for (const auto& [texture_id, vertices] : render_vertex_batches) {
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    ```
- [ ] in *immediate mode* expand lines into quads ( or triangles ) and render them as begin-end-shapes
- [ ] remove all reference to OpenGL in `PImage`
- [ ] emulate `GL_LINES` + `GL_LINE_STRIP` in 
    - [ ] `IM_render_end_shape`
    - [ ] `IM_render_line`
    - [ ] `IM_render_rect` 
    - [ ] `IM_render_ellipse`
- [ ] add `PShape` with VAO/VBOs
- [ ] `GL_POINTS` might need some shader love ( i.e `gl_PointSize` + `gl_PointCoord` for point sprites )
- [ ] texture coordinates currently go from `[0...1]` in processing 

and this updates the line vertices:

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

create a VBO for every primitve:

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

### Lines

check maximum line width:

```C
GLfloat lineWidthRange[2] = {0.0f, 0.0f};
glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
std::cout << "Supported line width range: " << lineWidthRange[0] << " to " << lineWidthRange[1] << std::endl;
```

- enable smooth lines:

```C
glEnable(GL_LINE_SMOOTH);
glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
```

since `glLineWidth` does not work reliably maybe use geometry shader:

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

## Build

- [ ] add `brew install sdl3_ttf`
- [ ] add `brew install glm`

## Archived

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
