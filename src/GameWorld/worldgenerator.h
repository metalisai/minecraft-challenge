#pragma once 

#include "../Maths/maths.h"

class WorldGenerator
{
public:
    WorldGenerator(class World *world);
    void fillChunk(IVec3 offset);
    void addTree(IVec3 block, int height, int leafsize, int seed);

    class World *world;
};
