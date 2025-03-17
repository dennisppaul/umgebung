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
    const int           tmp_bound_texture = texture_id_current;
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

    if (render_mode == RENDER_MODE_IMMEDIATE) {
        glUseProgram(fill_shader_program);

        // Upload matrices
        const GLint projLoc = glGetUniformLocation(fill_shader_program, "uProjection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix_3D));

        const GLint viewLoc = glGetUniformLocation(fill_shader_program, "uViewMatrix");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));

        const GLint modelLoc = glGetUniformLocation(fill_shader_program, "uModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        texture_id_current = 0;
        IMPL_bind_texture(texture_id_solid_color);
    }
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
        //     const GLint matrixLoc = glGetUniformLocation(stroke_shader_program, "uModelMatrix");
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
        //     const GLint modelLoc = glGetUniformLocation(fill_shader_program, "uModelMatrix");
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

    stroke_shader_program = OGL_build_shader(vertex_shader_source_simple(), fragment_shader_source_simple());
    // init_stroke_vertice_buffers();
    fill_shader_program = OGL_build_shader(vertex_shader_source_texture(), fragment_shader_source_texture());
    // init_fill_vertice_buffers();

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

// void PGraphicsOpenGLv33::fill_resize_buffer(const uint32_t newSize) {
//     // Create a new buffer
//     GLuint newVBO;
//     glGenBuffers(1, &newVBO);
//     glBindBuffer(GL_ARRAY_BUFFER, newVBO);
//     glBufferData(GL_ARRAY_BUFFER, newSize, nullptr, GL_DYNAMIC_DRAW); // Allocate new size
//
//     // Copy old data to new buffer
//     glBindBuffer(GL_COPY_READ_BUFFER, fill_VBO_xyz_rgba_uv);
//     glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, fill_max_buffer_size);
//
//     // Delete old buffer and update references
//     glDeleteBuffers(1, &fill_VBO_xyz_rgba_uv);
//     fill_VBO_xyz_rgba_uv = newVBO;
//     fill_max_buffer_size = newSize;
//
//     glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);
// }

// void PGraphicsOpenGLv33::add_transformed_fill_vertex_xyz_rgba_uv(const glm::vec3& position,
//                                                                  const glm::vec4& color,
//                                                                  const float u, const float v) {
//     fill_vertices_xyz_rgba_uv.push_back(position.x); // Position
//     fill_vertices_xyz_rgba_uv.push_back(position.y); // Position
//     fill_vertices_xyz_rgba_uv.push_back(position.z); // Position
//     fill_vertices_xyz_rgba_uv.push_back(color.r);    // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(color.g);    // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(color.b);    // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(color.a);    // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(u);          // Texture
//     fill_vertices_xyz_rgba_uv.push_back(v);          // Texture
// }

// void PGraphicsOpenGLv33::add_fill_vertex_xyz_rgba_uv(const glm::vec3 position,
//                                                      const glm::vec4 color,
//                                                      const glm::vec2 tex_coords) {
//     add_fill_vertex_xyz_rgba_uv(position.x, position.y, position.z,
//                                 color.r, color.g, color.b, color.a,
//                                 tex_coords.x, tex_coords.y);
// }

// void PGraphicsOpenGLv33::add_fill_vertex_xyz_rgba_uv_raw(const glm::vec3 position,
//                                                          const glm::vec4 color,
//                                                          const glm::vec2 tex_coords) {
//     fill_vertices_xyz_rgba_uv.push_back(position.x);   // Position
//     fill_vertices_xyz_rgba_uv.push_back(position.y);   // Position
//     fill_vertices_xyz_rgba_uv.push_back(position.z);   // Position
//     fill_vertices_xyz_rgba_uv.push_back(color.r);      // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(color.g);      // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(color.b);      // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(color.a);      // ColorState
//     fill_vertices_xyz_rgba_uv.push_back(tex_coords.x); // Texture
//     fill_vertices_xyz_rgba_uv.push_back(tex_coords.y); // Texture
// }

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

// void PGraphicsOpenGLv33::add_fill_vertex_xyz_rgba_uv(const float x, const float y, const float z,
//                                                      const float r, const float g, const float b, const float a,
//                                                      const float u, const float v) {
//     // Each vertex consists of 9 floats (x, y, z, r, g, b, u, v)
//     // uint32_t vertexSize = NUM_VERTEX_ATTRIBUTES * sizeof(float);
//     console("add_fill_vertex_xyz_rgba_uv?");
//     if ((fill_vertices_xyz_rgba_uv.size() + NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV) * sizeof(float) > fill_max_buffer_size) {
//         fill_resize_buffer(fill_max_buffer_size + VBO_BUFFER_CHUNK_SIZE);
//     }
//
//     const glm::vec4 transformed = model_matrix_client * glm::vec4(x, y, z, 1.0f); // Apply transformation
//     fill_vertices_xyz_rgba_uv.push_back(transformed.x);                           // Position
//     fill_vertices_xyz_rgba_uv.push_back(transformed.y);                           // Position
//     fill_vertices_xyz_rgba_uv.push_back(transformed.z);                           // Position
//     fill_vertices_xyz_rgba_uv.push_back(r);                                       // Color
//     fill_vertices_xyz_rgba_uv.push_back(g);                                       // Color
//     fill_vertices_xyz_rgba_uv.push_back(b);                                       // Color
//     fill_vertices_xyz_rgba_uv.push_back(a);                                       // Color
//     fill_vertices_xyz_rgba_uv.push_back(u);                                       // Texture
//     fill_vertices_xyz_rgba_uv.push_back(v);                                       // Texture
// }

