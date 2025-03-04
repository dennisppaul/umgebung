#include <iostream>
#include <vector>

#include <GL/glew.h>

#include "Umgebung.h"
#include "PGraphicsOpenGLv33.h"

using namespace umgebung;

PGraphicsOpenGLv33::PGraphicsOpenGLv33() : PImage(0, 0, 0) {}

void PGraphicsOpenGLv33::strokeWeight(const float weight) {
    stroke_weight = weight;
}

void PGraphicsOpenGLv33::background(const float a, const float b, const float c, const float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // TODO should this also flush bins?
}

void PGraphicsOpenGLv33::background(const float a) {
    background(a, a, a);
}

// NOTE: done
void PGraphicsOpenGLv33::rect(const float x, const float y, const float width, const float height) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        IM_render_rect(x, y, width, height);
    }
    if (render_mode == RENDER_MODE_RETAINED) {
        RM_render_rect(x, y, width, height);
    }
}

// NOTE: done
void PGraphicsOpenGLv33::ellipse(const float x, const float y, const float width, const float height) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        IM_render_ellipse(x, y, width, height);
    }
    if (render_mode == RENDER_MODE_RETAINED) {
        RM_render_ellipse(x, y, width, height);
    }
}

// NOTE: done
void PGraphicsOpenGLv33::circle(const float x, const float y, const float diameter) {
    ellipse(x, y, diameter, diameter);
}

// NOTE: done
void PGraphicsOpenGLv33::line(const float x1, const float y1, const float x2, const float y2) {
    if (!color_stroke.active) {
        return;
    }
    line(x1, y1, 0, x2, y2, 0);
}

// NOTE: done
void PGraphicsOpenGLv33::line(const float x1, const float y1, const float z1,
                              const float x2, const float y2, const float z2) {
    if (!color_stroke.active) {
        return;
    }
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        IM_render_line(x1, y1, z1, x2, y2, z2);
    }
    if (render_mode == RENDER_MODE_RETAINED) {
        RM_render_line(x1, y1, z1, x2, y2, z2);
    }
}

// NOTE: done
void PGraphicsOpenGLv33::linse(const float x1, const float y1,
                               const float x2, const float y2) {
    line(x1, y1, x2, y2);
}

// NOTE: done
void PGraphicsOpenGLv33::triangle(const float x1, const float y1, const float z1,
                                  const float x2, const float y2, const float z2,
                                  const float x3, const float y3, const float z3) {
    beginShape(POLYGON);
    vertex(x1, y1, z1);
    vertex(x2, y2, z2);
    vertex(x3, y3, z3);
    endShape(CLOSE);
}

// NOTE: done
void PGraphicsOpenGLv33::bezier(const float x1, const float y1,
                                const float x2, const float y2,
                                const float x3, const float y3,
                                const float x4, const float y4) {
    if (!color_stroke.active) {
        return;
    }
    if (bezier_detail < 2) {
        return;
    }

    const int   segments = bezier_detail;
    const float step     = 1.0f / static_cast<float>(segments);

    beginShape(LINE_STRIP);
    for (int i = 0; i < segments; ++i) {
        const float t = static_cast<float>(i) * step;
        const float u = 1.0f - t;

        const float b0 = u * u * u;
        const float b1 = 3 * u * u * t;
        const float b2 = 3 * u * t * t;
        const float b3 = t * t * t;

        const float x = b0 * x1 + b1 * x2 + b2 * x3 + b3 * x4;
        const float y = b0 * y1 + b1 * y2 + b2 * y3 + b3 * y4;

        if (i > 0) {
            vertex(x, y);
        }
    }
    endShape();
}

// NOTE: done
void PGraphicsOpenGLv33::bezier(const float x1, const float y1, const float z1,
                                const float x2, const float y2, const float z2,
                                const float x3, const float y3, const float z3,
                                const float x4, const float y4, const float z4) {
    if (!color_stroke.active) {
        return;
    }
    if (bezier_detail < 2) {
        return;
    }

    const int   segments = bezier_detail;
    const float step     = 1.0f / static_cast<float>(segments);

    beginShape(LINE_STRIP);
    for (int i = 0; i < segments; ++i) {
        const float t = static_cast<float>(i) * step;
        const float u = 1.0f - t;

        const float b0 = u * u * u;
        const float b1 = 3 * u * u * t;
        const float b2 = 3 * u * t * t;
        const float b3 = t * t * t;

        const float x = b0 * x1 + b1 * x2 + b2 * x3 + b3 * x4;
        const float y = b0 * y1 + b1 * y2 + b2 * y3 + b3 * y4;
        const float z = b0 * z1 + b1 * z2 + b2 * z3 + b3 * z4;

        if (i > 0) {
            vertex(x, y, z);
        }
    }
    endShape();
}

/* --- UTILITIES --- */

void PGraphicsOpenGLv33::RM_add_texture_id_to_render_batch(const std::vector<float>& vertices,
                                                           const int                 num_vertices,
                                                           const GLuint              batch_texture_id) {
#ifdef USE_UNORDERED_MAP
    renderBatches[batch_texture_id].insert(renderBatches[batch_texture_id].end(), vertices.begin(), vertices.end());
#else
    const unsigned long fill_vertices_count_xyz_rgba_uv = vertices.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
    if (renderBatches.empty() || renderBatches.back().texture_id != batch_texture_id) {
        renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - num_vertices, num_vertices, batch_texture_id);
    } else {
        renderBatches.back().num_vertices += num_vertices;
    }
#endif
}

static bool intersect_lines(const glm::vec2& p1, const glm::vec2& d1,
                            const glm::vec2& p2, const glm::vec2& d2,
                            glm::vec3& intersection) {
    const float det = d1.x * d2.y - d1.y * d2.x;

    if (fabs(det) < 1e-6f) {
        return false; // Parallel or coincident lines
    }

    const float t = ((p2.x - p1.x) * d2.y - (p2.y - p1.y) * d2.x) / det;

    intersection = glm::vec3(p1 + t * d1, 0);
    return true;
}

static bool are_almost_parallel(const glm::vec3& n1, const glm::vec3& n2, const float epsilon = 0.01f) {
    const float dotProduct = glm::dot(n1, n2);
    return -dotProduct > (1.0f - epsilon); // Closer to 1 or -1 means nearly parallel
    // return fabs(dotProduct) > (1.0f - epsilon); // Closer to 1 or -1 means nearly parallel
}

// NOTE: done
void PGraphicsOpenGLv33::bezierDetail(const int detail) {
    bezier_detail = detail;
}

// NOTE: done
void PGraphicsOpenGLv33::point(const float x, const float y, const float z) {
    // TODO this could be replaced by dedicated shader, also point size might not wqork in all contexts
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        beginShape(POINTS);
        vertex(x, y, z);
        endShape();
    }
    if (render_mode == RENDER_MODE_RETAINED) {
    }
}

// NOTE: done
void PGraphicsOpenGLv33::beginShape(const int shape) {
    shape_fill_vertex_cache.clear();
    shape_stroke_vertex_cache.clear();
    shape_stroke_vertex_cache_vec3_DEPRECATED.clear();
    shape_mode_cache = shape;
    shape_has_begun  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::endShape(const bool close_shape) {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        IM_render_end_shape(close_shape);
    }
    if (render_mode == RENDER_MODE_RETAINED) {
        const glm::vec4 color = as_vec4(color_stroke);
        switch (render_line_mode) {
            case RENDER_LINE_AS_QUADS_SEGMENTS_WITH_ROUND_CORNERS:
                RM_render_line_strip_as_quad_segments(shape_stroke_vertex_cache_vec3_DEPRECATED, color, close_shape, true);
                break;
            case RENDER_LINE_AS_QUADS_WITH_POINTY_CORNERS:
                RM_render_line_strip_as_connected_quads(shape_stroke_vertex_cache_vec3_DEPRECATED, color, close_shape);
                break;
            case RENDER_LINE_AS_QUADS_SEGMENTS:
            default:
                RM_render_line_strip_as_quad_segments(shape_stroke_vertex_cache_vec3_DEPRECATED, color, close_shape, false);
                break;
        }
    }
    shape_fill_vertex_cache.clear();
    shape_stroke_vertex_cache.clear();
    shape_stroke_vertex_cache_vec3_DEPRECATED.clear();
    shape_has_begun = false;
}

