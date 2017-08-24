#include "worldgenerator.h"

#include "world.h"
#include "chunk.h"
#include <assert.h>

struct Biome
{
    uint8_t layer1BlockId;
    uint8_t layer2BlockId;
    uint8_t layer3BlockId;

    float heightOffset;
    float heightMultiplier;
    bool hasWater;
};

Biome biomes[] = 
{
    {2, 3, 1, -10.0f, 7.0f, true},
    {2, 3, 1, 0.0f, 5.0f, true},
    {12, 12, 1, 2.0f, 3.0f, false},
    {2, 3, 1, 5.0f, 10.0f, true},
    {2, 3, 1, -10.0f, 7.0f, true}
};

WorldGenerator::WorldGenerator(World *world)
{
    this->world = world;
}

inline float lerp(float a, float b, float t)
{
    return a + (b-a)*t;
}

inline float clamp01(float a)
{
    return a > 1.0f ? 1.0f : a < 0.0f ? 0.0f : a;
}

// TODO: I don't think this is how it should be done!

static int calcBlockId(IVec3 position)
{
    float biomef = perlin2D(Vec3(position.x+400.654321f, position.z+200.654321f, 0.123456f), 0.00531415f)*200.0f + 200.0f;
    
    int biomei = (int)biomef/100.0f;
    assert(biomei >= 0 && biomei < 4);

    float fmodbi = fmodf(biomef, 100.0f);
    float t;
    int firstIndex;
    int secondIndex;
    if(fmodbi < 50.0f)
    {
        t = clamp01((fmodbi) / 10.0f);
        firstIndex = biomei+1;
        secondIndex = biomei;
    }
    else
    {
        t = clamp01((10.0f - (100.0f - (fmodbi)))/ 10.0f);
        firstIndex = biomei;
        secondIndex = biomei+1;
    }
    float heightOffset = lerp(biomes[firstIndex].heightOffset, biomes[secondIndex].heightOffset, t);
    float heightMultiplier = lerp(biomes[firstIndex].heightMultiplier, biomes[secondIndex].heightMultiplier, t);

    Biome *biome = &biomes[biomei];
    float height = perlin2D(Vec3(position.x+0.123456f, position.z+0.123456f, 0.123456f), 0.05641564535f) * heightMultiplier + heightOffset;

    if(position.y <= height)
    {
        if(position.y <= height - 3.0f)
            return biome->layer3BlockId;
        else if(position.y + 1.0f <= height || position.y < -1.0f)
            return biome->layer2BlockId;
        else
            return biome->layer1BlockId;
    }
    else if(position.y < 0.0f && biome->hasWater) // water
        return 9;
    else
        return 0;
}

void WorldGenerator::addTree(IVec3 block, int height, int leafsize, int seed)
{
    // trunk
    for(int i = 1; i < height+1; i++)
    {
        world->setBlockId(IVec3(block.x, block.y+i, block.z), 17);
    }

    int ysize = leafsize - 2;

    for(int i = -leafsize; i <= leafsize; i++)
    for(int j = -leafsize; j <= ysize; j++)
    for(int k = -leafsize; k <= leafsize; k++)
    {
        if(rand() % 5 != 0 && (i != 0 || k != 0))
        {
            world->setBlockId(IVec3(block.x+i, block.y+j+height+1, block.z+k), 18);
        }
    } 
}

void WorldGenerator::fillChunk(IVec3 offset)
{
    IVec3 trees[100];
    int treeLeafSizes[100];
    int treeHeights[100];

    uint32_t treeCount = 0;

    uint32_t chunkSeed;
    uint8_t xs = offset.x / 16;
    uint8_t ys = offset.y / 16;
    uint8_t zs = offset.z / 16;
    chunkSeed = xs | ((ys & 1023) << 10) | ((zs & 1023) << 20);

    srand(chunkSeed);

    const int chunkSize = 16;
    for(int i = 0; i < chunkSize; i++)
    for(int j = 0; j < chunkSize; j++)
    for(int k = 0; k < chunkSize; k++)
    {
        IVec3 block(offset.x + i, offset.y + j, offset.z + k);
        uint8_t blockId = calcBlockId(block);
        world->setBlockId(block, blockId);
        if(blockId == 2 && rand() % 500 == 0) // grass
        {
            int index = treeCount++;
            trees[index] = block;
            treeLeafSizes[index] = 2+(rand()%2);
            treeHeights[index] = 5+(rand()%4);
        }
    }

    for(int i = 0; i < treeCount; i++)
    {
        // don't allow trees at chunk borders
        // TODO: remove this limitation?
        IVec3 loffset = Chunk::getLocalOffset(trees[i]);
        if(loffset.x > 2 && loffset.x < 14 && loffset.z > 2 && loffset.z < 14)
            addTree(trees[i], treeHeights[i], treeLeafSizes[i], chunkSeed);
    }
}