// void PGraphicsOpenGLv33::add_stroke_vertex_xyz_rgba(const float x, const float y, const float z,
//                                                     const float r, const float g, const float b, const float a) {
//     // Each vertex consists of 7 floats (x, y, z, r, g, b, u, v)
//     // uint32_t vertexSize = NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA * sizeof(float);
//     if ((stroke_vertices_xyz_rgba.size() + NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA) * sizeof(float) > stroke_max_buffer_size) {
//         error("Stroke buffer is full!");
//         // TODO create version for stroke buffer
//         // stroke_resize_buffer(stroke_max_buffer_size + VBO_BUFFER_CHUNK_SIZE);
//     }
//
//     const glm::vec4 transformed = model_matrix_client * glm::vec4(x, y, z, 1.0f); // Apply transformation
//     stroke_vertices_xyz_rgba.push_back(transformed.x);                            // Position
//     stroke_vertices_xyz_rgba.push_back(transformed.y);                            // Position
//     stroke_vertices_xyz_rgba.push_back(transformed.z);                            // Position
//     stroke_vertices_xyz_rgba.push_back(r);                                        // Color
//     stroke_vertices_xyz_rgba.push_back(g);                                        // Color
//     stroke_vertices_xyz_rgba.push_back(b);                                        // Color
//     stroke_vertices_xyz_rgba.push_back(a);                                        // Color
// }

// void PGraphicsOpenGLv33::init_stroke_vertice_buffers() {
//     glGenVertexArrays(1, &stroke_VAO_xyz_rgba);
//     glGenBuffers(1, &stroke_VBO_xyz_rgba);
//
//     glBindVertexArray(stroke_VAO_xyz_rgba);
//
//     glBindBuffer(GL_ARRAY_BUFFER, stroke_VBO_xyz_rgba);
//     glBufferData(GL_ARRAY_BUFFER, stroke_max_buffer_size, nullptr, GL_DYNAMIC_DRAW);
//
//     constexpr auto STRIDE = static_cast<GLsizei>(NUM_STROKE_VERTEX_ATTRIBUTES_XYZ_RGBA * sizeof(float));
//
//     // Position Attribute (Location 0) -> 3 floats (x, y, z)
//     constexpr int NUM_POSITION_ATTRIBUTES = 3;
//     // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
//     glVertexAttribPointer(0, NUM_POSITION_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, static_cast<void*>(0)); // NOLINT(*-use-nullptr)
//     glEnableVertexAttribArray(0);
//
//     // Color Attribute (Location 1) -> 4 floats (r, g, b, a)
//     constexpr int NUM_COLOR_ATTRIBUTES = 4;
//     glVertexAttribPointer(1, NUM_COLOR_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<void*>(NUM_POSITION_ATTRIBUTES * sizeof(float)));
//     glEnableVertexAttribArray(1);
//
//     glBindVertexArray(0);
// }

// void PGraphicsOpenGLv33::init_fill_vertice_buffers() {
//     glGenVertexArrays(1, &fill_VAO_xyz_rgba_uv);
//     glGenBuffers(1, &fill_VBO_xyz_rgba_uv);
//
//     glBindVertexArray(fill_VAO_xyz_rgba_uv);
//     glBindBuffer(GL_ARRAY_BUFFER, fill_VBO_xyz_rgba_uv);
//
//     // Allocate buffer memory (maxBufferSize = 1024 KB) but don't fill it yet
//     glBufferData(GL_ARRAY_BUFFER, fill_max_buffer_size, nullptr, GL_DYNAMIC_DRAW);
//
//     constexpr auto STRIDE = static_cast<GLsizei>(NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV * sizeof(float));
//
//     // Position Attribute (Location 0) -> 3 floats (x, y, z)
//     constexpr int NUM_POSITION_ATTRIBUTES = 3;
//     // ReSharper disable once CppZeroConstantCanBeReplacedWithNullptr
//     glVertexAttribPointer(0, NUM_POSITION_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, static_cast<void*>(0)); // NOLINT(*-use-nullptr)
//     glEnableVertexAttribArray(0);
//
//     // Color Attribute (Location 1) -> 4 floats (r, g, b, a)
//     constexpr int NUM_COLOR_ATTRIBUTES = 4;
//     glVertexAttribPointer(1, NUM_COLOR_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<void*>(NUM_POSITION_ATTRIBUTES * sizeof(float)));
//     glEnableVertexAttribArray(1);
//
//     // Texture Coordinate Attribute (Location 2) -> 2 floats (u, v)
//     constexpr int NUM_TEXTURE_ATTRIBUTES = 2;
//     glVertexAttribPointer(2, NUM_TEXTURE_ATTRIBUTES, GL_FLOAT, GL_FALSE, STRIDE, reinterpret_cast<void*>((NUM_POSITION_ATTRIBUTES + NUM_COLOR_ATTRIBUTES) * sizeof(float)));
//     glEnableVertexAttribArray(2);
//
//     glBindVertexArray(0);
// }

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

// TODO add option to add geometry shader
GLuint PGraphicsOpenGLv33::OGL_build_shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // Build shaders
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    OGL_checkShaderCompileStatus(vertexShader);

    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    OGL_checkShaderCompileStatus(fragmentShader);

    const GLuint mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);
    OGL_checkProgramLinkStatus(mShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return mShaderProgram;
}

