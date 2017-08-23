#include "chunkmanager.h"

#include "chunk.h"
#include "../macros.h"
#include "../Renderer/renderer.h"
#include "../camera.h"

ChunkManager::ChunkManager(Renderer *renderer, Camera *camera, BlockStore *bs, World *world)
{
    this->renderer = renderer;
    this->camera = camera;
    this->blockStore = bs;
    this->world = world;

}

ChunkManager::~ChunkManager()
{
    for(auto it = loadedChunks.begin(); it != loadedChunks.end(); ++it )
    {
        delete it->second;
    }
    loadedChunks.clear();
}

bool ChunkManager::loadChunk(IVec3 chunkId)
{
    auto fchunk = loadedChunks.find(chunkId);
    if(fchunk == loadedChunks.end())
    {
        Chunk *chunk;
        chunk = new Chunk(blockStore, world, chunkId, 16);
        chunk->regenerateMesh();
        loadedChunks.insert({chunkId, chunk});
        return true;
    }
    return false;
}

void ChunkManager::unloadChunk(IVec3 chunkId)
{
    auto fchunk = loadedChunks.find(chunkId);
    if(fchunk != loadedChunks.end())
    {
        delete fchunk->second;
        loadedChunks.erase(fchunk);
    }
}

void ChunkManager::blockDirty(IVec3 block)
{
    IVec3 chunkId = Chunk::getChunkId(block);
    auto fchunk = loadedChunks.find(chunkId);
    if(fchunk != loadedChunks.end())
    {
        fchunk->second->flags |= Chunk::Flags::Dirty;
    }
}

void ChunkManager::blockChanged(IVec3 block)
{
    blockDirty(block + IVec3(-1, 0, 0));
    blockDirty(block + IVec3(0, -1, 0));
    blockDirty(block + IVec3(0, 0, -1));
    blockDirty(block);
}

void ChunkManager::update()
{
    Vec3 camPos = camera->transform.position;
    IVec3 camBlock((int)roundf(camPos.x), (int)roundf(camPos.y), (int)roundf(camPos.z));
    IVec3 camChunk = Chunk::getChunkId(camBlock);

    IVec3 loadedC[1000];
    int loadedCCount = 0;


    bool loadedSomething = false;

    for(int i = -5; i < 6; i++)
    for(int j = -5; j < 6; j++)
    {
        IVec3 upper(camChunk.x + i*16, -16, camChunk.z + j*16);
        IVec3 lower(camChunk.x + i*16,   0, camChunk.z + j*16);
        if(!loadedSomething)
        {
            loadedSomething = loadedSomething || loadChunk(upper);
            loadedSomething = loadedSomething || loadChunk(lower);
        }
        loadedC[loadedCCount++] = upper;
        loadedC[loadedCCount++] = lower;
    }

    if(frameId % 60 == 0) // delete chunks every 60th frame
    {
        for(auto it = loadedChunks.begin(); it != loadedChunks.end();)
        {
            IVec3 offset = it->second->offset;
            bool found = false;
            for(int i = 0; i < loadedCCount; i++)
            {
                if(offset == loadedC[i])
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                ++it; // increment first so that the iterator remains valid
                unloadChunk(offset);
            }
            else
                ++it;
        }
    }


    for(auto it = loadedChunks.begin(); it != loadedChunks.end(); ++it )
    {
        Chunk *chunk = it->second;
        if(FLAGSET(chunk->flags, Chunk::Flags::Dirty))
        {
            chunk->regenerateMesh();
        }
    }

    frameId++;
}

void ChunkManager::render()
{
    Mat4 world_to_clip = camera->getViewProjectionMatrix();
    // TODO: optimize, don't recalculate matrices for waterMesh
    renderer->setBlend(false);
    for(auto it = loadedChunks.begin(); it != loadedChunks.end(); ++it )
    {
        // TODO: only need transform
        IVec3 offset = it->second->offset;
        Vec3 offsetf(offset.x, offset.y, offset.z);
        Mat4 model_to_world = Mat4::TRS(offsetf, Quaternion::Identity(), Vec3(1.0f, 1.0f, 1.0f));
        //Mat4 model_to_clip = world_to_clip * model_to_world;
        renderer->renderMesh(it->second->mesh, renderer->defaultMaterial, &model_to_world, &world_to_clip);
    }
    renderer->setBlend(true);
    for(auto it = loadedChunks.begin(); it != loadedChunks.end(); ++it )
    {
        IVec3 offset = it->second->offset;
        Vec3 offsetf(offset.x, offset.y, offset.z);
        Mat4 model_to_world = Mat4::TRS(offsetf, Quaternion::Identity(), Vec3(1.0f, 1.0f, 1.0f));
        //Mat4 model_to_clip = world_to_clip * model_to_world;
        renderer->renderMesh(it->second->waterMesh, renderer->defaultMaterial, &model_to_world, &world_to_clip);
    }

}

