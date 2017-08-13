#pragma once

#include "Maths/maths.h"
#include <stdint.h>

class Mesh
{
public:

    enum Flags
    {
        Initialized = 1 << 0,
        HasVertices = 1 << 1,
        HasIndices = 1 << 2,
        Dirty = 1 << 3,
        Uploaded = 1 << 4
    };

    Mesh();
    void copyVertices(const Vec3 *vertices, uint32_t count);
    void copyIndices(const uint16_t *indices, uint32_t count);

    uint16_t flags;
    uint16_t numVertices;
    uint32_t numIndices;
    uintptr_t rendererHandle;
    // ugly IKR
    uintptr_t rendererHandle2;
    uintptr_t rendererHandle3;
    Vec3 *vertices;
    uint16_t *indices;
};