void PGraphicsOpenGLv33::OGL_checkShaderCompileStatus(const GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        error("ERROR::SHADER::COMPILATION_FAILED\n", infoLog);
    }
}

void PGraphicsOpenGLv33::OGL_checkProgramLinkStatus(const GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        error("ERROR::SHADER::COMPILATION_FAILED\n", infoLog);
    }
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

void PGraphicsOpenGLv33::OGL3_init_vertex_buffer(VertexBufferData& primitive) {
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
    // glBindTexture(GL_TEXTURE_2D, texture_id_current); // TODO why? what "why?"? do we need to bind a texture?!?
    glDrawArrays(primitive_mode, 0, static_cast<GLsizei>(shape_vertices.size()));

    // Unbind VAO for safety (optional)
    glBindVertexArray(0);
}

void PGraphicsOpenGLv33::OGL_tranform_model_matrix_and_render_vertex_buffer(VertexBufferData&    primitive,
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
    OGL3_render_vertex_buffer(primitive, mode, shape_vertices);
    if (mModelMatrixTransformOnGPU) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    }
}

// void PGraphicsOpenGLv33::IM_render_end_shape(const bool close_shape) {
//     // TODO move this to e.g `OGL_tranform_model_matrix_and_render_vertex_buffer`
//     if (vertex_buffer_data.uninitialized()) {
//         OGL3_init_vertex_buffer(vertex_buffer_data);
//     }
//

