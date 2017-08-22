#pragma once

#include "../Maths/maths.h"
#include <unordered_map>

class ChunkManager
{
public:
    ChunkManager(class Renderer* renderer, class Camera* camera, class BlockStore *blockStore, class World *world);
    ~ChunkManager();

    bool loadChunk(IVec3 chunkId);
    void unloadChunk(IVec3 chunkId);
    void blockDirty(IVec3 block);
    void blockChanged(IVec3 block);
    void update();
    void render();

    std::unordered_map<IVec3, class Chunk*> loadedChunks;

    class Renderer *renderer;
    class Camera *camera;
    class BlockStore *blockStore;
    class World *world;
    int frameId = 0;
};
