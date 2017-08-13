#include "game.h"
#include "renderer.h"
#include "mesh.h"

Vec3 quadVerts[] =
{
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};

uint16_t quadIndices[] = 
{
    0, 1, 2, 0, 2, 3
};

Mesh mesh;

void Game::simulate(float dt)
{
    if(!initialized)
    {
        initialized = true;
        mesh.copyVertices(quadVerts, 4);
        mesh.copyIndices(quadIndices, 6);
    }
}

void Game::render(Renderer *renderer)
{
    Mat4 mat;
    renderer->clearScreen(Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    renderer->renderMesh(&mesh, renderer->defaultMaterial, &mat);
}