//
//     const int tmp_shape_mode_cache = shape_mode_cache;
//
//     /* --- render fill --- */
//
//     if (!shape_fill_vertex_buffer.empty()) {
//         switch (tmp_shape_mode_cache) {
//             case POINTS:
//             case LINES:
//             case LINE_STRIP:
//                 break;
//             case TRIANGLES:
//                 emit_shape_fill_triangles(shape_fill_vertex_buffer);
//                 break;
//             case TRIANGLE_FAN: {
//                 std::vector<Vertex> vertices_fill_quads = convertTriangleFanToTriangles(shape_fill_vertex_buffer);
//                 emit_shape_fill_triangles(vertices_fill_quads);
//             } break;
//             case QUAD_STRIP: // NOTE does this just work?!?
//             case TRIANGLE_STRIP: {
//                 std::vector<Vertex> vertices_fill_quads = convertTriangleStripToTriangles(shape_fill_vertex_buffer);
//                 emit_shape_fill_triangles(vertices_fill_quads);
//             } break;
//             case QUADS: {
//                 std::vector<Vertex> vertices_fill_quads = convertQuadsToTriangles(shape_fill_vertex_buffer);
//                 emit_shape_fill_triangles(vertices_fill_quads);
//             } break;
//             default:
//             case POLYGON: {
//                 std::vector<Vertex> vertices_fill_polygon;
//                 if (polygon_triangulation_strategy == POLYGON_TRIANGULATION_FASTER) {
//                     // EARCUT :: supports concave polygons, textures but no holes or selfintersection
//                     vertices_fill_polygon = triangulate_faster(shape_fill_vertex_buffer);
//                 } else if (polygon_triangulation_strategy == POLYGON_TRIANGULATION_BETTER) {
//                     // LIBTESS2 :: supports concave polygons, textures, holes and selfintersection but no textures
//                     vertices_fill_polygon = triangulate_good(shape_fill_vertex_buffer);
//                 } else if (polygon_triangulation_strategy == POLYGON_TRIANGULATION_MID) {
//                     // POLYPARTITION + CLIPPER2 // TODO maybe remove this option
//                     vertices_fill_polygon = triangulate_better_quality(shape_fill_vertex_buffer);
//                 }
//                 emit_shape_fill_triangles(vertices_fill_polygon);
//                 // TODO what if polygon has only 3 ( triangle ) or 4 vertices ( quad )? could shortcut … here
//             } break;
//         }
//     }
//
//     /* --- render stroke --- */
//
//     if (!shape_stroke_vertex_buffer.empty()) {
//         if (tmp_shape_mode_cache == POINTS) {
//             // TODO decide if point is a fill shape or it s own shape
//             if (point_render_mode == POINT_RENDER_MODE_NATIVE) {
//                 // TODO does this still work under macOS? it renders squares … maybe texturize them
//                 // TODO @OpenGLES3.1 replace with circle or textured quad
//                 const float tmp_point_size = std::max(std::min(point_size, open_gl_capabilities.point_size_max), open_gl_capabilities.point_size_min);
//                 glPointSize(tmp_point_size);
//                 OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_POINTS, shape_stroke_vertex_buffer);
//             }
//             if (point_render_mode == POINT_RENDER_MODE_TRIANGULATE) {
//                 std::vector<Vertex> line_vertices = convertPointsToTriangles(shape_stroke_vertex_buffer, point_size);
//                 emit_shape_fill_triangles(line_vertices);
//             }
//             return; // NOTE rendered as points exit early
//         }
//
//         // TODO this decision has been moved to `emit_shape_stroke_line_strip()`
//         if (line_render_mode == STROKE_RENDER_MODE_TRIANGULATE) {
//             switch (tmp_shape_mode_cache) {
//                 case LINES: {
//                     const int buffer_size = shape_stroke_vertex_buffer.size() / 2 * 2;
//                     for (int i = 0; i < buffer_size; i += 2) {
//                         std::vector line = {shape_stroke_vertex_buffer[i], shape_stroke_vertex_buffer[i + 1]};
//                         emit_shape_stroke_line_strip(line, false);
//                     }
//                 } break;
//                 case TRIANGLE_FAN: {
//                     const std::vector<Vertex> _triangle_vertices = convertTriangleFanToTriangles(shape_stroke_vertex_buffer);
//                     const int                 buffer_size        = _triangle_vertices.size() / 3 * 3;
//                     for (int i = 0; i < buffer_size; i += 3) {
//                         std::vector line = {_triangle_vertices[i], _triangle_vertices[i + 1], _triangle_vertices[i + 2]};
//                         emit_shape_stroke_line_strip(line, true);
//                     }
//                 } break;
//                 case TRIANGLES: {
//                     const int buffer_size = shape_stroke_vertex_buffer.size() / 3 * 3;
//                     for (int i = 0; i < buffer_size; i += 3) {
//                         std::vector line = {shape_stroke_vertex_buffer[i], shape_stroke_vertex_buffer[i + 1], shape_stroke_vertex_buffer[i + 2]};
//                         emit_shape_stroke_line_strip(line, true);
//                     }
//                 } break;
//                 case TRIANGLE_STRIP: {
//                     const std::vector<Vertex> _triangle_vertices = convertTriangleStripToTriangles(shape_stroke_vertex_buffer);
//                     const int                 buffer_size        = _triangle_vertices.size() / 3 * 3;
//                     for (int i = 0; i < buffer_size; i += 3) {
//                         std::vector line = {_triangle_vertices[i], _triangle_vertices[i + 1], _triangle_vertices[i + 2]};
//                         emit_shape_stroke_line_strip(line, true);
//                     }
//                 } break;
//                 case QUAD_STRIP: {
//                     const std::vector<Vertex> _quad_vertices = convertQuadStripToQuads(shape_stroke_vertex_buffer);
//                     const int                 buffer_size    = _quad_vertices.size() / 4 * 4;
//                     for (int i = 0; i < buffer_size; i += 4) {
//                         std::vector line = {_quad_vertices[i], _quad_vertices[i + 1], _quad_vertices[i + 2], _quad_vertices[i + 3]};
//                         emit_shape_stroke_line_strip(line, true);
//                     }
//                 } break;
//                 case LINE_STRIP: {
//                     std::vector<Vertex> line_vertices;
//                     triangulate_line_strip_vertex(shape_stroke_vertex_buffer, false, line_vertices);
//                     OGL3_render_vertex_buffer(vertex_buffer_data, GL_TRIANGLES, line_vertices);
//                 } break;
//                 case QUADS: {
//                     const int buffer_size = shape_stroke_vertex_buffer.size() / 4 * 4;
//                     for (int i = 0; i < buffer_size; i += 4) {
//                         std::vector line = {shape_stroke_vertex_buffer[i], shape_stroke_vertex_buffer[i + 1], shape_stroke_vertex_buffer[i + 2], shape_stroke_vertex_buffer[i + 3]};
//                         emit_shape_stroke_line_strip(line, true);
//                     }
//                 }
//                 default:
//                 case POLYGON: {
//                     emit_shape_stroke_line_strip(shape_stroke_vertex_buffer, close_shape);
//                 } break;
//             }
//             return; // NOTE rendered as triangles exit early
//         }
//
//         // NOTE handles GL_LINES and GL_LINE_STRIP not compatible with OpenGL ES 3.1 and OpenGL 3.3 core
//         if (line_render_mode == STROKE_RENDER_MODE_NATIVE) {
//             if (close_shape && (tmp_shape_mode_cache == POLYGON || tmp_shape_mode_cache == LINE_STRIP)) {
//                 // NOTE add first vertex as last …
//                 shape_stroke_vertex_cache_vec3_DEPRECATED.push_back(shape_stroke_vertex_cache_vec3_DEPRECATED[0]);
//                 shape_stroke_vertex_buffer.push_back(shape_stroke_vertex_buffer[0]);
//             }
//             const float tmp_line_width = std::max(std::min(stroke_weight, open_gl_capabilities.line_size_max), open_gl_capabilities.line_size_min);
//             glLineWidth(tmp_line_width);
//             switch (tmp_shape_mode_cache) {
//                 case LINES:
//                     OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_LINES, shape_stroke_vertex_buffer);
//                     break;
//                 case QUADS: {
//                     std::vector<Vertex> vertices_stroke_quads = convertQuadsToTriangles(shape_stroke_vertex_buffer);
//                     OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_LINE_STRIP, vertices_stroke_quads);
//                 } break;
//                 default:
//                 case LINE_STRIP:
//                 case TRIANGLES:
//                 case TRIANGLE_FAN:
//                 case QUAD_STRIP:
//                 case TRIANGLE_STRIP:
//                 case POLYGON:
//                     OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_LINE_STRIP, shape_stroke_vertex_buffer);
//                     break;
//             }
//             return; // NOTE rendered as native lines ( if supported ) exit early
//         }
//     }
// }


/*
 *
 * how to put vertives into a VBO with texture batches

// add_transformed_fill_vertex_xyz_rgba_uv(p1 + perp, color);
// add_transformed_fill_vertex_xyz_rgba_uv(p2 + perp, color);
// add_transformed_fill_vertex_xyz_rgba_uv(p1 - perp, color);
// add_transformed_fill_vertex_xyz_rgba_uv(p1 - perp, color);
// add_transformed_fill_vertex_xyz_rgba_uv(p2 + perp, color);
// add_transformed_fill_vertex_xyz_rgba_uv(p2 - perp, color);
// RM_add_texture_id_to_render_batch(fill_vertices_xyz_rgba_uv, 6, texture_id_solid_color);

// void PGraphicsOpenGLv33::RM_add_texture_id_to_render_batch(const std::vector<float>& vertices,
//                                                            const int                 num_vertices,
//                                                            const GLuint              batch_texture_id) {
//     const unsigned long fill_vertices_count_xyz_rgba_uv = vertices.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
//     if (renderBatches.empty() || renderBatches.back().texture_id != batch_texture_id) {
//         renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - num_vertices, num_vertices, batch_texture_id);
//     } else {
//         renderBatches.back().num_vertices += num_vertices;
//     }
// }
*/