// NOTE: done
void PGraphicsOpenGLv33::vertex(const float x, const float y, const float z) {
    vertex(x, y, z, 0, 0);
}

// NOTE: done
void PGraphicsOpenGLv33::vertex(const float x, const float y, const float z, const float u, const float v) {
    if (!color_stroke.active && !color_fill.active) {
        return;
    }

    const glm::vec3 position{x, y, z};

    if (color_stroke.active) {
        shape_stroke_vertex_cache_vec3_DEPRECATED.emplace_back(position);
        const glm::vec4 strokeColor = as_vec4(color_stroke);
        shape_stroke_vertex_cache.emplace_back(position, strokeColor, glm::vec2{u, v});
    }

    if (color_fill.active) {
        const glm::vec4 fillColor = as_vec4(color_fill);
        shape_fill_vertex_cache.emplace_back(position, fillColor, glm::vec2{u, v});
    }
}

PFont* PGraphicsOpenGLv33::loadFont(const std::string& file, float size) {
    auto* font = new PFont(file.c_str(), size, static_cast<float>(pixel_density));
    return font;
}

void PGraphicsOpenGLv33::textFont(PFont* font) {
    fCurrentFont = font;
}

void PGraphicsOpenGLv33::textSize(float size) {
    if (fCurrentFont == nullptr) {
        return;
    }
    fCurrentFont->size(size);
}

void PGraphicsOpenGLv33::text(const char* value, float x, float y, float z) {
    text_str(value, x, y, z);
}

float PGraphicsOpenGLv33::textWidth(const std::string& text) {
    if (fCurrentFont == nullptr) {
        return 0;
    }

#ifndef DISABLE_GRAPHICS
    return fCurrentFont->textWidth(text.c_str());
#endif // DISABLE_GRAPHICS
}

void PGraphicsOpenGLv33::text_str(const std::string& text, float x, float y, float z) {
    if (fCurrentFont == nullptr) {
        return;
    }
    if (!color_fill.active) {
        return;
    }

#ifndef DISABLE_GRAPHICS
    // glColor4f(color_fill.r, color_fill.g, color_fill.b, color_fill.a);
    fCurrentFont->draw(text.c_str(), x, y, z);
#endif // DISABLE_GRAPHICS
}

// NOTE: done
PImage* PGraphicsOpenGLv33::loadImage(const std::string& filename) {
    auto* img = new PImage(filename);
    return img;
}

// NOTE: done
void PGraphicsOpenGLv33::image(PImage* img, const float x, const float y, float w, float h) {
    if (!color_fill.active) {
        return;
    }

    if (img == nullptr) {
        error("image is null");
        return;
    }

    if (w < 0) {
        w = img->width;
    }
    if (h < 0) {
        h = img->height;
    }

    // TODO move this to own method and share with `texture()`
    if (img->texture_id == NOT_INITIALIZED) {
        SHARED_upload_image_as_texture(img, true);
        if (img->texture_id == NOT_INITIALIZED) {
            error("image cannot create texture.");
            return;
        }
        console("uploaded texture image to GPU");
    }

    if (render_mode == RENDER_MODE_IMMEDIATE) {
        const uint8_t tmp_rect_mode          = rect_mode;
        const uint8_t tmp_texture_id_current = texture_id_current;
        SHARED_bind_texture(img->texture_id);
        rect(x, y, w, h);
        if (tmp_texture_id_current != img->texture_id) {
            SHARED_bind_texture(tmp_texture_id_current);
        }
        rect_mode = tmp_rect_mode;
        return;
    }

    if (render_mode == RENDER_MODE_RETAINED) {
        add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 0.0f, 0.0f);
        add_fill_vertex_xyz_rgba_uv(x + w, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 1.0f, 0.0f);
        add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 1.0f, 1.0f);

        add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 1.0f, 1.0f);
        add_fill_vertex_xyz_rgba_uv(x, y + h, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 0.0f, 1.0f);
        add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 0.0f, 0.0f);

        constexpr int       RECT_NUM_VERTICES               = 6;
        const unsigned long fill_vertices_count_xyz_rgba_uv = fill_vertices_xyz_rgba_uv.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
        if (renderBatches.empty() || renderBatches.back().texture_id != img->texture_id) {
            renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - RECT_NUM_VERTICES, RECT_NUM_VERTICES, img->texture_id);
        } else {
            renderBatches.back().num_vertices += RECT_NUM_VERTICES;
        }
    }
}

// NOTE: done
void PGraphicsOpenGLv33::image(PImage* img, const float x, const float y) {
    image(img, x, y, img->width, img->height);
}

// NOTE: done
void PGraphicsOpenGLv33::texture(PImage* img) {
    if (img == nullptr) {
        return;
    }

    // TODO move this to own method and share with `texture()`
    if (img->texture_id == NOT_INITIALIZED) {
        SHARED_upload_image_as_texture(img, true);
        if (img->texture_id == NOT_INITIALIZED) {
            error("image cannot create texture.");
            return;
        }
        console("uploaded texture image to GPU");
    }

    SHARED_bind_texture(img->texture_id);
}

// NOTE: done
void PGraphicsOpenGLv33::popMatrix() {
    if (!model_matrix_stack.empty()) {
        model_matrix_client = model_matrix_stack.back();
        model_matrix_stack.pop_back();
    }
}

// NOTE: done
void PGraphicsOpenGLv33::pushMatrix() {
    model_matrix_stack.push_back(model_matrix_client);
}

