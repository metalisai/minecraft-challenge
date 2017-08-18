#include "Maths/maths.h"

class WorldGenerator
{
public:
    WorldGenerator();
    class Mesh* generateChunk(Vec3 offset, int size);
};