// // NOTE: done
// void PGraphicsOpenGLv33::endShape(const bool close_shape) {
//     if (render_mode == RENDER_MODE_IMMEDIATE) {
//         IM_render_end_shape(close_shape);
//     }
//
//     if (render_mode == RENDER_MODE_BUFFERED) {
//         // const glm::vec4 color = as_vec4(color_stroke);
//         // switch (render_line_mode) {
//         //     case RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_ROUND:
//         //         RM_render_line_strip_as_quad_segments(shape_stroke_vertex_cache_vec3_DEPRECATED, color, close_shape, true);
//         //         break;
//         //     case RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_MITER:
//         //         RM_render_line_strip_as_connected_quads(shape_stroke_vertex_cache_vec3_DEPRECATED, color, close_shape);
//         //         break;
//         //     case RENDER_LINE_STRIP_AS_QUADS_STROKE_JOIN_NONE:
//         //     default:
//         //         RM_render_line_strip_as_quad_segments(shape_stroke_vertex_cache_vec3_DEPRECATED, color, close_shape, false);
//         //         break;
//         // }
//     }
// }

// void PGraphicsOpenGLv33::image(PImage* img, const float x, const float y, float w, float h) {
//     if (!color_fill.active) {
//         return;
//     }
//
//     if (img == nullptr) {
//         error("img is null");
//         return;
//     }
//
//     if (w < 0) {
//         w = img->width;
//     }
//     if (h < 0) {
//         h = img->height;
//     }
//
//     // TODO move this to own method and share with `texture()`
//     if (img->texture_id == TEXTURE_NOT_GENERATED) {
//         OGL_generate_and_upload_image_as_texture(img, true);
//         if (img->texture_id == TEXTURE_NOT_GENERATED) {
//             error("image cannot create texture.");
//             return;
//         }
//         // console("PGraphicsOpenGLv33::image // uploaded texture image to GPU");
//     }
//
//     if (render_mode == RENDER_MODE_IMMEDIATE) {
//         const int _last_bound_texture_id_cache = texture_id_current;
//         IMPL_bind_texture(img->texture_id);
//         rect(x, y, w, h);
//         IMPL_bind_texture(_last_bound_texture_id_cache);
//         return;
//     }
//
//     if (render_mode == RENDER_MODE_BUFFERED) {
//         add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 0.0f, 0.0f);
//         add_fill_vertex_xyz_rgba_uv(x + w, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 1.0f, 0.0f);
//         add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 1.0f, 1.0f);
//
//         add_fill_vertex_xyz_rgba_uv(x + w, y + h, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 1.0f, 1.0f);
//         add_fill_vertex_xyz_rgba_uv(x, y + h, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 0.0f, 1.0f);
//         add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a, 0.0f, 0.0f);
//
//         constexpr int       RECT_NUM_VERTICES               = 6;
//         const unsigned long fill_vertices_count_xyz_rgba_uv = fill_vertices_xyz_rgba_uv.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
//         if (renderBatches.empty() || renderBatches.back().texture_id != img->texture_id) {
//             renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - RECT_NUM_VERTICES, RECT_NUM_VERTICES, img->texture_id);
//         } else {
//             renderBatches.back().num_vertices += RECT_NUM_VERTICES;
//         }
//     }
// }

/* --- RENDER_MODE_BUFFERED (RM) --- */

// void PGraphicsOpenGLv33::RM_render_line(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) {
//     if (render_lines_as_quads) {
//         auto p1 = glm::vec3(x1, y1, z1);
//         auto p2 = glm::vec3(x2, y2, z2);
//
//         to_screen_space(p1);
//         to_screen_space(p2);
//
//         glm::vec3 perp = p2 - p1;
//         perp           = glm::normalize(perp);
//         perp           = {-perp.y, perp.x, 0};
//         perp *= stroke_weight * 0.5f;
//
//         const glm::vec4 color{color_stroke.r,
//                               color_stroke.g,
//                               color_stroke.b,
//                               color_stroke.a};
//
//         add_transformed_fill_vertex_xyz_rgba_uv(p1 + perp, color);
//         add_transformed_fill_vertex_xyz_rgba_uv(p2 + perp, color);
//         add_transformed_fill_vertex_xyz_rgba_uv(p1 - perp, color);
//         add_transformed_fill_vertex_xyz_rgba_uv(p1 - perp, color);
//         add_transformed_fill_vertex_xyz_rgba_uv(p2 + perp, color);
//         add_transformed_fill_vertex_xyz_rgba_uv(p2 - perp, color);
//         RM_add_texture_id_to_render_batch(fill_vertices_xyz_rgba_uv, 6, texture_id_solid_color);
//     } else {
//         add_stroke_vertex_xyz_rgba(x1, y1, z1, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//         add_stroke_vertex_xyz_rgba(x2, y2, z2, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//     }
// }