// NOTE: done
void PGraphicsOpenGLv33::translate(const float x, const float y, const float z) {
    model_matrix_client = glm::translate(model_matrix_client, glm::vec3(x, y, z));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::rotateX(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(1.0f, 0.0f, 0.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::rotateY(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::rotateZ(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::rotate(const float angle) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::rotate(const float angle, const float x, const float y, const float z) {
    model_matrix_client = glm::rotate(model_matrix_client, angle, glm::vec3(x, y, z));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::scale(const float x) {
    model_matrix_client = glm::scale(model_matrix_client, glm::vec3(x, x, x));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::scale(const float x, const float y) {
    model_matrix_client = glm::scale(model_matrix_client, glm::vec3(x, y, 1));
    model_matrix_dirty  = true;
}

// NOTE: done
void PGraphicsOpenGLv33::scale(const float x, const float y, const float z) {
    model_matrix_client = glm::scale(model_matrix_client, glm::vec3(x, y, z));
    model_matrix_dirty  = true;
}

void PGraphicsOpenGLv33::pixelDensity(int density) {
    pixel_density = density;
}

void PGraphicsOpenGLv33::hint(const uint16_t property) {
    if (property == HINT_ENABLE_SMOOTH_LINES) {
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
}

void PGraphicsOpenGLv33::beginDraw() {
    // TODO this is NOT OpenGL3.3 code.
#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    /* save state */
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_FBO);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // bind the FBO for offscreen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
    glViewport(0, 0, framebuffer.width, framebuffer.height);

    /* setup projection and modelview matrices */

    glMatrixMode(GL_PROJECTION);
    // save the current projection matrix
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, framebuffer.width, 0, framebuffer.height, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    // save the current modelview matrix
    glPushMatrix();
    glLoadIdentity();
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER

    // TODO maybe replace the code above with this:
    //      make sure to also check `draw_pre()`
    // reset_matrices();
    // prepare_frame();
}

void PGraphicsOpenGLv33::endDraw() {
#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    // restore projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    /* restore state */
    glBindFramebuffer(GL_FRAMEBUFFER, previous_FBO); // Restore the previously bound FBO
    glPopMatrix();
    glPopAttrib();
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER

    // TODO maybe replace the code above with this:
    //      make sure to also check `draw_post()`
    // flush_fill();
    // flush_stroke();
}

void PGraphicsOpenGLv33::bind() {
    if (texture_id) {
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
}

void PGraphicsOpenGLv33::prepare_frame() {
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(fill_shader_program);

        // Upload matrices
        const GLint projLoc = glGetUniformLocation(fill_shader_program, "uProjection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix_3D));

        const GLint viewLoc = glGetUniformLocation(fill_shader_program, "uViewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

        const GLint modelLoc = glGetUniformLocation(fill_shader_program, "uModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix_shader));

        texture_id_current = 0;
        SHARED_bind_texture(texture_id_solid_color);
    }
}

void PGraphicsOpenGLv33::reset_matrices() {
    model_matrix_shader = glm::mat4(1.0f);
    model_matrix_client = glm::mat4(1.0f);
    model_matrix_dirty  = false;

    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

    // Orthographic projection
    projection_matrix_2D = glm::ortho(0.0f, width, height, 0.0f);

    const float fov            = DEFAULT_FOV;                       // distance from the camera = screen height
    const float cameraDistance = (height / 2.0f) / tan(fov / 2.0f); // 1 unit = 1 pixel

    // Perspective projection
    projection_matrix_3D = glm::perspective(fov, width / height, 0.1f, 10000.0f);

    view_matrix = glm::lookAt(
        glm::vec3(width / 2.0f, height / 2.0f, -cameraDistance), // Flip Z to fix X-axis
        glm::vec3(width / 2.0f, height / 2.0f, 0.0f),            // Look at the center
        glm::vec3(0.0f, -1.0f, 0.0f)                             // Keep Y-up as normal
    );
}

void PGraphicsOpenGLv33::init(uint32_t* pixels,
                              const int width,
                              const int height,
                              int       format,
                              bool      generate_mipmap) {
    this->width        = static_cast<float>(width);
    this->height       = static_cast<float>(height);
    framebuffer.width  = width;
    framebuffer.height = height;

    stroke_shader_program = SHARED_build_shader(vertex_shader_source_simple(), fragment_shader_source_simple());
    init_stroke_vertice_buffers();
    fill_shader_program = SHARED_build_shader(vertex_shader_source_texture(), fragment_shader_source_texture());
    init_fill_vertice_buffers();
    create_solid_color_texture();
    SHARED_bind_texture(texture_id_solid_color);

    reset_matrices();

#ifdef PGRAPHICS_RENDER_INTO_FRAMEBUFFER
    glGenFramebuffers(1, &framebuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
    glGenTextures(1, &framebuffer.texture);
    glBindTexture(GL_TEXTURE_2D, framebuffer.texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 framebuffer.width,
                 framebuffer.height,
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Handle framebuffer incomplete error
        std::cerr << "ERROR Framebuffer is not complete!" << std::endl;
    }
    glViewport(0, 0, framebuffer.width, framebuffer.height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
#endif // PGRAPHICS_RENDER_INTO_FRAMEBUFFER
}

/* additional */

bool PGraphicsOpenGLv33::SHARED_upload_image_as_texture(PImage* image, const bool generate_texture_mipmapped) {
    if (image->pixels == nullptr) {
        error("Failed to upload texture because pixels are null");
        return false;
    }

    GLuint mTextureID;
    glGenTextures(1, &mTextureID);

    if (mTextureID == 0) {
        error("Failed to generate texture ID");
        return false;
    }

    image->texture_id = static_cast<int>(mTextureID);
    glBindTexture(GL_TEXTURE_2D, image->texture_id);

    // Set texture parameters
    if (generate_texture_mipmapped) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Load image data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 static_cast<GLint>(image->width),
                 static_cast<GLint>(image->height),
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE,
                 image->pixels);
    if (generate_texture_mipmapped) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    return true;
}


void PGraphicsOpenGLv33::flush_stroke() {
    if (stroke_vertices_xyz_rgba.empty()) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, stroke_VBO_xyz_rgba);
    const unsigned long size = stroke_vertices_xyz_rgba.size() * sizeof(float);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), stroke_vertices_xyz_rgba.data());

    glUseProgram(stroke_shader_program);

    // Upload matrices
    const GLint projLoc = glGetUniformLocation(stroke_shader_program, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix_3D)); // or projection2D

    const GLint viewLoc = glGetUniformLocation(stroke_shader_program, "uViewMatrix");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

    const GLint matrixLoc = glGetUniformLocation(stroke_shader_program, "uModelMatrix");
    glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix_shader));

    glBindVertexArray(stroke_VAO_xyz_rgba);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(stroke_vertices_xyz_rgba.size()) / NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA);
    glBindVertexArray(0);

    stroke_vertices_xyz_rgba.clear();
}

void PGraphicsOpenGLv33::flush_fill() {
    if (fill_vertices_xyz_rgba_uv.empty()) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);
    const unsigned long size = fill_vertices_xyz_rgba_uv.size() * sizeof(float);
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), fill_vertices_xyz_rgba_uv.data());

    glUseProgram(fill_shader_program);

    // Upload matrices
    const GLint projLoc = glGetUniformLocation(fill_shader_program, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix_3D));

    const GLint viewLoc = glGetUniformLocation(fill_shader_program, "uViewMatrix");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

    const GLint modelLoc = glGetUniformLocation(fill_shader_program, "uModelMatrix");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix_shader));

    // Bind textures per batch
    glBindVertexArray(fill_VAO_xyz_rgba_uv);
#ifdef USE_UNORDERED_MAP
    for (const auto& [texture_id, vertices]: renderBatches) {
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV);
    }
#else
    for (const auto& batch: renderBatches) {
        glBindTexture(GL_TEXTURE_2D, batch.texture_id);
        glDrawArrays(GL_TRIANGLES, batch.start_index, batch.num_vertices);
    }
#endif
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    fill_vertices_xyz_rgba_uv.clear();
    renderBatches.clear();
}

void PGraphicsOpenGLv33::fill_resize_buffer(const uint32_t newSize) {
    // Create a new buffer
    GLuint newVBO;
    glGenBuffers(1, &newVBO);
    glBindBuffer(GL_ARRAY_BUFFER, newVBO);
    glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW); // Allocate new size

    // Copy old data to new buffer
    glBindBuffer(GL_COPY_READ_BUFFER, fill_VBO_xyz_rgba_uv);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, fill_max_buffer_size);

    // Delete old buffer and update references
    glDeleteBuffers(1, &fill_VBO_xyz_rgba_uv);
    fill_VBO_xyz_rgba_uv = newVBO;
    fill_max_buffer_size = newSize;

    glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);
}

void PGraphicsOpenGLv33::add_transformed_fill_vertex_xyz_rgba_uv(const glm::vec3& position,
                                                                 const glm::vec4& color,
                                                                 const float u, const float v) {
    fill_vertices_xyz_rgba_uv.push_back(position.x); // Position
    fill_vertices_xyz_rgba_uv.push_back(position.y); // Position
    fill_vertices_xyz_rgba_uv.push_back(position.z); // Position
    fill_vertices_xyz_rgba_uv.push_back(color.r);    // ColorState
    fill_vertices_xyz_rgba_uv.push_back(color.g);    // ColorState
    fill_vertices_xyz_rgba_uv.push_back(color.b);    // ColorState
    fill_vertices_xyz_rgba_uv.push_back(color.a);    // ColorState
    fill_vertices_xyz_rgba_uv.push_back(u);          // Texture
    fill_vertices_xyz_rgba_uv.push_back(v);          // Texture
}

