#include "mesh.h"

#include <cstring>
#include <cstdio>
#include <assert.h>
#include "../macros.h"
#include "renderevents.h"

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
    RenderEvents::meshDeleted(this);

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
    if(FLAGSET(this->flags, Mesh::Flags::HasNormals))
    {
        delete[] this->normals;
        this->normals = nullptr;
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
        if(FLAGSET(this->flags, Flags::HasNormals))
        {
            delete[] this->normals;
            this->flags &= ~Mesh::Flags::HasNormals;
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


void Mesh::calculateNormals()
{
    assert(FLAGSET(this->flags, Mesh::Flags::HasVertices));
    assert(FLAGSET(this->flags, Mesh::Flags::HasIndices));

    Vec3 *normals;
    if(FLAGSET(this->flags, Flags::HasNormals))
        normals = this->normals;
    else
        normals = new Vec3[this->numVertices];
    
    for(int i = 0; i < this->numIndices; i+=3)
    {
        // FLAT normals
        Vec3 verts[3];
        for(int v = 0; v < 3; v++)
        {
            assert(indices[i+v] >= 0 && indices[i+v] < numVertices);
            verts[v] = vertices[indices[i+v]];
        }
        Vec3 edge1 = verts[1] - verts[0];
        Vec3 edge2 = verts[2] - verts[0];
        Vec3 normal = Vec3::Cross(edge2, edge1).normalized();
        for(int v = 0; v < 3; v++)
            normals[indices[i+v]] = normal;
    }
    this->flags |= Mesh::Flags::HasNormals;
    this->normals = normals;
}

