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

#pragma once

#include <vector>
#include <tuple>
#include <cstddef>
#include <GL/glew.h>
#include <type_traits>
#include "Vertex.h"

#include <tuple>
#include <vector>
#include <cstddef>

using umgebung::Vertex;

// Helper: Get OpenGL type from C++ type
template<typename T>
constexpr GLenum getGLType() {
    if constexpr (std::is_same_v<T, float>) {
        return GL_FLOAT;
    }
    if constexpr (std::is_same_v<T, int>) {
        return GL_INT;
    }
    return 0; // Unknown type (should not happen)
}

// Struct to define a field with metadata
template<typename StructType, typename MemberType, MemberType StructType::* Member, size_t Index>
struct VertexAttribute {
    static constexpr GLenum type     = getGLType<typename MemberType::value_type>();
    static constexpr size_t size     = sizeof(MemberType) / sizeof(float); // Number of components (e.g., vec3 -> 3)
    static constexpr size_t offset   = offsetof(StructType, Member);       // FIXED: No dereference
    static constexpr size_t location = Index;
};

// Macro to simplify member definition
#define DEFINE_ATTRIBUTE(structType, memberName, index) \
    VertexAttribute<structType, decltype(structType::memberName), &structType::memberName, index>

template<typename VertexType, typename... Attributes>
struct VertexLayout {
    using AttributesTuple = std::tuple<Attributes...>;
    AttributesTuple attributesTuple; // Now it holds an instance
};

// Automatically extract attributes using index sequence
template<typename VertexType, typename... Members, size_t... Indices>
constexpr auto makeVertexLayout(std::tuple<Members...> members, std::index_sequence<Indices...>) {
    return VertexLayout<VertexType, DEFINE_ATTRIBUTE(VertexType, Members::value, Indices)...>{};
}

// Entry point to infer attributes
template<typename VertexType, typename... Members>
constexpr auto inferVertexLayout(std::tuple<Members...> members) {
    return makeVertexLayout<VertexType>(members, std::index_sequence_for<Members...>{});
}

// // Specialization for Vertex
// template <>
// constexpr auto inferVertexLayout<Vertex>(std::tuple{}) {
//  return makeVertexLayout<Vertex>(
//      std::make_tuple(
//          &Vertex::position,
//          &Vertex::color,
//          &Vertex::tex_coord
//      ),
//      std::index_sequence_for<decltype(Vertex::position), decltype(Vertex::color), decltype(Vertex::tex_coord)>{});
// }

// // Specialization for VertexBarycentric
// template <>
// constexpr auto inferVertexLayout<VertexBarycentric>(std::tuple{}) {
//  return makeVertexLayout<VertexBarycentric>(
//      std::make_tuple(
//          &VertexBarycentric::position,
//          &VertexBarycentric::color,
//          &VertexBarycentric::barycentric
//      ),
//      std::index_sequence_for<decltype(VertexBarycentric::position), decltype(VertexBarycentric::color), decltype(VertexBarycentric::barycentric)>{});
// }

template<typename VertexType>
void setupAttributes(const VertexLayout<VertexType>& layout) { // Ensure it takes a const reference
    std::apply([](auto... attributes) {
        ((glEnableVertexAttribArray(attributes.location),
          glVertexAttribPointer(attributes.location, attributes.size, attributes.type, GL_FALSE,
                                sizeof(VertexType), (void*) attributes.offset)),
         ...);
    },
               layout.attributesTuple); // Use instance
}

template<typename VertexType>
GLuint createVAO(const std::vector<VertexType>& vertices) {
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType), vertices.data(), GL_STATIC_DRAW);

    // Fully automatic setup
    setupAttributes(inferVertexLayout<VertexType>({}));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}

struct VertexBarycentric {
    glm::vec3 position;
    glm::vec3 barycentric;
    glm::vec4 color;
};

inline GLuint test_this() {
    // create some vertices
    const std::vector<Vertex> vertices = {
        {1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
    };
    const GLuint vao = createVAO(vertices);
    const std::vector<VertexBarycentric> vertices2 = {
        {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
    };
    const GLuint vao2 = createVAO(vertices);
    return vao;
}