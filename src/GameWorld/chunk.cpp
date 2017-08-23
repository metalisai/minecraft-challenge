#include "chunk.h"
#include "../macros.h"

#include <assert.h>
#include "../Renderer/mesh.h"
#include "blockstore.h"
#include "world.h"
#include <cstring>

namespace
{
    extern Vec3 sideOffsets[6];
    extern Vec3 sideQuads[3][4];
    extern Vec3 directions[3];
    extern Vec3 sideTexCoords[3][4];
    extern uint16_t quadIndices[6]; 
}

Chunk::Chunk(BlockStore *blockStore, class World *world, IVec3 offset, int size)
{
    this->flags = 0;
    this->offset = offset;
    this->size = size;

    this->mesh = nullptr;
    this->blockStore = blockStore;
    this->world = world;
}

Chunk::~Chunk()
{
    if(FLAGSET(this->flags, Flags::HasMesh))
        delete this->mesh;
    if(FLAGSET(this->flags, Flags::HasWaterMesh))
        delete this->waterMesh;
}

IVec3 Chunk::getChunkId(IVec3 block)
{
    IVec3 ret;
    ret.x = ((int)floor(block.x / (float)CHUNK_STORE_SIZE)) * CHUNK_STORE_SIZE;
    ret.y = ((int)floor(block.y / (float)CHUNK_STORE_SIZE)) * CHUNK_STORE_SIZE;
    ret.z = ((int)floor(block.z / (float)CHUNK_STORE_SIZE)) * CHUNK_STORE_SIZE;
    return ret;
}

bool isTransparent(uint8_t blockId)
{
    switch(blockId)
    {
        case 0:
            return true;
        case 9:
            return true;
        default:
            return false;
    };
}

void Chunk::regenerateMesh()
{
    Mesh *mesh, *transMesh;
    if(FLAGSET(this->flags, Flags::HasMesh))
        mesh = this->mesh;
    else
    {
        mesh = new Mesh();
        this->mesh = mesh;
    }
    if(FLAGSET(this->flags, Flags::HasWaterMesh))
    {
        transMesh = this->waterMesh;
    }
    else
    {
        waterMesh = new Mesh();
        this->waterMesh = waterMesh;
    }

    Vec3 vertices[65536];
    Vec3 texCoords[65536];
    uint16_t indices[65536];
    int vertCount = 0;
    int indexCount = 0;

    Vec3 transVertices[65536];
    Vec3 transTexCoords[65536];
    uint16_t transIndices[65536];
    int transVertCount = 0;
    int transIndexCount = 0;

    int size = this->size;
    for(int i = 0; i < size; i++)
    for(int j = 0; j < size; j++)
    for(int k = 0; k < size; k++)
    {
        Vec3 localOffset(i, j, k);
        IVec3 blockV((int)offset.x+i, (int)offset.y+j, (int)offset.z+k);
        int blockId = world->getBlockId(blockV);
        bool blockEmpty = blockId == 0;
        bool blockTransparent = isTransparent(blockId);
        
        Block *block = blockStore->getBlock(blockId);

        for(int dir = 0; dir < 3; dir++)
        {
            uint8_t dirBlockId = world->getBlockId(IVec3(blockV.x+(int)directions[dir].x, blockV.y+(int)directions[dir].y, blockV.z+(int)directions[dir].z));
            Block *dirBlock = blockStore->getBlock(dirBlockId);
            bool dirEmpty = dirBlockId == 0;
            bool dirTransparent = isTransparent(dirBlockId);
            if(blockEmpty != dirEmpty || blockTransparent != dirTransparent)
            {
                Block* faceBlock;
                if(blockEmpty != dirEmpty)
                    faceBlock = blockEmpty ? dirBlock : block;
                else if(blockTransparent != dirTransparent)
                    faceBlock = blockTransparent ? dirBlock : block;

                uint8_t faceBlockId = faceBlock == block ? blockId : dirBlockId;
                bool faceTransparent = (isTransparent(faceBlockId) && faceBlockId != 0);

                for(int vert = 0; vert < 4; vert++)
                {
                    if(!faceTransparent)
                    {
                        vertices[vertCount] = localOffset + sideQuads[dir][vert] + directions[dir];
                        texCoords[vertCount] = sideTexCoords[dir][vert];
                        texCoords[vertCount].z = faceBlock->faceTextureLayers[dir*2 + (faceBlock == block ? 0 : 1)];
                        vertCount++;
                    }
                    else
                    {
                        transVertices[transVertCount] = localOffset + sideQuads[dir][vert] + directions[dir];
                        transTexCoords[transVertCount] = sideTexCoords[dir][vert];
                        transTexCoords[transVertCount].z = faceBlock->faceTextureLayers[dir*2 + (faceBlock == block ? 0 : 1)];
                        transVertCount++;
                    }
                }
                for(int idx = 0; idx < 6; idx++)
                {
                    int vertIdx;
                    vertIdx = faceBlock == block ? idx : 5 - idx;
                    if(!faceTransparent)
                    {
                        indices[indexCount] = vertCount - 4 + quadIndices[vertIdx];
                        indexCount++;
                    }
                    else
                    {
                        transIndices[transIndexCount] = transVertCount - 4 + quadIndices[vertIdx];
                        transIndexCount++;
                    }
                }
            }
        }
    }

    mesh->copyVertices(vertices, vertCount);
    mesh->copyTexCoords(texCoords, vertCount);
    mesh->copyIndices(indices, indexCount);
    mesh->calculateNormals();

    waterMesh->copyVertices(transVertices, transVertCount);
    waterMesh->copyTexCoords(transTexCoords, transVertCount);
    waterMesh->copyIndices(transIndices, transIndexCount);
    waterMesh->calculateNormals();

    this->flags |= Flags::HasMesh;
    this->flags |= Flags::HasWaterMesh;
    this->flags &= ~Flags::Dirty;
}

namespace
{
    Vec3 directions [3] =
    {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };

    Vec3 sideQuads[3][4] =
    {
        { // right/left
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 0.0f}
        },
        { // top/bottom
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f}
        },
        { // front/back
            {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f}
        }
    };

    Vec3 sideTexCoords [3][4] =
    {
        { // right/left
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}
        },
        { // top/bottom
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}
        },
        { // front/back
            {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f}
        }
    };

    uint16_t quadIndices[6] = 
    {
        0, 1, 2, 0, 2, 3
    };
}

