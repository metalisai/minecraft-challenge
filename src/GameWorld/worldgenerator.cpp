#include "worldgenerator.h"

#include "../Renderer/mesh.h"

namespace
{
    extern Vec3 sideOffsets[6];
    extern Vec3 sideQuads[3][4];
    extern Vec3 directions[3];
    extern Vec3 sideTexCoords[3][4];
    extern uint16_t quadIndices[6]; 
}

WorldGenerator::WorldGenerator(BlockStore *blockStore)
{
    this->blockStore = blockStore;
}

int getBlockId(Vec3 position)
{
    if(position.length() >= 10.0f)
        return 2;
    else
        return 0;
}


Mesh* WorldGenerator::generateChunk(Vec3 offset, int size)
{
    Vec3 vertices[65536];
    Vec3 texCoords[65536];
    uint16_t indices[65536];
    int vertCount = 0;
    int indexCount = 0;

    for(int i = 0; i < size; i++)
    for(int j = 0; j < size; j++)
    for(int k = 0; k < size; k++)
    {
        Vec3 localOffset(i, j, k);
        localOffset += offset;
        int blockId = getBlockId(localOffset);
        bool blockEmpty = blockId == 0;
        
        Block *block = blockStore->getBlock(blockId);

        for(int dir = 0; dir < 3; dir++)
        {
            int dirBlockId = getBlockId(localOffset + directions[dir]);
            Block *dirBlock = blockStore->getBlock(dirBlockId);
            bool dirEmpty = dirBlockId == 0;
            if(blockEmpty != dirEmpty)
            {
                int firstIndex = vertCount;
                for(int vert = 0; vert < 4; vert++)
                {
                    vertices[vertCount] = localOffset + sideQuads[dir][vert] + directions[dir];
                    texCoords[vertCount] = sideTexCoords[dir][vert];
                    texCoords[vertCount].z = blockEmpty ? dirBlock->faceTextureLayers[dir*2 + 1] 
                        : block->faceTextureLayers[dir*2];
                    vertCount++;
                }
                for(int idx = 0; idx < 6; idx++)
                {
                    int vertIdx = blockEmpty ? 5 - idx : idx;
                    indices[indexCount] = firstIndex + quadIndices[vertIdx];
                    indexCount++;
                }
            }
        }
    }

    Mesh *ret = new Mesh();
    ret->copyVertices(vertices, vertCount);
    ret->copyTexCoords(texCoords, vertCount);
    ret->copyIndices(indices, indexCount);
    return ret;
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

