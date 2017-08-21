#include "world.h"
#include <math.h>

#include "../Maths/maths.h"

World::World(Renderer *renderer, BlockStore *blockStore, Camera *cam)
    : chunkManager(renderer, cam, blockStore, this)
{
    this->mainCam = cam;

    for(int i = -1; i < 2; i++)
    for(int j = -1; j < 2; j++)
    {
        chunkManager.loadChunk(IVec3(i*16, j*16, -16));
        chunkManager.loadChunk(IVec3(i*16, j*16, 0));
    }
}

World::~World()
{
    for(auto it = chunks.begin(); it != chunks.end(); ++it )
    {
        delete it->second;
    }
    chunks.clear();
}

static int calcBlockId(Vec3 position)
{
    if(position.length() >= 10.0f && position.y < 0.0f)
        return 2;
    else
        return 0;
}

uint8_t World::getBlockId(IVec3 block)
{
    IVec3 chunkId = Chunk::getChunkId(block);
    IVec3 localOffset(block.x - chunkId.x, block.y - chunkId.y, block.z - chunkId.z);
    auto fchunk = this->chunks.find(chunkId);
    
    ChunkData *cdata;

    const int s = CHUNK_STORE_SIZE;

    if(fchunk != this->chunks.end())
    {
        cdata = fchunk->second;
    }
    else
    {
        printf("not found! %d %d %d\n", chunkId.x, chunkId.y, chunkId.z);
        cdata = new ChunkData();
        for(int i = 0; i < CHUNK_STORE_SIZE; i++)
        for(int j = 0; j < CHUNK_STORE_SIZE; j++)
        for(int k = 0; k < CHUNK_STORE_SIZE; k++)
        {
            Vec3 offset(chunkId.x + i, chunkId.y + j, chunkId.z + k);
            cdata->data[i*s*s + j*s + k] = calcBlockId(offset);
        }
        this->chunks.insert({chunkId, cdata});
    }
    uint8_t ret = cdata->data[localOffset.x*s*s + localOffset.y*s + localOffset.z];
    return ret;
}

// TODO: share code with getBlockId
uint8_t World::setBlockId(IVec3 block, uint8_t newId)
{
    IVec3 chunkId = Chunk::getChunkId(block);
    IVec3 localOffset(block.x - chunkId.x, block.y - chunkId.y, block.z - chunkId.z);
    auto fchunk = this->chunks.find(chunkId);
    
    ChunkData *cdata;

    const int s = CHUNK_STORE_SIZE;

    if(fchunk != this->chunks.end())
    {
        cdata = fchunk->second;
    }
    else
    {
        cdata = new ChunkData();
        for(int i = 0; i < CHUNK_STORE_SIZE; i++)
        for(int j = 0; j < CHUNK_STORE_SIZE; j++)
        for(int k = 0; k < CHUNK_STORE_SIZE; k++)
        {
            Vec3 offset(chunkId.x + i, chunkId.y + j, chunkId.z + k);
            cdata->data[i*s*s + j*s + k] = calcBlockId(offset);
        }
        this->chunks.insert({chunkId, cdata});
    }
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
            if(bid != 0)
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

