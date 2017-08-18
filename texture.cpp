#include "texture.h"

#include "macros.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <assert.h>

Texture::Texture(int width, int height, int components)
{
    this->flags = 0;
    this->data = nullptr;
    this->renderer_handle = 0;

    this->width = width;
    this->height = height;
    this->components = components;
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
    if(FLAGSET(this->flags, Texture::Flags::IsTextureArray))
    {
        fprintf(stderr, "Not valid operation for array texture\n");
        return;
    }

    size_t size = width * height * components;
    if(!FLAGSET(this->flags, Texture::Flags::HasData))
    {
        this->data = malloc(size);
        this->flags |= Texture::Flags::HasData;
    }
    assert(components == this->components);
    assert(width == this->width && height == this->height);
    memcpy(this->data, src, size);
}

TextureArray::TextureArray(int width, int height, int components, int layers) : Texture(width, height, components)
{
    this->layers = layers;
}

void TextureArray::copyLayer(void *src, int width, int height, int components, int layer)
{
    if(!FLAGSET(this->flags, Texture::Flags::HasData))
    {
        size_t size = width * height * components * layers;
        this->data = malloc(size);
        this->flags |= Texture::Flags::HasData;
    }
    assert(layer < this->layers);
    assert(this->components = components);
    // TODO: resize?
    assert(this->width == width && this->height == height);
    size_t offset = width * height * components * layer;
    uint8_t *layerData = (uint8_t*)this->data;
    layerData += offset;
    memcpy(layerData, src, width * height * components);
}

