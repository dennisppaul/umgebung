/*
 * Umgebung
 *
 * This file is part of the *Umgebung* library (https://github.com/dennisppaul/umgebung).
 * Copyright (c) 2025 Dennis P Paul.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Umgebung.h"
#include "PGraphicsOpenGL.h"
#include "PGraphicsOpenGLv33.h"
#include "Vertex.h"
#include "Geometry.h"
#include "PMesh.h"
#include "PShader.h"
#include "ShaderSourceColorTexture.h"

using namespace umgebung;

PGraphicsOpenGLv33::PGraphicsOpenGLv33(const bool render_to_offscreen) : PImage(0, 0, 0) {
    this->render_to_offscreen = render_to_offscreen;
}

void PGraphicsOpenGLv33::IMPL_background(float a, float b, float c, float d) {
    glClearColor(a, b, c, d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // TODO should this also flush bins?
}

void PGraphicsOpenGLv33::IMPL_bind_texture(const int bind_texture_id) {
    if (bind_texture_id != texture_id_current) {
        texture_id_current = bind_texture_id;
        glBindTexture(GL_TEXTURE_2D, texture_id_current); // NOTE this should be the only glBindTexture ( except for initializations )
    }
}

void PGraphicsOpenGLv33::IMPL_set_texture(PImage* img) {
    if (img == nullptr) {
        IMPL_bind_texture(texture_id_solid_color);
        return;
    }

    if (shape_has_begun) {
        console("`texture()` can only be called right before `beginShape(...)`. ( note, this is different from the original processing )");
        return;
    }

    // TODO move this to own method and share with `texture()`
    if (img->texture_id == TEXTURE_NOT_GENERATED) {
        OGL_generate_and_upload_image_as_texture(img, true);
        if (img->texture_id == TEXTURE_NOT_GENERATED) {
            error("image cannot create texture.");
            return;
        }
    }

    IMPL_bind_texture(img->texture_id);
    // TODO so this is interesting: we could leave the texture bound and require the client
    //      to unbind it with `texture_unbind()` or should `endShape()` always reset to
    //      `texture_id_solid_color` with `texture_unbind()`.
}

/**
 * implement this method for respective renderer e.g
 *
 * - OpenGLv3.3 ( shader based, buffered mode, vertex array objects ),
 * - OpenGLv2.0 ( fixed function pipeline, immediate mode, vertex buffer arrays ),
 * - SDL2
 *
 * and maybe later vulkan, metal, etc.
 *
 * @param line_strip_vertices
 * @param line_strip_closed
 */
void PGraphicsOpenGLv33::emit_shape_stroke_line_strip(std::vector<Vertex>& line_strip_vertices, const bool line_strip_closed) {
    // NOTE relevant information for this method
    //     - closed
    //     - stroke_weight
    //     - stroke_join
    //     - stroke_cap
    //     - (shader_id)
    //     - (texture_id)

    // NOTE this is a very central method! up until everything should have been done in generic PGraphics.
    //      - vertices are in model space
    //      - vertices are in line strip format ( i.e not triangulated or anything yet )
    //      - decide on rendering mode ( triangulated, native, etcetera )
    //      - this method is usually accessed from `endShape()`

    // TODO maybe add stroke recorder here ( need to transform vertices to world space )

    if (render_mode == RENDER_MODE_BUFFERED) {
        if (line_render_mode == STROKE_RENDER_MODE_TRIANGULATE) {
            std::vector<Vertex> line_vertices;
            triangulate_line_strip_vertex(line_strip_vertices, line_strip_closed, line_vertices);
            // TODO collect `line_vertices` and render as `GL_TRIANGLES` at end of frame
        }
        if (line_render_mode == STROKE_RENDER_MODE_NATIVE) {
            // TODO collect `line_strip_vertices` and render as `GL_LINE_STRIP` at end of frame
        }
    }

    if (render_mode == RENDER_MODE_IMMEDIATE) {
        if (vertex_buffer_data.uninitialized()) {
            OGL3_init_vertex_buffer(vertex_buffer_data);
        }
        if (line_render_mode == STROKE_RENDER_MODE_TRIANGULATE) {
            std::vector<Vertex> line_vertices;
            triangulate_line_strip_vertex(line_strip_vertices, line_strip_closed, line_vertices);
            OGL3_render_vertex_buffer(vertex_buffer_data, GL_TRIANGLES, line_vertices);
        }
        if (line_render_mode == STROKE_RENDER_MODE_NATIVE) {
            OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_LINE_STRIP, line_strip_vertices);
        }
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
}

