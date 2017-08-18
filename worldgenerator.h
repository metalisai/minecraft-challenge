#include "Maths/maths.h"

class WorldGenerator
{
public:
    WorldGenerator();
    class Mesh* generateChunk(Vec3 offset, int size);
};

#include <map>

// 0 - right face
// 1 - left face
// 2 - top face
// 3 - bottom face
// 4 - front face
// 5 - back face

class Block
{
public:
    const char *name;
    int faceTextures[6];
    int faceTextureLayers[6];
};

class BlockStore
{
public:
   Block *blocks[256];
};

