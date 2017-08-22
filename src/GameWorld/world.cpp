#include "world.h"
#include <math.h>

#include "../Maths/maths.h"

World::World(Renderer *renderer, BlockStore *blockStore, Camera *cam)
    : chunkManager(renderer, cam, blockStore, this), worldGenerator(this)
{
    this->mainCam = cam;
}

World::~World()
{
    for(auto it = chunks.begin(); it != chunks.end(); ++it )
    {
        delete it->second;
    }
    chunks.clear();
}

ChunkData* World::getOrCreateChunkData(IVec3 chunkId)
{
    if(this->lastChunkAccess != nullptr 
            && this->lastChunkAccess->offset == chunkId)
    {
        return this->lastChunkAccess;
    }

    ChunkData *ret;
    auto fchunk = this->chunks.find(chunkId);
    if(fchunk != this->chunks.end())
    {
        ret = fchunk->second;
    }
    else
    {
        ret = new ChunkData(chunkId);
        this->chunks.insert({chunkId, ret});
        worldGenerator.fillChunk(chunkId);
    }
    this->lastChunkAccess = ret;
    return ret;
}

uint8_t World::getBlockId(IVec3 block)
{
    IVec3 chunkId = Chunk::getChunkId(block);
    IVec3 localOffset(block.x - chunkId.x, block.y - chunkId.y, block.z - chunkId.z);
    auto fchunk = this->chunks.find(chunkId);
    
    ChunkData *cdata = getOrCreateChunkData(chunkId);

    const int s = CHUNK_STORE_SIZE;
    uint8_t ret = cdata->data[localOffset.x*s*s + localOffset.y*s + localOffset.z];
    return ret;
}

// TODO: share code with getBlockId
uint8_t World::setBlockId(IVec3 block, uint8_t newId)
{
    IVec3 chunkId = Chunk::getChunkId(block);
    IVec3 localOffset(block.x - chunkId.x, block.y - chunkId.y, block.z - chunkId.z);
    
    ChunkData *cdata = getOrCreateChunkData(chunkId);

    const int s = CHUNK_STORE_SIZE;
    uint8_t ret = cdata->data[localOffset.x*s*s + localOffset.y*s + localOffset.z];
    cdata->data[localOffset.x*s*s + localOffset.y*s + localOffset.z] = newId;
    if(ret != newId)
        chunkManager.blockChanged(block);
    return ret;
}

bool World::lineCast(RaycastHit &hit, Vec3 start, Vec3 end)
{
    const float step = 0.01f;
    int steps = (start-end).length() / step;
    float progress = 0.0f;

    Vec3 ray = (end - start).normalized();

    IVec3 lastBlock;
    for(int i = 0; i < steps; i++)
    {
        Vec3 point = start + progress*ray;
        IVec3 blockV((int)floor(point.x), (int)floor(point.y), (int)floor(point.z));
        if(!(blockV == lastBlock))
        {
            uint8_t bid = getBlockId(blockV);    
            if(bid != 0 && bid != 9)
            {
                hit.point = point;
                hit.block = blockV;
                return true;
            }
            lastBlock = blockV;
        }
        progress += step;
    }
    return false;
}

void World::update()
{
    chunkManager.update();
}

void World::render()
{
    chunkManager.render();
}

