#pragma once 

#include "../Maths/maths.h"

class WorldGenerator
{
public:
    WorldGenerator(class World *world);
    void fillChunk(IVec3 offset);

    class World *world;
};
