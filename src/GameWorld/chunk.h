#pragma once

#include "../Maths/maths.h"
#include <stdint.h>

#define CHUNK_STORE_SIZE    16

class Chunk
{
public:
    enum Flags
    {
        Dirty           = 1 << 0,
        HasMesh         = 1 << 1
    };

    Chunk(class BlockStore *blockStore, class World *world, Vec3 offset, int size);
    ~Chunk();

    void regenerateMesh();
    static IVec3 getChunkId(IVec3 block);

    int size;
    Vec3 offset;
    int flags;
    class Mesh *mesh;
    class BlockStore *blockStore;
    class World *world;
};