void PGraphicsOpenGLv33::emit_shape_fill_triangles(std::vector<Vertex>& triangle_vertices) {
    // NOTE relevant information for this method
    //     - textured_id
    //     - normal
    //     - (shader_id)

    // NOTE this is a very central method! up until everything should have been done in generic PGraphics.
    //      - vertices are in model space

    // NOTE this is the magic place. here we can do everything we want with the triangle_vertices
    //      e.g export to PDF or SVG, or even do some post processing.
    //      ideally up until here everything could stau in PGraphics i.e all client side drawing
    //      like point, line, rect, ellipse and begin-end-shape

    // TODO maybe add triangle recorder here ( need to transform vertices to world space )

    if (render_mode == RENDER_MODE_BUFFERED) {
        // TODO collect triangles and current texture information for retained mode here.
        //      - maybe sort by transparency ( and by depth )
        //      - maybe sort transparent triangles by depth
        //      - maybe sort by fill and stroke
        //      ```C
        //      add_stroke_vertex_xyz_rgba(position, color, tex_coord); // applies transformation
        //      ... // add more triangle_vertices … maybe optimize by adding multiple triangle_vertices at once
        //      RM_add_texture_id_to_render_batch(triangle_vertices,num_vertices,batch_texture_id);
        //      ```
    }
    if (render_mode == RENDER_MODE_IMMEDIATE) {
        if (vertex_buffer_data.uninitialized()) {
            OGL3_init_vertex_buffer(vertex_buffer_data);
        }
        OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_TRIANGLES, triangle_vertices);
    }
}

// TODO could move this to a shared method in `PGraphics` and use beginShape(TRIANGLES)
void PGraphicsOpenGLv33::debug_text(const std::string& text, const float x, const float y) {
    std::vector<Vertex> triangle_vertices = debug_font.generate(text, x, y, glm::vec4(color_fill));
    const int           tmp_bound_texture = texture_id_current; // TODO use push_texture_id() and pop_texture_id()
    IMPL_bind_texture(debug_font.textureID);
    OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_TRIANGLES, triangle_vertices);
    IMPL_bind_texture(tmp_bound_texture);
}

/* --- UTILITIES --- */

void PGraphicsOpenGLv33::reset_matrices() {
    PGraphics::reset_matrices();

    glViewport(0, 0, framebuffer.width, framebuffer.height);
}

void PGraphicsOpenGLv33::prepare_frame() {
    set_default_graphics_state();

    shader(default_shader);
    default_shader->set_uniform("uProjection", projection_matrix_3D);
    default_shader->set_uniform("uViewMatrix", view_matrix);
    default_shader->set_uniform(SHADER_UNIFORM_MODEL_MATRIX, glm::mat4(1.0f));

    texture_id_current = 0;
    IMPL_bind_texture(texture_id_solid_color);
}

void PGraphicsOpenGLv33::setup_fbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
}

