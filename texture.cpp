#include "texture.h"

#include "macros.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

Texture::Texture()
{
    this->flags = 0;
    this->data = nullptr;
    this->renderer_handle = 0;
}

Texture::Texture(int width, int height) : Texture()
{
    this->width = width;
    this->height = height;
}

Texture::~Texture()
{
    if(FLAGSET(this->flags, Texture::Flags::HasData))
    {
        free(this->data);
        this->data = nullptr;
        this->flags = 0;
    }
}

void Texture::copyData(void *src, int width, int height, int components)
{
    size_t size = width * height * components;
    this->data = malloc(size);
    memcpy(this->data, src, size);
    this->width = width;
    this->height = height;
    this->components = components;
    this->flags |= Texture::Flags::HasData;
}