// /**
//  * render line strip as individual quads per line segment, with optional round corners
//  * @param points
//  * @param color
//  * @param close_shape
//  * @param round_corners
//  */
// void PGraphicsOpenGLv33::RM_render_line_strip_as_quad_segments(const std::vector<glm::vec3>& points,
//                                                                const glm::vec4&              color,
//                                                                const bool                    close_shape,
//                                                                const bool                    round_corners) {
//     if (points.size() < 2) {
//         return;
//     }
//
//     if (points.size() == 2) {
//         // TODO replace with raw version
//         line(points[0].x, points[0].y, points[0].z,
//              points[1].x, points[1].y, points[1].z);
//         return;
//     }
//
//     const uint32_t num_line_segments = points.size() - (close_shape ? 0 : 1);
//     for (size_t i = 0; i < num_line_segments; i++) {
//         const size_t ii = (i + 1) % points.size();
//         line(points[i].x, points[i].y, points[i].z,
//              points[ii].x, points[ii].y, points[ii].z);
//         if (round_corners) {
//             // TODO must be transformed to screenspace!
//             RM_render_ellipse_filled(points[i].x, points[i].y, stroke_weight, stroke_weight, 8, color);
//         }
//     }
// }

// /**
//  * render line strip as quads with pointy corners
//  * @param points
//  * @param color
//  * @param close_shape
//  */
// void PGraphicsOpenGLv33::RM_render_line_strip_as_connected_quads(std::vector<glm::vec3>& points,
//                                                                  const glm::vec4&        color,
//                                                                  const bool              close_shape) {
//     if (points.size() < 2) {
//         return;
//     }
//
//     if (points.size() == 2) {
//         line(points[0].x, points[0].y, points[0].z,
//              points[1].x, points[1].y, points[1].z);
//         return;
//     }
//
//     // Transform all points to screen space first
//     std::vector<glm::vec3> screen_points = points;
//     std::vector<glm::vec3> normals(screen_points.size());
//     std::vector<glm::vec3> directions(screen_points.size());
//
//     for (auto& p: screen_points) {
//         to_screen_space(p);
//     }
//
//     for (size_t i = 0; i < normals.size(); i++) {
//         const size_t ii   = (i + 1) % screen_points.size();
//         glm::vec3&   p1   = screen_points[i];
//         glm::vec3&   p2   = screen_points[ii];
//         glm::vec3    dir  = p2 - p1;
//         glm::vec3    perp = glm::normalize(dir);
//         perp              = glm::vec3(-perp.y, perp.x, 0);
//         directions[i]     = dir;
//         normals[i]        = perp;
//     }
//
//     const float half_width = stroke_weight * 0.5f;
//     uint32_t    vertex_count{0};
//
//     glm::vec3 p1_left{};
//     glm::vec3 p1_right{};
//
//     const uint32_t num_line_segments = screen_points.size() + (close_shape ? 1 : 0);
//
//     for (size_t i = 0; i < num_line_segments; i++) {
//         const glm::vec3 point     = screen_points[i % screen_points.size()];
//         const glm::vec3 direction = directions[i % directions.size()];
//         const glm::vec3 normal    = normals[i % normals.size()];
//
//         const glm::vec3 next_point     = screen_points[(i + 1) % screen_points.size()];
//         const glm::vec3 next_direction = directions[(i + 1) % directions.size()];
//         const glm::vec3 next_normal    = normals[(i + 1) % normals.size()];
//
//         const glm::vec3 p2_left  = point + normal * half_width;
//         const glm::vec3 p2_right = point - normal * half_width;
//         const glm::vec3 p3_left  = next_point + next_normal * half_width;
//         const glm::vec3 p3_right = next_point - next_normal * half_width;
//
//         glm::vec2  intersection_left;
//         const bool result_left = intersect_lines(p2_left, direction,
//                                                  p3_left, next_direction,
//                                                  intersection_left);
//         glm::vec2  intersection_right;
//         const bool result_right = intersect_lines(p2_right, direction,
//                                                   p3_right, next_direction,
//                                                   intersection_right);
//
//         // TODO this is just for surgecially removing things:
//         int  RENDER_LINE_STRIP_AS_QUADS_MAX_ANGLE = 0;
//         bool line_segments_are_too_pointy         = are_almost_parallel(normal, next_normal, RENDER_LINE_STRIP_AS_QUADS_MAX_ANGLE);
//
//         if (!result_left) {
//             intersection_left = p3_left;
//         }
//
//         if (!result_right) {
//             intersection_right = p3_right;
//         }
//
//         /*
//          * 1--2
//          * |\ |
//          * | \|
//          * 0--3
//          */
//         if (close_shape) {
//             /* closed */
//             if (i != 0) {
//                 if (line_segments_are_too_pointy) {
//                     RM_add_quad_as_triangles(p1_left,
//                                              next_point + normal * half_width,
//                                              next_point - normal * half_width,
//                                              p1_right,
//                                              color);
//                     p1_left  = next_point - normal * half_width;
//                     p1_right = next_point + normal * half_width;
//                     vertex_count += 6;
//                 } else {
//                     RM_add_quad_as_triangles(p1_left,
//                                              glm::vec3(intersection_left, 0),
//                                              glm::vec3(intersection_right, 0),
//                                              p1_right,
//                                              color);
//                     p1_left  = glm::vec3(intersection_left, 0);
//                     p1_right = glm::vec3(intersection_right, 0);
//                     vertex_count += 6;
//                 }
//             } else {
//                 if (line_segments_are_too_pointy) {
//                     p1_left  = next_point - normal * half_width;
//                     p1_right = next_point + normal * half_width;
//                 } else {
//                     p1_left  = glm::vec3(intersection_left, 0);
//                     p1_right = glm::vec3(intersection_right, 0);
//                 }
//             }
//         } else {
//             /* open */
//             if (i == 0) {
//                 // first segment
//                 if (line_segments_are_too_pointy) {
//                     console("x");
//                     RM_add_quad_as_triangles(p2_left,
//                                              p3_right,
//                                              p3_left,
//                                              p2_right,
//                                              color);
//                     p1_left  = p3_left;
//                     p1_right = p3_right;
//                     // RM_add_quad_as_triangles(p2_left,
//                     //                          next_point + normal * half_width,
//                     //                          next_point - normal * half_width,
//                     //                          p2_right,
//                     //                          color);
//                     // p1_left  = next_point - normal * half_width;
//                     // p1_right = next_point + normal * half_width;
//                     vertex_count += 6;
//                 } else {
//                     RM_add_quad_as_triangles(p2_left,
//                                              glm::vec3(intersection_left, 0),
//                                              glm::vec3(intersection_right, 0),
//                                              p2_right,
//                                              color);
//                     p1_left  = glm::vec3(intersection_left, 0);
//                     p1_right = glm::vec3(intersection_right, 0);
//                     vertex_count += 6;
//                 }
//             } else if (i == num_line_segments - 2) {
//                 // last segment
//                 RM_add_quad_as_triangles(p1_left,
//                                          next_point + normal * half_width,
//                                          next_point - normal * half_width,
//                                          p1_right,
//                                          color);
//                 p1_left  = next_point + normal * half_width;
//                 p1_right = next_point - normal * half_width;
//                 vertex_count += 6;
//             } else if (i < num_line_segments - 2) {
//                 // other segments
//                 if (line_segments_are_too_pointy) {
//                     RM_add_quad_as_triangles(p1_left,
//                                              next_point + normal * half_width,
//                                              next_point - normal * half_width,
//                                              p1_right,
//                                              color);
//                     p1_left  = next_point - normal * half_width;
//                     p1_right = next_point + normal * half_width;
//                     vertex_count += 6;
//                 } else {
//                     RM_add_quad_as_triangles(p1_left,
//                                              glm::vec3(intersection_left, 0),
//                                              glm::vec3(intersection_right, 0),
//                                              p1_right,
//                                              color);
//                     p1_left  = glm::vec3(intersection_left, 0);
//                     p1_right = glm::vec3(intersection_right, 0);
//                     vertex_count += 6;
//                 }
//             }
//             // TODO this could be used for round caps
//         }
//     }
//
//     RM_add_texture_id_to_render_batch(fill_vertices_xyz_rgba_uv, static_cast<int>(vertex_count), texture_id_solid_color);
// }

