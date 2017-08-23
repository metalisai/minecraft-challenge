#include "renderevents.h"
#include "renderer.h"
#include <assert.h>

void RenderEvents::meshDeleted(class Mesh *mesh)
{
    Renderer::meshUnload(mesh);
}

void RenderEvents::textureDeleted(class Texture *texture)
{
    // TODO: implement
    assert(false);
}

void RenderEvents::textureArrayDeleted(class TextureArray *textureArray)
{
    // TODO: implement
    assert(false);
}