void PGraphicsOpenGLv33::endDraw() {
    if (render_mode == RENDER_MODE_BUFFERED) {
        // TODO flush collected vertices
        // RM_flush_fill();
        // RM_flush_stroke();
        // void PGraphicsOpenGLv33::RM_flush_stroke() {
        //     if (stroke_vertices_xyz_rgba.empty()) {
        //         return;
        //     }
        //
        //     // glEnable(GL_BLEND);
        //     // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //
        //     glBindBuffer(GL_ARRAY_BUFFER, stroke_VBO_xyz_rgba);
        //     const unsigned long size = stroke_vertices_xyz_rgba.size() * sizeof(float);
        //     glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), stroke_vertices_xyz_rgba.data());
        //
        //     glUseProgram(stroke_shader_program);
        //
        //     // Upload matrices
        //     const GLint projLoc = glGetUniformLocation(stroke_shader_program, "uProjection");
        //     glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix_3D)); // or projection2D
        //
        //     const GLint viewLoc = glGetUniformLocation(stroke_shader_program, "uViewMatrix");
        //     glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
        //
        //     const GLint matrixLoc = glGetUniformLocation(stroke_shader_program, SHADER_UNIFORM_MODEL_MATRIX);
        //     glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        //
        //     glBindVertexArray(stroke_VAO_xyz_rgba);
        //     glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(stroke_vertices_xyz_rgba.size()) / NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA);
        //     glBindVertexArray(0);
        //
        //     stroke_vertices_xyz_rgba.clear();
        // }
        //
        // void PGraphicsOpenGLv33::RM_flush_fill() {
        //     if (fill_vertices_xyz_rgba_uv.empty()) {
        //         return;
        //     }
        //
        //     // glEnable(GL_BLEND);
        //     // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //
        //     glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);
        //     const unsigned long size = fill_vertices_xyz_rgba_uv.size() * sizeof(float);
        //     glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), fill_vertices_xyz_rgba_uv.data());
        //
        //     glUseProgram(fill_shader_program);
        //
        //     // Upload matrices
        //     const GLint projLoc = glGetUniformLocation(fill_shader_program, "uProjection");
        //     glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix_3D));
        //
        //     const GLint viewLoc = glGetUniformLocation(fill_shader_program, "uViewMatrix");
        //     glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
        //
        //     const GLint modelLoc = glGetUniformLocation(fill_shader_program, SHADER_UNIFORM_MODEL_MATRIX);
        //     glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        //
        //     // Bind textures per batch
        //     glBindVertexArray(fill_VAO_xyz_rgba_uv);
        //     for (const auto& batch: renderBatches) {
        //         glBindTexture(GL_TEXTURE_2D, batch.texture_id); // NOTE no need to use `IMPL_bind_texture()`
        //         glDrawArrays(GL_TRIANGLES, batch.start_index, batch.num_vertices);
        //     }
        //     glBindTexture(GL_TEXTURE_2D, 0); // NOTE no need to use `IMPL_bind_texture()`
        //     glBindVertexArray(0);
        //
        //     fill_vertices_xyz_rgba_uv.clear();
        //     renderBatches.clear();
        // }
    }

    PGraphicsOpenGL::endDraw();
}

void PGraphicsOpenGLv33::render_framebuffer_to_screen(const bool use_blit) {
    // modern OpenGL framebuffer rendering method
    if (use_blit) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, framebuffer.width, framebuffer.height,
                          0, 0, framebuffer.width, framebuffer.height,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR); // TODO maybe GL_NEAREST is enough
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    } else {
        warning("`render_framebuffer_to_screen` need to implement this ... may re-use existing shader");
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glDisable(GL_DEPTH_TEST);
        // glDisable(GL_BLEND);
        //
        // glUseProgram(shaderProgram);
        // glBindVertexArray(screenVAO);
        //
        // bind_framebuffer_texture();
        // glUniform1i(glGetUniformLocation(shaderProgram, "screenTexture"), 0);
        //
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        //
        // glBindVertexArray(0);
        // glUseProgram(0);
    }
}

void PGraphicsOpenGLv33::hint(const uint16_t property) {
    // TODO @MERGE
    switch (property) {
        case HINT_ENABLE_SMOOTH_LINES:
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            break;
        case HINT_DISABLE_SMOOTH_LINES:
            glDisable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
            break;
        default:
            break;
    }
}

void PGraphicsOpenGLv33::upload_texture(PImage*         img,
                                        const uint32_t* pixel_data,
                                        const int       width,
                                        const int       height,
                                        const int       offset_x,
                                        const int       offset_y,
                                        const bool      mipmapped) {
    if (img == nullptr) {
        return;
    }

    if (pixel_data == nullptr) {
        return;
    }

    if (img->texture_id < TEXTURE_VALID_ID) {
        OGL_generate_and_upload_image_as_texture(img, mipmapped);
        console("PGraphics / `upload_texture` texture has not been initialized yet … trying to initialize");
        if (img->texture_id < TEXTURE_VALID_ID) {
            error("PGraphics / `upload_texture` failed to create texture");
            return;
        }
    }

    // Check if the provided width, height, and offsets are within the valid range
    if (width <= 0 || height <= 0) {
        error("PGraphics / `upload_texture` invalid width or height");
        return;
    }

    if (offset_x < 0 || offset_y < 0 || offset_x + width > img->width || offset_y + height > img->height) {
        error("PGraphics / `upload_texture` parameters exceed image dimensions");
        return;
    }

    const int tmp_bound_texture = texture_id_current;
    IMPL_bind_texture(img->texture_id);

    constexpr GLint mFormat = UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT; // internal format is always RGBA
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, offset_x, offset_y,
                    width, height,
                    mFormat,
                    UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE,
                    pixel_data);

    IMPL_bind_texture(tmp_bound_texture);
}