// void PGraphicsOpenGLv33::RM_render_rect(const float x, const float y, const float width, const float height) {
//     if (color_stroke.active) {
//         add_stroke_vertex_xyz_rgba(x, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//         add_stroke_vertex_xyz_rgba(x + width, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//
//         add_stroke_vertex_xyz_rgba(x + width, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//         add_stroke_vertex_xyz_rgba(x + width, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//
//         add_stroke_vertex_xyz_rgba(x + width, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//         add_stroke_vertex_xyz_rgba(x, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//
//         add_stroke_vertex_xyz_rgba(x, y + height, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//         add_stroke_vertex_xyz_rgba(x, y, 0, color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//     }
//     if (color_fill.active) {
//         add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
//         add_fill_vertex_xyz_rgba_uv(x + width, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
//         add_fill_vertex_xyz_rgba_uv(x + width, y + height, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
//
//         add_fill_vertex_xyz_rgba_uv(x + width, y + height, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
//         add_fill_vertex_xyz_rgba_uv(x, y + height, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
//         add_fill_vertex_xyz_rgba_uv(x, y, 0, color_fill.r, color_fill.g, color_fill.b, color_fill.a);
//
//         constexpr int       RECT_NUM_VERTICES               = 6;
//         const unsigned long fill_vertices_count_xyz_rgba_uv = fill_vertices_xyz_rgba_uv.size() / NUM_FILL_VERTEX_ATTRIBUTES_XYZ_RGBA_UV;
//         if (renderBatches.empty() || renderBatches.back().texture_id != texture_id_solid_color) {
//             renderBatches.emplace_back(fill_vertices_count_xyz_rgba_uv - RECT_NUM_VERTICES, RECT_NUM_VERTICES, texture_id_solid_color);
//         } else {
//             renderBatches.back().num_vertices += RECT_NUM_VERTICES;
//         }
//     }
// }

