#include "mesh.h"

#include <cstring>
#include <cstdio>
#include "macros.h"

Mesh::Mesh()
{
    this->vertices = nullptr;
    this->indices = nullptr;
    this->numVertices = 0;
    this->numIndices = 0;
    this->flags = 0;
}

Mesh::~Mesh()
{
    if(FLAGSET(this->flags, Mesh::Flags::HasVertices))
    {
        delete[] this->vertices;
        this->vertices = nullptr;
    }
    if(FLAGSET(this->flags, Mesh::Flags::HasIndices))
    {
        delete[] this->indices;
        this->indices = nullptr;
    }
    if(FLAGSET(this->flags, Mesh::Flags::HasTexCoords))
    {
        delete[] this->texCoords;
        this->texCoords = nullptr;
    }
    this->flags = 0;
}

void Mesh::copyVertices(const Vec3 *vertices, uint32_t count)
{
    if(FLAGSET(this->flags, Flags::HasVertices) && count != this->numVertices)
    {
        delete[] this->vertices;

        if(FLAGSET(this->flags, Flags::HasTexCoords))
        {
            delete[] this->texCoords;
            this->flags &= ~Mesh::Flags::HasTexCoords;
        }
    }
    if(!FLAGSET(this->flags, Flags::HasVertices) || count != this->numVertices)
        this->vertices = new Vec3[count];
    memcpy(this->vertices, vertices, sizeof(Vec3)*count);
    this->flags |= Flags::HasVertices;
    this->flags |= Flags::Dirty;
    this->numVertices = count;
}

void Mesh::copyIndices(const uint16_t *indices, uint32_t count)
{
    if(FLAGSET(this->flags, Flags::HasVertices))
    {
        for(int i = 0; i < count; i++)
        {
            if(indices[i] >= this->numVertices)
            {
                fprintf(stderr, "Mesh::CopyIndices: One or more indices refering to out of bounds vertices!\n");
                break;
            }
        }
    }
    if(FLAGSET(this->flags, Flags::HasIndices) && count != this->numIndices)
    {
        delete[] this->indices;
    }
    if(!FLAGSET(this->flags, Flags::HasIndices) || count != this->numIndices)
        this->indices = new uint16_t[count];
    memcpy(this->indices, indices, sizeof(uint16_t)*count);
    this->flags |= Flags::HasIndices;
    this->flags |= Flags::Dirty;
    this->numIndices = count;
}

void Mesh::copyTexCoords(const Vec3 *coords, uint32_t count)
{
    if(FLAGSET(this->flags, Flags::HasTexCoords) && count != this->numVertices)
    {
        fprintf(stderr, "Number of texture coordinates didn't match number of vertices!\n");
        return;
    }
    if(!FLAGSET(this->flags, Flags::HasTexCoords))
    {
        this->texCoords = new Vec3[count];
    }
    memcpy(this->texCoords, coords, sizeof(Vec3)*count);
    this->flags |= Flags::HasTexCoords;
    this->flags |= Flags::Dirty;
}