void PGraphicsOpenGLv33::add_fill_vertex_xyz_rgba_uv(const glm::vec3 position,
                                                     const glm::vec4 color,
                                                     const glm::vec2 tex_coords) {
    add_fill_vertex_xyz_rgba_uv(position.x, position.y, position.z,
                                color.r, color.g, color.b, color.a,
                                tex_coords.x, tex_coords.y);
}

void PGraphicsOpenGLv33::add_fill_vertex_xyz_rgba_uv_raw(const glm::vec3 position,
                                                         const glm::vec4 color,
                                                         const glm::vec2 tex_coords) {
    fill_vertices_xyz_rgba_uv.push_back(position.x);   // Position
    fill_vertices_xyz_rgba_uv.push_back(position.y);   // Position
    fill_vertices_xyz_rgba_uv.push_back(position.z);   // Position
    fill_vertices_xyz_rgba_uv.push_back(color.r);      // ColorState
    fill_vertices_xyz_rgba_uv.push_back(color.g);      // ColorState
    fill_vertices_xyz_rgba_uv.push_back(color.b);      // ColorState
    fill_vertices_xyz_rgba_uv.push_back(color.a);      // ColorState
    fill_vertices_xyz_rgba_uv.push_back(tex_coords.x); // Texture
    fill_vertices_xyz_rgba_uv.push_back(tex_coords.y); // Texture
}

void PGraphicsOpenGLv33::to_screen_space(glm::vec3& world_position) const {
    // Transform world position to camera (view) space
    const glm::vec4 viewPos = view_matrix * model_matrix_client * glm::vec4(world_position, 1.0f);

    // Project onto clip space
    glm::vec4 clipPos = projection_matrix_3D * viewPos;

    // Perspective divide (convert to normalized device coordinates)
    if (clipPos.w != 0.0f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
    }

    // Now the coordinates are in NDC (-1 to 1 range)
    // Convert NDC to screen space (assuming viewport width and height)
    // TODO what is it? `width` or `framebuffer_width`
    // float screenX = (clipPos.x * 0.5f + 0.5f) * static_cast<float>(framebuffer_width);
    // float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * static_cast<float>(framebuffer_height);
    const float screenX = (clipPos.x * 0.5f + 0.5f) * static_cast<float>(width);
    const float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * static_cast<float>(height);

    world_position.x = screenX;
    world_position.y = screenY;
    world_position.z = 0.0f;
}

// // TODO optimize … this is inefficient AF
// void PGraphicsOpenGLv33::add_quad_line(const glm::vec3& p1,
//                                        const glm::vec3& p2,
//                                        const glm::vec4& color,
//                                        float            width,
//                                        const glm::mat4& mvp) {
//
//     glm::vec3 p1_transformed = model_matrix_client * glm::vec4(p1.x, p1.y, p1.z, 1.0f);
//     glm::vec3 p2_transformed = model_matrix_client * glm::vec4(p2.x, p2.y, p2.z, 1.0f);
//     // Transform points to clip space (multiply by MVP)
//     glm::vec4 clipP1 = mvp * glm::vec4(p1_transformed, 1.0f);
//     glm::vec4 clipP2 = mvp * glm::vec4(p2_transformed, 1.0f);
//
//     // Perform perspective division to get NDC coordinates
//     glm::vec2 ndcP1 = glm::vec2(clipP1) / clipP1.w;
//     glm::vec2 ndcP2 = glm::vec2(clipP2) / clipP2.w;
//
//     // Compute the 2D line direction
//     glm::vec2 dir = glm::normalize(ndcP2 - ndcP1);
//
//     // Compute the perpendicular vector for thickness
//     glm::vec2 perp = glm::vec2(-dir.y, dir.x) * (width / 2.0f);
//
//     // Generate the quad vertices in screen space
//     glm::vec2 v1 = ndcP1 + perp;
//     glm::vec2 v2 = ndcP1 - perp;
//     glm::vec2 v3 = ndcP2 + perp;
//     glm::vec2 v4 = ndcP2 - perp;
//
//     // // Convert back to clip space (set z and w = 1)
//     // std::vector<glm::vec3> quad = {
//     //     glm::vec3(v1, clipP1.z),
//     //     glm::vec3(v2, clipP1.z),
//     //     glm::vec3(v4, clipP2.z), // First triangle
//     //     glm::vec3(v1, clipP1.z),
//     //     glm::vec3(v4, clipP2.z),
//     //     glm::vec3(v3, clipP2.z) // Second triangle
//     // };
//     glm::vec2 tex_coords(1, 1);
//
//     add_fill_vertex_xyz_rgba_uv_raw(glm::vec3(v1, clipP1.z), color, tex_coords);
//     add_fill_vertex_xyz_rgba_uv_raw(glm::vec3(v2, clipP1.z), color, tex_coords);
//     add_fill_vertex_xyz_rgba_uv_raw(glm::vec3(v4, clipP1.z), color, tex_coords);
//
//     add_fill_vertex_xyz_rgba_uv_raw(glm::vec3(v1, clipP1.z), color, tex_coords);
//     add_fill_vertex_xyz_rgba_uv_raw(glm::vec3(v4, clipP1.z), color, tex_coords);
//     add_fill_vertex_xyz_rgba_uv_raw(glm::vec3(v3, clipP1.z), color, tex_coords);
// }

void PGraphicsOpenGLv33::add_fill_vertex_xyz_rgba_uv(const float x, const float y, const float z,
                                                     const float r, const float g, const float b, const float a,
                                                     const float u, const float v) {
    // Each vertex consists of 9 floats (x, y, z, r, g, b, u, v)
    // uint32_t vertexSize = NUM_VERTEX_ATTRIBUTES * sizeof(float);
    console("add_fill_vertex_xyz_rgba_uv?");
    if ((fill_vertices_xyz_rgba_uv.size() + NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV) * sizeof(float) > fill_max_buffer_size) {
        fill_resize_buffer(fill_max_buffer_size + VBO_BUFFER_CHUNK_SIZE);
    }

    const glm::vec4 transformed = model_matrix_client * glm::vec4(x, y, z, 1.0f); // Apply transformation
    fill_vertices_xyz_rgba_uv.push_back(transformed.x);                           // Position
    fill_vertices_xyz_rgba_uv.push_back(transformed.y);                           // Position
    fill_vertices_xyz_rgba_uv.push_back(transformed.z);                           // Position
    fill_vertices_xyz_rgba_uv.push_back(r);                                       // Color
    fill_vertices_xyz_rgba_uv.push_back(g);                                       // Color
    fill_vertices_xyz_rgba_uv.push_back(b);                                       // Color
    fill_vertices_xyz_rgba_uv.push_back(a);                                       // Color
    fill_vertices_xyz_rgba_uv.push_back(u);                                       // Texture
    fill_vertices_xyz_rgba_uv.push_back(v);                                       // Texture
}

void PGraphicsOpenGLv33::add_stroke_vertex_xyz_rgba(const float x, const float y, const float z,
                                                    const float r, const float g, const float b, const float a) {
    // Each vertex consists of 7 floats (x, y, z, r, g, b, u, v)
    // uint32_t vertexSize = NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA * sizeof(float);
    if ((stroke_vertices_xyz_rgba.size() + NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA) * sizeof(float) > stroke_max_buffer_size) {
        std::cerr << "Stroke buffer is full!" << std::endl;
        // TODO create version for stroke buffer
        // stroke_resize_buffer(stroke_max_buffer_size + VBO_BUFFER_CHUNK_SIZE);
    }

    const glm::vec4 transformed = model_matrix_client * glm::vec4(x, y, z, 1.0f); // Apply transformation
    stroke_vertices_xyz_rgba.push_back(transformed.x);                            // Position
    stroke_vertices_xyz_rgba.push_back(transformed.y);                            // Position
    stroke_vertices_xyz_rgba.push_back(transformed.z);                            // Position
    stroke_vertices_xyz_rgba.push_back(r);                                        // Color
    stroke_vertices_xyz_rgba.push_back(g);                                        // Color
    stroke_vertices_xyz_rgba.push_back(b);                                        // Color
    stroke_vertices_xyz_rgba.push_back(a);                                        // Color
}