// void PGraphicsOpenGLv33::RM_render_ellipse_filled(const float x, const float y,
//                                                   const float      width,
//                                                   const float      height,
//                                                   const int        detail,
//                                                   const glm::vec4& color) {
//     const float            radiusX = width / 2.0f;
//     const float            radiusY = height / 2.0f;
//     std::vector<glm::vec3> points;
//     for (int i = 0; i <= detail; ++i) {
//         const float     theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(detail);
//         const glm::vec3 p{x + radiusX * cosf(theta),
//                           y + radiusY * sinf(theta), 0};
//         points.push_back(p);
//     }
//     const glm::vec3     center{x, y, 0};
//     constexpr glm::vec2 tex_coords{0, 0};
//     for (int i = 0; i < points.size(); ++i) {
//         const glm::vec3 p1 = points[i];
//         const glm::vec3 p2 = points[(i + 1) % points.size()];
//         add_fill_vertex_xyz_rgba_uv(center, color, tex_coords);
//         add_fill_vertex_xyz_rgba_uv(p1, color, tex_coords);
//         add_fill_vertex_xyz_rgba_uv(p2, color, tex_coords);
//     }
//     RM_add_texture_id_to_render_batch(fill_vertices_xyz_rgba_uv, static_cast<int>(points.size() * 3), texture_id_solid_color);
// }
//
// void PGraphicsOpenGLv33::RM_render_ellipse(const float x, const float y, const float width, const float height) {
//     if (color_stroke.active) {
//         const float            radiusX = width / 2.0f;
//         const float            radiusY = height / 2.0f;
//         std::vector<glm::vec3> points;
//         for (int i = 0; i <= ellipse_detail; ++i) {
//             const float     theta = 2.0f * 3.1415926f * static_cast<float>(i) / static_cast<float>(ellipse_detail);
//             const glm::vec3 p{x + radiusX * cosf(theta),
//                               y + radiusY * sinf(theta), 0};
//             points.push_back(p);
//         }
//         const glm::vec4 color{color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a};
//         RM_render_line_strip_as_quad_segments(points, color, true, false);
//     }
//     if (color_fill.active) {
//         const glm::vec4 color{color_fill.r, color_fill.g, color_fill.b, color_fill.a};
//         RM_render_ellipse_filled(x, y, width, height, ellipse_detail, color);
//     }
// }

/* --- RENDER_MODE_IMMEDIATE (IM) --- */

// void PGraphicsOpenGLv33::IM_render_line(const float x1, const float y1, const float z1,
//                                         const float x2, const float y2, const float z2) {
//     // ensure primitive is initialized
//     if (IM_primitive_line.uninitialized()) {
//         OGL3_init_vertex_buffer(IM_primitive_line);
//     }
//
//     IM_primitive_line.vertices[0].position  = glm::vec3(x1, y1, z1);
//     IM_primitive_line.vertices[0].color     = glm::vec4(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//     IM_primitive_line.vertices[0].tex_coord = glm::vec2(0.0f, 0.0f);
//     IM_primitive_line.vertices[1].position  = glm::vec3(x2, y2, z2);
//     IM_primitive_line.vertices[1].color     = glm::vec4(color_stroke.r, color_stroke.g, color_stroke.b, color_stroke.a);
//     IM_primitive_line.vertices[1].tex_coord = glm::vec2(1.0f, 0.0f);
//     OGL_tranform_model_matrix_and_render_vertex_buffer(IM_primitive_line, GL_LINES, IM_primitive_line.vertices);
// }

// void PGraphicsOpenGLv33::IM_render_ellipse(const float x, const float y, const float width, const float height) {
//     if (!color_fill.active && !color_stroke.active) {
//         return;
//     }
//
//     // TODO: Implement `ellipseMode()`
//     constexpr glm::vec2 tex_coord = {0.0f, 0.0f};
//     const float         radiusX   = width * 0.5f;
//     const float         radiusY   = height * 0.5f;
//
//     std::vector<glm::vec3> points;
//     points.reserve(ellipse_detail + 1);
//
//     // TODO create and recompute LUT for when `ellipse_detail` changes
//     float i_f = 0.0f;
//     for (int i = 0; i <= ellipse_detail; ++i, i_f += 1.0f) {
//         points.emplace_back(x + radiusX * ellipse_points_LUT[i].x,
//                             y + radiusY * ellipse_points_LUT[i].y,
//                             0.0f);
//     }
//
//     if (RENDER_PRIMITVES_AS_SHAPES) {
//         // maybe separate into TRIANGLE_FAN and LINE_STRIP
//         beginShape(POLYGON);
//         points.pop_back();
//         for (const auto& p: points) {
//             vertex(p.x, p.y);
//         }
//         endShape(CLOSE);
//     } else {
//         shape_fill_vertex_buffer.clear();
//         shape_fill_vertex_buffer.reserve(ellipse_detail + 2);
//
//         const glm::vec3 center{x, y, 0};
//
//         if (color_fill.active) {
//             const glm::vec4 fill_color = as_vec4(color_fill);
//
//             shape_fill_vertex_buffer.emplace_back(center, fill_color, tex_coord);
//             for (const auto& p: points) {
//                 shape_fill_vertex_buffer.emplace_back(p, fill_color, tex_coord);
//             }
//
//             OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_TRIANGLE_FAN, shape_fill_vertex_buffer);
//         }
//
//         if (color_stroke.active) {
//             const glm::vec4 stroke_color = as_vec4(color_stroke);
//
//             shape_fill_vertex_buffer.clear();
//             for (const auto& p: points) {
//                 shape_fill_vertex_buffer.emplace_back(p, stroke_color, tex_coord);
//             }
//             if (line_render_mode == STROKE_RENDER_MODE_TRIANGULATE) {
//                 std::vector<Vertex> line_vertices;
//                 shape_fill_vertex_buffer.pop_back();
//                 triangulate_line_strip_vertex(shape_fill_vertex_buffer, true, line_vertices);
//                 emit_shape_fill_triangles(line_vertices);
//             }
//             if (line_render_mode == STROKE_RENDER_MODE_NATIVE) {
//                 OGL_tranform_model_matrix_and_render_vertex_buffer(vertex_buffer_data, GL_LINE_STRIP, shape_fill_vertex_buffer);
//             }
//         }
//     }
// }
