#pragma once

#include <unordered_map>
#include <stdint.h>

#include "../Maths/maths.h"
#include "chunk.h"
#include "chunkmanager.h"
#include "worldgenerator.h"

struct ChunkData
{
    enum Flags
    {
        Populated = 1 << 0
    };

    ChunkData(IVec3 offset)
    {
        this->offset = offset;
    }

    int flags;
    IVec3 offset;
    uint8_t data[CHUNK_STORE_SIZE*CHUNK_STORE_SIZE*CHUNK_STORE_SIZE];
};

struct RaycastHit
{
    Vec3 point;
    IVec3 block;
    IVec3 faceDirection;
};

class World
{
public:
    World(class Renderer *renderer, class BlockStore *blockStore, class Camera *cam);
    ~World();

    class ChunkData* getOrCreateChunkData(IVec3 chunkId);
    uint8_t getBlockId(IVec3 block);
    uint8_t setBlockId(IVec3 block, uint8_t newId);
    bool lineCast(RaycastHit& hit, Vec3 start, Vec3 end);

    void update();
    void render();
    void setCamera(class Camera *cam);

    std::unordered_map<IVec3, ChunkData*> chunks;

    class Camera *mainCam;
    class Renderer *renderer;
    class BlockStore *blockStore;
    ChunkData *lastChunkAccess = nullptr;
    ChunkManager chunkManager;
    WorldGenerator worldGenerator;

    Vec3 gravity;
};