void PGraphicsOpenGLv33::init_stroke_vertice_buffers() {
    glGenVertexArrays(1, &stroke_VAO_xyz_rgba);
    glGenBuffers(1, &stroke_VBO_xyz_rgba);

    glBindVertexArray(stroke_VAO_xyz_rgba);

    glBindBuffer(GL_ARRAY_BUFFER, stroke_VBO_xyz_rgba);
    glBufferData(GL_ARRAY_BUFFER, stroke_max_buffer_size, nullptr, GL_DYNAMIC_DRAW);

    constexpr auto STRIDE = static_cast<GLsizei>(NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA * sizeof(float));

    // Position Attribute (Location 0) -> 3 floats (x, y, z)
    constexpr int NUM_POSITION_ATTRIBUTES = 3;
    // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
    glVertexAttribPointer(0, NUM_POSITION_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, static_cast<void*>(0)); // NOLINT(*-use-nullptr)
    glEnableVertexAttribArray(0);

    // Color Attribute (Location 1) -> 4 floats (r, g, b, a)
    constexpr int NUM_COLOR_ATTRIBUTES = 4;
    glVertexAttribPointer(1, NUM_COLOR_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<void*>(NUM_POSITION_ATTRIBUTES * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void PGraphicsOpenGLv33::init_fill_vertice_buffers() {
    glGenVertexArrays(1, &fill_VAO_xyz_rgba_uv);
    glGenBuffers(1, &fill_VBO_xyz_rgba_uv);

    glBindVertexArray(fill_VAO_xyz_rgba_uv);
    glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);

    // Allocate buffer memory (maxBufferSize = 1024 KB) but don't fill it yet
    glBufferData(GL_ARRAY_BUFFER, fill_max_buffer_size, nullptr, GL_DYNAMIC_DRAW);

    constexpr auto STRIDE = static_cast<GLsizei>(NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV * sizeof(float));

    // Position Attribute (Location 0) -> 3 floats (x, y, z)
    constexpr int NUM_POSITION_ATTRIBUTES = 3;
    // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
    glVertexAttribPointer(0, NUM_POSITION_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, static_cast<void*>(0)); // NOLINT(*-use-nullptr)
    glEnableVertexAttribArray(0);

    // Color Attribute (Location 1) -> 4 floats (r, g, b, a)
    constexpr int NUM_COLOR_ATTRIBUTES = 4;
    glVertexAttribPointer(1, NUM_COLOR_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<void*>(NUM_POSITION_ATTRIBUTES * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture Coordinate Attribute (Location 2) -> 2 floats (u, v)
    constexpr int NUM_TEXTURE_ATTRIBUTES = 2;
    glVertexAttribPointer(2, NUM_TEXTURE_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<void*>((NUM_POSITION_ATTRIBUTES + NUM_COLOR_ATTRIBUTES) * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void PGraphicsOpenGLv33::create_solid_color_texture() {
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    constexpr unsigned char whitePixel[4] = {255, 255, 255, 255}; // RGBA: White
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 1, 1,
                 0,
                 UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                 GL_UNSIGNED_BYTE,
                 whitePixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture_id_solid_color = texture_id;
}

GLuint PGraphicsOpenGLv33::SHARED_build_shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // Build shaders
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    SHARED_checkShaderCompileStatus(vertexShader);

    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    SHARED_checkShaderCompileStatus(fragmentShader);

    const GLuint mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);
    SHARED_checkProgramLinkStatus(mShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return mShaderProgram;
}

void PGraphicsOpenGLv33::SHARED_checkShaderCompileStatus(const GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
}

void PGraphicsOpenGLv33::SHARED_checkProgramLinkStatus(const GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
}

void PGraphicsOpenGLv33::printMatrix(const glm::mat4& matrix) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << matrix[j][i] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


const char* PGraphicsOpenGLv33::vertex_shader_source_texture() {
    const auto vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main() {
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
    vTexCoord = aTexCoord;
}
)";
    return vertexShaderSource;
}

const char* PGraphicsOpenGLv33::fragment_shader_source_texture() {
    const auto fragmentShaderSource = R"(
#version 330 core

in vec4 vColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    FragColor = texture(uTexture, vTexCoord) * vColor;
}
)";
    return fragmentShaderSource;
}

const char* PGraphicsOpenGLv33::vertex_shader_source_simple() {
    // Vertex Shader source ( without texture )
    const auto vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

out vec4 vColor;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main() {
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    vColor = aColor;
}
)";
    return vertexShaderSource;
}

const char* PGraphicsOpenGLv33::fragment_shader_source_simple() {
    // Fragment Shader source ( without texture )
    const auto fragmentShaderSource = R"(
#version 330 core

in vec4 vColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(vColor);
}
)";
    return fragmentShaderSource;
}

/* --- SHARED --- */

void PGraphicsOpenGLv33::SHARED_init_vertex_buffer(PrimitiveVertexBuffer& primitive) {
    // Generate and bind VAO & VBO
    glGenVertexArrays(1, &primitive.VAO);
    glBindVertexArray(primitive.VAO);

    glGenBuffers(1, &primitive.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, primitive.VBO);

    // Allocate GPU memory (without initializing data, as it will be updated before use)
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(primitive.num_vertices * sizeof(Vertex)), nullptr, GL_DYNAMIC_DRAW);

    // Set up attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tex_coord)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void PGraphicsOpenGLv33::SHARED_resize_vertex_buffer(const size_t buffer_size_bytes) {
    // Get current buffer size
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    if (buffer_size_bytes > static_cast<size_t>(bufferSize)) {
        // Allocate extra space to reduce reallocations
        const size_t growSize = std::max(static_cast<int>(buffer_size_bytes), bufferSize + static_cast<int>(VBO_BUFFER_CHUNK_SIZE));
        console("increasing vertex buffer array to ", growSize, " ( should be all good )");
        glBufferData(GL_ARRAY_BUFFER, static_cast<int>(growSize), nullptr, GL_DYNAMIC_DRAW);
    }
}

void PGraphicsOpenGLv33::SHARED_render_vertex_buffer(PrimitiveVertexBuffer&     vertex_buffer,
                                                     const GLenum               primitive_mode,
                                                     const std::vector<Vertex>& shape_vertices) {
    // Ensure there are vertices to render
    if (shape_vertices.empty()) {
        return;
    }

    // Ensure primitive is initialized
    if (vertex_buffer.uninitialized()) {
        SHARED_init_vertex_buffer(vertex_buffer);
    }

    // Update VBO with collected vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.VBO);

    constexpr bool hint_check_buffer_size = true;
    if (hint_check_buffer_size) {
        const size_t buffer_size = shape_vertices.size() * sizeof(Vertex);
        SHARED_resize_vertex_buffer(buffer_size);
    }

    // Update only necessary data
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<int>(shape_vertices.size() * sizeof(Vertex)), shape_vertices.data());

    // Bind VAO and draw the shape
    glBindVertexArray(vertex_buffer.VAO);
    // glBindTexture(GL_TEXTURE_2D, texture_id_current); // TODO why?
    glDrawArrays(primitive_mode, 0, static_cast<GLsizei>(shape_vertices.size()));

    // Unbind VAO for safety (optional)
    glBindVertexArray(0);
}