void PGraphicsOpenGLv33::download_texture(PImage* img) {
    if (img == nullptr) {
        return;
    }
    if (img->pixels == nullptr) {
        return;
    }
    if (img->texture_id < TEXTURE_VALID_ID) {
        return;
    }

    const int tmp_bound_texture = texture_id_current;
    IMPL_bind_texture(img->texture_id);
    glGetTexImage(GL_TEXTURE_2D, 0,
                  UMGEBUNG_DEFAULT_INTERNAL_PIXEL_FORMAT,
                  UMGEBUNG_DEFAULT_TEXTURE_PIXEL_TYPE,
                  img->pixels);
    IMPL_bind_texture(tmp_bound_texture);
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

    default_shader = loadShader(shader_source_color_texture.vertex, shader_source_color_texture.fragment);

    if (default_shader == nullptr) {
        error("Failed to load default shader.");
    }

    // stroke_shader_program = OGL_build_shader(shader_source_color.vertex, shader_source_color.fragment);
    // fill_shader_program   = OGL_build_shader(shader_source_color_texture.vertex, shader_source_color_texture.fragment);

    if (render_to_offscreen) {
        console("setting up rendering to offscreen buffer:");
        console("framebuffer: ", framebuffer.width, "x", framebuffer.height);
        console("graphics   : ", this->width, "x", this->height);
        glGenFramebuffers(1, &framebuffer.id);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
        glGenTextures(1, &framebuffer.texture_id);
        glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id); // NOTE no need to use `IMPL_bind_texture()`
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture_id, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            // Handle framebuffer incomplete error
            error("ERROR Framebuffer is not complete!");
        }
        glViewport(0, 0, framebuffer.width, framebuffer.height);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0); // NOTE no need to use `IMPL_bind_texture()`
    }

    OGL3_create_solid_color_texture();
    IMPL_bind_texture(texture_id_solid_color);

    reset_matrices();
}

/* additional */

bool PGraphicsOpenGLv33::OGL_generate_and_upload_image_as_texture(PImage* image, const bool generate_texture_mipmapped) {
    // TODO merge with method in PGraphicsOpenGLv20
    if (image == nullptr) {
        error("Failed to upload texture because image nullptr.");
        return false;
    }

    if (image->pixels == nullptr) {
        error("Failed to upload texture because pixels are null. make sure pixel array exists.");
        return false;
    }

    GLuint mTextureID;
    glGenTextures(1, &mTextureID);

    if (mTextureID == 0) {
        error("Failed to generate texture ID");
        return false;
    }

    image->texture_id           = static_cast<int>(mTextureID);
    const int tmp_bound_texture = texture_id_current;
    IMPL_bind_texture(image->texture_id);

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

    IMPL_bind_texture(tmp_bound_texture);
    return true;
}

void PGraphicsOpenGLv33::OGL3_create_solid_color_texture() {
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id); // NOTE no need to use `IMPL_bind_texture()`

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

    glBindTexture(GL_TEXTURE_2D, 0); // NOTE no need to use `IMPL_bind_texture()`

    texture_id_solid_color = texture_id;
}

void PGraphicsOpenGLv33::OGL3_init_vertex_buffer(VertexBufferData& vertex_buffer) {
    // Generate and bind VAO & VBO
    glGenVertexArrays(1, &vertex_buffer.VAO);
    glBindVertexArray(vertex_buffer.VAO);

    glGenBuffers(1, &vertex_buffer.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.VBO);

    // Allocate GPU memory (without initializing data, as it will be updated before use)
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertex_buffer.num_vertices * sizeof(Vertex)), nullptr, GL_DYNAMIC_DRAW);

    // set up attribute pointers. NOTE make sure to align to locations in shader.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tex_coord)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void PGraphicsOpenGLv33::OGL3_resize_vertex_buffer(const size_t buffer_size_bytes) {
    GLint bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    if (buffer_size_bytes > static_cast<size_t>(bufferSize)) {
        // allocate extra space to reduce reallocations
        const size_t growSize = std::max(static_cast<int>(buffer_size_bytes), bufferSize + static_cast<int>(VBO_BUFFER_CHUNK_SIZE));
        console("increasing vertex buffer array to ", growSize, " ( no worries, this should be all good )");
        glBufferData(GL_ARRAY_BUFFER, static_cast<int>(growSize), nullptr, GL_DYNAMIC_DRAW);
    }
}

