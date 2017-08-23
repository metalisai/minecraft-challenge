#include "worldgenerator.h"

#include "world.h"
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

void WorldGenerator::fillChunk(IVec3 offset)
{
    const int chunkSize = 16;
    for(int i = 0; i < chunkSize; i++)
    for(int j = 0; j < chunkSize; j++)
    for(int k = 0; k < chunkSize; k++)
    {
        IVec3 block(offset.x + i, offset.y + j, offset.z + k);
        world->setBlockId(block, calcBlockId(block));
    }
}