void PGraphicsOpenGLv33::SHARED_bind_texture(const GLuint bind_texture_id) {
    if (bind_texture_id != texture_id_current) {
        texture_id_current = bind_texture_id;
        glBindTexture(GL_TEXTURE_2D, texture_id_current);
    }
}

/* --- RENDER_MODE_RETAINED (RM) --- */

void PGraphicsOpenGLv33::RM_render_line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
    if (render_lines_as_quads) {
        auto p1 = glm::vec3(x1, y1, z1);
        auto p2 = glm::vec3(x2, y2, z2);

        to_screen_space(p1);
        to_screen_space(p2);

        glm::vec3 perp = p2 - p1;
        perp           = glm::normalize(perp);
        perp           = {-perp.y, perp.x, 0};
        perp *= stroke_weight * 0.5f;

        const glm::vec4 color{color_stroke.r,
                              color_stroke.g,
                              color_stroke.b,
                              color_stroke.a};

        add_transformed_fill_vertex_xyz_rgba_uv(p1 + perp, color);
        add_transformed_fill_vertex_xyz_rgba_uv(p2 + perp, color);
        add_transformed_fill_vertex_xyz_rgba_uv(p1 - perp, color);
        add_transformed_fill_vertex_xyz_rgba_uv(p1 - perp, color);
        add_transformed_fill_vertex_xyz_rgba_uv(p2 + perp, color);
        add_transformed_fill_vertex_xyz_rgba_uv(p2 - perp, color);
        RM_add_texture_id_to_render_batch(fill_vertices_xyz_rgba_uv, 6, texture_id_solid_color);
    } else {
        add_stroke_vertex_xyz_rgba(x1, y1, z1, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        add_stroke_vertex_xyz_rgba(x2, y2, z2, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
    }
}

/**
 * render line strip as individual quads per line segment, with optional round corners
 * @param points
 * @param color
 * @param close_shape
 * @param round_corners
 */
void PGraphicsOpenGLv33::RM_render_line_strip_as_quad_segments(const std::vector<glm::vec3>& points,
                                                               const glm::vec4&              color,
                                                               const bool                    close_shape,
                                                               const bool                    round_corners) {
    if (points.size() < 2) {
        return;
    }

    if (points.size() == 2) {
        // TODO replace with raw version
        line(points[0].x, points[0].y, points[0].z,
             points[1].x, points[1].y, points[1].z);
        return;
    }

    const uint32_t num_line_segments = points.size() - (close_shape ? 0 : 1);
    for (size_t i = 0; i < num_line_segments; i++) {
        const size_t ii = (i + 1) % points.size();
        line(points[i].x, points[i].y, points[i].z,
             points[ii].x, points[ii].y, points[ii].z);
        if (round_corners) {
            // TODO must be transformed to screenspace!
            RM_render_ellipse_filled(points[i].x, points[i].y, stroke_weight, stroke_weight, 8, color);
        }
    }
}

/**
 * render line strip as quads with pointy corners
 * @param points
 * @param color
 * @param close_shape
 */
void PGraphicsOpenGLv33::RM_render_line_strip_as_connected_quads(std::vector<glm::vec3>& points, const glm::vec4& color, const bool close_shape) {
    if (points.size() < 2) {
        return;
    }

    if (points.size() == 2) {
        line(points[0].x, points[0].y, points[0].z,
             points[1].x, points[1].y, points[1].z);
        return;
    }

    // Transform all points to screen space first
    std::vector<glm::vec3> screen_points = points;
    std::vector<glm::vec3> normals(screen_points.size());
    std::vector<glm::vec3> directions(screen_points.size());

    for (auto& p: screen_points) {
        to_screen_space(p);
    }

    for (size_t i = 0; i < normals.size(); i++) {
        const size_t ii   = (i + 1) % screen_points.size();
        glm::vec3&   p1   = screen_points[i];
        glm::vec3&   p2   = screen_points[ii];
        glm::vec3    dir  = p2 - p1;
        glm::vec3    perp = glm::normalize(dir);
        perp              = glm::vec3(-perp.y, perp.x, 0);
        directions[i]     = dir;
        normals[i]        = perp;
    }

    const float half_width = stroke_weight * 0.5f;
    uint32_t    vertex_count{0};

    glm::vec3 p1_left{};
    glm::vec3 p1_right{};

    const uint32_t num_line_segments = screen_points.size() + (close_shape ? 1 : 0);

    for (size_t i = 0; i < num_line_segments; i++) {
        const glm::vec3 point     = screen_points[i % screen_points.size()];
        const glm::vec3 direction = directions[i % directions.size()];
        const glm::vec3 normal    = normals[i % normals.size()];

        const glm::vec3 next_point     = screen_points[(i + 1) % screen_points.size()];
        const glm::vec3 next_direction = directions[(i + 1) % directions.size()];
        const glm::vec3 next_normal    = normals[(i + 1) % normals.size()];

        const glm::vec3 p2_left  = point + normal * half_width;
        const glm::vec3 p2_right = point - normal * half_width;
        const glm::vec3 p3_left  = next_point + next_normal * half_width;
        const glm::vec3 p3_right = next_point - next_normal * half_width;

        glm::vec3  intersection_left;
        const bool result_left = intersect_lines(p2_left, direction,
                                                 p3_left, next_direction,
                                                 intersection_left);
        glm::vec3  intersection_right;
        const bool result_right = intersect_lines(p2_right, direction,
                                                  p3_right, next_direction,
                                                  intersection_right);

        if (are_almost_parallel(normal, next_normal, 0.2f)) {
            // TODO maybe handle sharp angles
        }

        if (!result_left) {
            intersection_left = p3_left;
        }
        if (!result_right) {
            intersection_right = p3_right;
        }

        if (close_shape) {
            if (i != 0) {
                add_transformed_fill_vertex_xyz_rgba_uv(p1_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(p1_right, color);

                add_transformed_fill_vertex_xyz_rgba_uv(p1_right, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_right, color);
            }
            vertex_count += 6;
        } else {
            if (i == 0) { // first segment
                add_transformed_fill_vertex_xyz_rgba_uv(p2_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(p2_right, color);

                add_transformed_fill_vertex_xyz_rgba_uv(p2_right, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_right, color);
            } else if (i == num_line_segments - 2) { // last segment
                add_transformed_fill_vertex_xyz_rgba_uv(p1_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(next_point + normal * half_width, color);
                add_transformed_fill_vertex_xyz_rgba_uv(p1_right, color);

                add_transformed_fill_vertex_xyz_rgba_uv(p1_right, color);
                add_transformed_fill_vertex_xyz_rgba_uv(next_point + normal * half_width, color);
                add_transformed_fill_vertex_xyz_rgba_uv(next_point - normal * half_width, color);
            } else if (i < num_line_segments - 2) {
                add_transformed_fill_vertex_xyz_rgba_uv(p1_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(p1_right, color);

                add_transformed_fill_vertex_xyz_rgba_uv(p1_right, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_left, color);
                add_transformed_fill_vertex_xyz_rgba_uv(intersection_right, color);
            }
            vertex_count += 6;
            // TODO this could be used for round caps
        }
        p1_left  = intersection_left;
        p1_right = intersection_right;
    }

    RM_add_texture_id_to_render_batch(fill_vertices_xyz_rgba_uv, static_cast<int>(vertex_count), texture_id_solid_color);
}

void PGraphicsOpenGLv33::RM_render_rect(const float x, const float y, const float width, const float height) {
    if (color_stroke.active) {
        add_stroke_vertex_xyz_rgba(x, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        add_stroke_vertex_xyz_rgba(x + width, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);

        add_stroke_vertex_xyz_rgba(x + width, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        add_stroke_vertex_xyz_rgba(x + width, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);

        add_stroke_vertex_xyz_rgba(x + width, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        add_stroke_vertex_xyz_rgba(x, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);

        add_stroke_vertex_xyz_rgba(x, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
        add_stroke_vertex_xyz_rgba(x, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
    }
    if (color_fill.active) {
        add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        add_fill_vertex_xyz_rgba_uv(x + width, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        add_fill_vertex_xyz_rgba_uv(x + width, y + height, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);

        add_fill_vertex_xyz_rgba_uv(x + width, y + height, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        add_fill_vertex_xyz_rgba_uv(x, y + height, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
        add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);

        constexpr int       RECT_NUM_VERTICES               = 6;
        const unsigned long fill_vertices_count_xyz_rgba_uv = fill_vertices_xyz_rgba_uv.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
        if (renderBatches.empty() || renderBatches.back().texture_id != texture_id_solid_color) {
            renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - RECT_NUM_VERTICES, RECT_NUM_VERTICES, texture_id_solid_color);
        } else {
            renderBatches.back().num_vertices += RECT_NUM_VERTICES;
        }
    }
}

void PGraphicsOpenGLv33::RM_render_ellipse_filled(const float x, const float y,
                                                  const float      width,
                                                  const float      height,
                                                  const int        detail,
                                                  const glm::vec4& color) {
    const float            radiusX = width / 2.0f;
    const float            radiusY = height / 2.0f;
    std::vector<glm::vec3> points;
    for (int i = 0; i <= detail; ++i) {
        const float     theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(detail);
        const glm::vec3 p{x + radiusX * cosf(theta),
                          y + radiusY * sinf(theta), 0};
        points.push_back(p);
    }
    const glm::vec3     center{x, y, 0};
    constexpr glm::vec2 tex_coords{0, 0};
    for (int i = 0; i < points.size(); ++i) {
        const glm::vec3 p1 = points[i];
        const glm::vec3 p2 = points[(i + 1) % points.size()];
        add_fill_vertex_xyz_rgba_uv(center, color, tex_coords);
        add_fill_vertex_xyz_rgba_uv(p1, color, tex_coords);
        add_fill_vertex_xyz_rgba_uv(p2, color, tex_coords);
    }
    RM_add_texture_id_to_render_batch(fill_vertices_xyz_rgba_uv, static_cast<int>(points.size() * 3), texture_id_solid_color);
}

void PGraphicsOpenGLv33::RM_render_ellipse(const float x, const float y, const float width, const float height) {
    if (color_stroke.active) {
        const float            radiusX = width / 2.0f;
        const float            radiusY = height / 2.0f;
        std::vector<glm::vec3> points;
        for (int i = 0; i <= ellipse_detail; ++i) {
            const float     theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(ellipse_detail);
            const glm::vec3 p{x + radiusX * cosf(theta),
                              y + radiusY * sinf(theta), 0};
            points.push_back(p);
        }
        const glm::vec4 color{color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a};
        RM_render_line_strip_as_quad_segments(points, color, true, false);
    }
    if (color_fill.active) {
        const glm::vec4 color{color_fill.r, color_fill.g, color_fill.b, color_fill.a};
        RM_render_ellipse_filled(x, y, width, height, ellipse_detail, color);
    }
}

/* --- RENDER_MODE_IMMEDIATE (IM) --- */

void PGraphicsOpenGLv33::IM_render_line(const float x1, const float y1, const float z1,
                                        const float x2, const float y2, const float z2) {
    // ensure primitive is initialized
    if (IM_primitive_line.uninitialized()) {
        SHARED_init_vertex_buffer(IM_primitive_line);
    }

    IM_primitive_line.vertices[0].position  = glm::vec3(x1, y1, z1);
    IM_primitive_line.vertices[0].color     = glm::vec4(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
    IM_primitive_line.vertices[0].tex_coord = glm::vec2(0.0f, 0.0f);
    IM_primitive_line.vertices[1].position  = glm::vec3(x2, y2, z2);
    IM_primitive_line.vertices[1].color     = glm::vec4(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
    IM_primitive_line.vertices[1].tex_coord = glm::vec2(1.0f, 0.0f);
    IM_render_vertex_buffer(IM_primitive_line, GL_LINES, IM_primitive_line.vertices);
}

void PGraphicsOpenGLv33::IM_render_rect(const float x, const float y, const float width, const float height) {
    if (!color_stroke.active && !color_fill.active) {
        return;
    }

    // compute rectangle corners using glm::vec2
    glm::vec2 p1, p2;
    switch (rect_mode) {
        case CORNERS:
            p1 = {x, y};
            p2 = {width, height};
            break;
        case CENTER:
            p1 = {x - width * 0.5f, y - height * 0.5f};
            p2 = {x + width * 0.5f, y + height * 0.5f};
            break;
        case RADIUS:
            p1 = {x - width, y - height};
            p2 = {x + width, y + height};
            break;
        case CORNER:
        default:
            p1 = {x, y};
            p2 = {x + width, y + height};
            break;
    }

    // define colors once (avoiding redundant glm::vec4 conversions)
    const glm::vec4 fill_color   = as_vec4(color_fill);
    const glm::vec4 stroke_color = as_vec4(color_stroke);

    // define rectangle vertices (shared for fill and stroke)
    static constexpr uint8_t                  NUM_VERTICES  = 4;
    const std::array<glm::vec3, NUM_VERTICES> rect_vertices = {
        glm::vec3{p1.x, p1.y, 0},
        glm::vec3{p2.x, p1.y, 0},
        glm::vec3{p2.x, p2.y, 0},
        glm::vec3{p1.x, p2.y, 0}};
    constexpr std::array<glm::vec2, NUM_VERTICES> rect_tex_coords = {
        glm::vec2{0, 0},
        glm::vec2{1, 0},
        glm::vec2{1, 1},
        glm::vec2{0, 1}};

    if (color_fill.active) {
        if (IM_primitive_rect_fill.uninitialized()) {
            SHARED_init_vertex_buffer(IM_primitive_rect_fill);
        }

        for (int i = 0; i < NUM_VERTICES; ++i) {
            IM_primitive_rect_fill.vertices[i].position  = rect_vertices[i];
            IM_primitive_rect_fill.vertices[i].color     = fill_color;
            IM_primitive_rect_fill.vertices[i].tex_coord = rect_tex_coords[i];
        }

        IM_render_vertex_buffer(IM_primitive_rect_fill, GL_TRIANGLE_FAN, IM_primitive_rect_fill.vertices);
    }

    if (color_stroke.active) {
        if (IM_primitive_rect_stroke.uninitialized()) {
            SHARED_init_vertex_buffer(IM_primitive_rect_stroke);
        }

        for (int i = 0; i < NUM_VERTICES; ++i) {
            IM_primitive_rect_stroke.vertices[i].position  = rect_vertices[i];
            IM_primitive_rect_stroke.vertices[i].color     = stroke_color;
            IM_primitive_rect_stroke.vertices[i].tex_coord = rect_tex_coords[i];
        }
        IM_primitive_rect_stroke.vertices[NUM_VERTICES] = IM_primitive_rect_stroke.vertices[0];

        IM_render_vertex_buffer(IM_primitive_rect_stroke, GL_LINE_STRIP, IM_primitive_rect_stroke.vertices);
    }
}

void PGraphicsOpenGLv33::IM_render_ellipse(const float x, const float y, const float width, const float height) {
    if (!color_fill.active && !color_stroke.active) {
        return;
    }

    // TODO: Implement `ellipseMode()`
    constexpr glm::vec2 tex_coord  = {0.0f, 0.0f};
    constexpr float     PI         = 3.14159265358979323846f;
    const float         radiusX    = width * 0.5f;
    const float         radiusY    = height * 0.5f;
    const float         deltaTheta = (2.0f * PI) / static_cast<float>(ellipse_detail);

    std::vector<glm::vec3> points;
    points.reserve(ellipse_detail + 1);

    // TODO create and recompute LUT for when `ellipse_detail` changes
    float i_f = 0.0f;
    for (int i = 0; i <= ellipse_detail; ++i, i_f += 1.0f) {
        const float theta = deltaTheta * i_f;
        points.emplace_back(x + radiusX * ellipse_points_LUT[i].x,
                            y + radiusY * ellipse_points_LUT[i].y,
                            0.0f);
    }

    shape_fill_vertex_cache.clear();
    shape_fill_vertex_cache.reserve(ellipse_detail + 2);

    const glm::vec3 center{x, y, 0};

    if (color_fill.active) {
        const glm::vec4 fill_color = as_vec4(color_fill);

        shape_fill_vertex_cache.emplace_back(center, fill_color, tex_coord);
        for (const auto& p: points) {
            shape_fill_vertex_cache.emplace_back(p, fill_color, tex_coord);
        }

        IM_render_vertex_buffer(IM_primitive_shape, GL_TRIANGLE_FAN, shape_fill_vertex_cache);
    }

    if (color_stroke.active) {
        const glm::vec4 stroke_color = as_vec4(color_stroke);

        shape_fill_vertex_cache.clear();
        for (const auto& p: points) {
            shape_fill_vertex_cache.emplace_back(p, stroke_color, tex_coord);
        }

        IM_render_vertex_buffer(IM_primitive_shape, GL_LINE_STRIP, shape_fill_vertex_cache);
    }
}

void PGraphicsOpenGLv33::IM_render_vertex_buffer(PrimitiveVertexBuffer& primitive,
                                                 const GLenum           mode,
                                                 std::vector<Vertex>&   shape_vertices) const {
    // ensure there are vertices to render
    if (shape_vertices.empty()) {
        return;
    }

    static constexpr int MAX_NUM_VERTICES_CLIENT_SIDE_TRANSFORM = 4; // NOTE all shapes *up to* quads are transformed on CPU
    bool                 mModelMatrixTransformOnGPU             = false;
    if (model_matrix_dirty) {
        // NOTE depending on the number of vertices transformation are handle on the GPU
        if (shape_vertices.size() <= MAX_NUM_VERTICES_CLIENT_SIDE_TRANSFORM) {
            for (auto& p: shape_vertices) {
                p.position = glm::vec3(model_matrix_client * glm::vec4(p.position, 1.0f));
            }
        } else {
            mModelMatrixTransformOnGPU = true;
        }
    }

    GLint modelLoc{0};
    if (mModelMatrixTransformOnGPU) {
        modelLoc = glGetUniformLocation(fill_shader_program, "uModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix_client));
    }
    SHARED_render_vertex_buffer(primitive, mode, shape_vertices);
    if (mModelMatrixTransformOnGPU) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_matrix_shader));
    }
}

std::vector<PGraphicsOpenGLv33::Vertex> PGraphicsOpenGLv33::convertPolygonToTriangleFan(const std::vector<Vertex>& polygon) {
    if (polygon.size() < 3) {
        return {}; // Need at least 3 vertices
    }

    std::vector<Vertex> fan;
    fan.push_back(polygon[0]); // Center vertex

    for (size_t i = 1; i < polygon.size() - 1; i++) {
        fan.push_back(polygon[i]);
        fan.push_back(polygon[i + 1]);
    }
    return fan;
}

std::vector<PGraphicsOpenGLv33::Vertex> PGraphicsOpenGLv33::convertQuadsToTriangles(const std::vector<Vertex>& quads) {
    if (quads.size() < 4) {
        return {};
    }

    std::vector<Vertex> triangles;
    const size_t        validQuadCount = quads.size() / 4; // only use full quads

    for (size_t i = 0; i < validQuadCount * 4; i += 4) {
        // First triangle (0-1-2)
        triangles.push_back(quads[i]);
        triangles.push_back(quads[i + 1]);
        triangles.push_back(quads[i + 2]);

        // Second triangle (1-3-2)
        triangles.push_back(quads[i + 1]);
        triangles.push_back(quads[i + 3]);
        triangles.push_back(quads[i + 2]);
    }
    return triangles;
}

void PGraphicsOpenGLv33::IM_render_end_shape(const bool close_shape) {
    if (IM_primitive_shape.uninitialized()) {
        SHARED_init_vertex_buffer(IM_primitive_shape);
    }

    if (close_shape) {
        // TODO add first entry as last …
        shape_stroke_vertex_cache_vec3_DEPRECATED.push_back(shape_stroke_vertex_cache_vec3_DEPRECATED[0]);
        shape_fill_vertex_cache.push_back(shape_fill_vertex_cache[0]);
        shape_stroke_vertex_cache.push_back(shape_stroke_vertex_cache[0]);
    }
    /*
     * OpenGL ES 3.1 is stricter:
     *
     * 1.	No GL_LINES, GL_LINE_STRIP, or GL_LINE_LOOP support in core spec!
     * 2.	No glLineWidth support at all.
     * 3.	Only GL_TRIANGLES, GL_TRIANGLE_STRIP, and GL_TRIANGLE_FAN are guaranteed.
     *
     * i.e GL_LINES + GL_LINE_STRIP must be emulated
     */
    switch (shape_mode_cache) {
        case POINTS:
            glPointSize(point_size); // TODO does this still work under macOS? it renders squares … maybe texturize them
            IM_render_vertex_buffer(IM_primitive_shape, GL_POINTS, shape_stroke_vertex_cache);
            glPointSize(1);
            break;
        case LINES:
            IM_render_vertex_buffer(IM_primitive_shape, GL_LINES, shape_stroke_vertex_cache);
            break;
        case LINE_STRIP:
            IM_render_vertex_buffer(IM_primitive_shape, GL_LINE_STRIP, shape_stroke_vertex_cache);
            break;
        case TRIANGLES:
            IM_render_vertex_buffer(IM_primitive_shape, GL_TRIANGLES, shape_fill_vertex_cache);
            IM_render_vertex_buffer(IM_primitive_shape, GL_LINE_STRIP, shape_stroke_vertex_cache);
            break;
        case TRIANGLE_FAN:
            IM_render_vertex_buffer(IM_primitive_shape, GL_TRIANGLE_FAN, shape_fill_vertex_cache);
            IM_render_vertex_buffer(IM_primitive_shape, GL_LINE_STRIP, shape_stroke_vertex_cache);
            break;
        case QUAD_STRIP: // NOTE does this just work?!?
        case TRIANGLE_STRIP:
            IM_render_vertex_buffer(IM_primitive_shape, GL_TRIANGLE_STRIP, shape_fill_vertex_cache);
            IM_render_vertex_buffer(IM_primitive_shape, GL_LINE_STRIP, shape_stroke_vertex_cache);
            break;
        case QUADS: {
            std::vector<Vertex> vertices_fill = convertQuadsToTriangles(shape_fill_vertex_cache);
            IM_render_vertex_buffer(IM_primitive_shape, GL_TRIANGLES, vertices_fill);
            std::vector<Vertex> vertices_stroke = convertQuadsToTriangles(shape_stroke_vertex_cache);
            IM_render_vertex_buffer(IM_primitive_shape, GL_LINE_STRIP, vertices_stroke);
        } break;
        default:
        case POLYGON: {
            // TODO tesselate properly with [`libtess2`](https://github.com/memononen/libtess2) ( fast, 3D ) or [`earcut.hpp`](https://github.com/mapbox/earcut.hpp)
            std::vector<Vertex> vertices_fill = convertPolygonToTriangleFan(shape_fill_vertex_cache);
            IM_render_vertex_buffer(IM_primitive_shape, GL_TRIANGLE_FAN, vertices_fill);
            std::vector<Vertex> vertices_stroke = convertPolygonToTriangleFan(shape_stroke_vertex_cache);
            IM_render_vertex_buffer(IM_primitive_shape, GL_LINE_STRIP, vertices_stroke);
        } break;
    }
}