void PGraphicsOpenGLv33::OGL3_render_vertex_buffer(VertexBufferData&          vertex_buffer,
                                                   const GLenum               primitive_mode,
                                                   const std::vector<Vertex>& shape_vertices) {
    // TODO maybe replace this with PMesh
    // Ensure there are vertices to render
    if (shape_vertices.empty()) {
        return;
    }

    // Ensure primitive is initialized
    if (vertex_buffer.uninitialized()) {
        OGL3_init_vertex_buffer(vertex_buffer);
    }

    // Update VBO with collected vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.VBO);

    constexpr bool hint_check_buffer_size = true;
    if (hint_check_buffer_size) {
        const size_t buffer_size = shape_vertices.size() * sizeof(Vertex);
        OGL3_resize_vertex_buffer(buffer_size);
    }

    // Update only necessary data
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<int>(shape_vertices.size() * sizeof(Vertex)), shape_vertices.data());

    // Bind VAO and draw the shape
    glBindVertexArray(vertex_buffer.VAO);
    glDrawArrays(primitive_mode, 0, static_cast<GLsizei>(shape_vertices.size()));

    // Unbind VAO for safety (optional)
    glBindVertexArray(0);
}

void PGraphicsOpenGLv33::OGL_tranform_model_matrix_and_render_vertex_buffer(VertexBufferData&    vertex_buffer,
                                                                            const GLenum         mode,
                                                                            std::vector<Vertex>& shape_vertices) const {
    static bool _emit_warning_only_once = false;
    if (mode != GL_TRIANGLES && mode != GL_LINE_STRIP) {
        if (!_emit_warning_only_once) {
            warning("this test is just for development purposes: only GL_TRIANGLES and GL_LINE_STRIP are supposed to be used atm.");
            warning("( warning only once )");
            _emit_warning_only_once = true;
        }
    }

    if (shape_vertices.empty()) {
        return;
    }

    static constexpr int MAX_NUM_VERTICES_CLIENT_SIDE_TRANSFORM = 4; // NOTE all shapes *up to* quads are transformed on CPU
    bool                 mModelMatrixTransformOnGPU             = false;
    if (model_matrix_dirty) {
        // NOTE depending on the number of vertices transformation are handle on the GPU
        if (shape_vertices.size() <= MAX_NUM_VERTICES_CLIENT_SIDE_TRANSFORM) {
            for (auto& p: shape_vertices) {
                p.position = glm::vec3(model_matrix * glm::vec4(p.position, 1.0f));
            }
        } else {
            mModelMatrixTransformOnGPU = true;
        }
    }
    if (mModelMatrixTransformOnGPU) {
        if (current_shader == default_shader) {
            default_shader->set_uniform(SHADER_UNIFORM_MODEL_MATRIX, model_matrix);
        }
    }
    OGL3_render_vertex_buffer(vertex_buffer, mode, shape_vertices);
    if (mModelMatrixTransformOnGPU) {
        if (current_shader == default_shader) {
            default_shader->set_uniform(SHADER_UNIFORM_MODEL_MATRIX, glm::mat4(1.0f));
        }
    }
}

void PGraphicsOpenGLv33::mesh(PMesh* mesh_shape) {
    if (mesh_shape == nullptr) {
        return;
    }
    if (current_shader == default_shader) { default_shader->set_uniform(SHADER_UNIFORM_MODEL_MATRIX, model_matrix); }
    mesh_shape->draw();
    if (current_shader == default_shader) { default_shader->set_uniform(SHADER_UNIFORM_MODEL_MATRIX, glm::mat4(1.0f)); }
}

PShader* PGraphicsOpenGLv33::loadShader(const std::string& vertex_code, const std::string& fragment_code, const std::string& geometry_code) {
    const auto _shader = new PShader();
    _shader->load(vertex_code, fragment_code, geometry_code);
    return _shader;
}

void PGraphicsOpenGLv33::shader(PShader* shader) {
    if (shader == nullptr) {
        default_shader->use();
        current_shader = default_shader;
        return;
    }
    shader->use();
    current_shader = shader;
}
