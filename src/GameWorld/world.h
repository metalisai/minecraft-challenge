#pragma once

#include <unordered_map>
#include <stdint.h>

#include "../Maths/maths.h"
#include "chunk.h"
#include "chunkmanager.h"
#include "../camera.h"

struct ChunkData
{
    enum Flags
    {
        Populated = 1 << 0
    };

    int flags;
    uint8_t data[CHUNK_STORE_SIZE*CHUNK_STORE_SIZE*CHUNK_STORE_SIZE];
};

struct RaycastHit
{
    Vec3 point;
    IVec3 block;
};

class World
{
public:
    World(class Renderer *renderer, class BlockStore *blockStore, class Camera *cam);
    ~World();

    uint8_t getBlockId(IVec3 block);
    uint8_t setBlockId(IVec3 block, uint8_t newId);
    bool lineCast(RaycastHit& hit, Vec3 start, Vec3 end);

    void update();
    void render();

    std::unordered_map<IVec3, ChunkData*> chunks;

    class Camera *mainCam;
    class Renderer *renderer;
    class BlockStore *blockStore;
    ChunkManager chunkManager;
};
