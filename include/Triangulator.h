#pragma once

#include <vector>
#include <memory>
#include "glm/glm.hpp"
#include "libtess2/tesselator.h"
#include "Vertex.h"

namespace umfeld {
    class Triangulator {
    public:
        enum Winding {
            WINDING_ODD,
            WINDING_NONZERO,
            WINDING_POSITIVE,
            WINDING_NEGATIVE,
            WINDING_ABS_GEQ_TWO
        };

        Triangulator();
        ~Triangulator();
        std::vector<Vertex>    triangulate(const std::vector<Vertex>& inputVertices, Winding winding = WINDING_ODD) const;
        std::vector<glm::vec2> triangulate(const std::vector<glm::vec2>& inputVertices, Winding winding = WINDING_ODD) const;

    private:
        void                            allocate();
        std::shared_ptr<TESStesselator> mTess;
        int                             mAllocated;
    };

    inline Triangulator::Triangulator() {
        allocate();
    }

    inline Triangulator::~Triangulator() {
        // NOTE libtess2 handles cleanup with shared_ptr
        // TODO ^^^ check this ;)
    }

    inline void Triangulator::allocate() {
        mAllocated   = 0;
        TESSalloc ma = {};
        ma.memalloc  = [](void* userData, const unsigned int size) -> void* {
            const auto allocated = static_cast<int*>(userData);
            *allocated += static_cast<int>(size);
            return malloc(size);
        };
        ma.memfree = [](void*, void* ptr) {
            free(ptr);
        };
        ma.userData      = &mAllocated;
        ma.extraVertices = 256;
        mTess            = std::shared_ptr<TESStesselator>(tessNewTess(&ma), tessDeleteTess);
    }

    inline std::vector<Vertex> Triangulator::triangulate(const std::vector<Vertex>& inputVertices, const Winding winding) const {
        std::vector<Vertex> outputTriangles;

        if (inputVertices.empty()) {
            return outputTriangles;
        }

        tessAddContour(mTess.get(), 2, &inputVertices[0].position, sizeof(Vertex), static_cast<int>(inputVertices.size()));

        if (!tessTesselate(mTess.get(), winding, TESS_POLYGONS, 3, 2, nullptr)) {
            return outputTriangles;
        }

        const float* tessVertices = tessGetVertices(mTess.get());
        const int*   tessElements = tessGetElements(mTess.get());
        const int    vertexCount  = tessGetVertexCount(mTess.get());
        const int    elementCount = tessGetElementCount(mTess.get());

        std::vector<glm::vec2> positions(vertexCount);
        for (int i = 0; i < vertexCount; ++i) {
            positions[i] = glm::vec2(tessVertices[i * 2], tessVertices[i * 2 + 1]);
        }

        const glm::vec4 color     = inputVertices[0].color;
        const glm::vec4 normal    = inputVertices[0].normal;
        const glm::vec2 tex_coord = inputVertices[0].tex_coord;
        for (int i = 0; i < elementCount * 3; i += 3) {
            const int idx0 = tessElements[i];
            const int idx1 = tessElements[i + 1];
            const int idx2 = tessElements[i + 2];
            if (idx0 == TESS_UNDEF || idx1 == TESS_UNDEF || idx2 == TESS_UNDEF) {
                continue;
            }
            outputTriangles.emplace_back(glm::vec3(positions[idx0], 0.0f), color, tex_coord, normal);
            outputTriangles.emplace_back(glm::vec3(positions[idx1], 0.0f), color, tex_coord, normal);
            outputTriangles.emplace_back(glm::vec3(positions[idx2], 0.0f), color, tex_coord, normal);
        }

        return outputTriangles;
    }

    inline std::vector<glm::vec2> Triangulator::triangulate(const std::vector<glm::vec2>& inputVertices, const Winding winding) const {
        std::vector<glm::vec2> outputTriangles;

        if (inputVertices.empty()) {
            return outputTriangles;
        }

        tessAddContour(mTess.get(), 2, inputVertices.data(), sizeof(glm::vec2), static_cast<int>(inputVertices.size()));

        if (!tessTesselate(mTess.get(), winding, TESS_POLYGONS, 3, 2, nullptr)) {
            return outputTriangles;
        }

        const float* tessVertices = tessGetVertices(mTess.get());
        const int*   tessElements = tessGetElements(mTess.get());
        const int    elementCount = tessGetElementCount(mTess.get());

        outputTriangles.reserve(elementCount * 3); // Reserve space for triangles

        for (int i = 0; i < elementCount * 3; i += 3) {
            const int idx0 = tessElements[i];
            const int idx1 = tessElements[i + 1];
            const int idx2 = tessElements[i + 2];

            if (idx0 == TESS_UNDEF || idx1 == TESS_UNDEF || idx2 == TESS_UNDEF) {
                continue;
            }

            outputTriangles.emplace_back(tessVertices[idx0 * 2], tessVertices[idx0 * 2 + 1]);
            outputTriangles.emplace_back(tessVertices[idx1 * 2], tessVertices[idx1 * 2 + 1]);
            outputTriangles.emplace_back(tessVertices[idx2 * 2], tessVertices[idx2 * 2 + 1]);
        }

        return outputTriangles;
    }
} // namespace umfeld