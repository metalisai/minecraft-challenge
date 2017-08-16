#include "game.h"
#include "renderer.h"
#include "mesh.h"
#include "camera.h"

#include <cstdio>

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
Camera mainCam;
float rot;

void Game::simulate(float dt)
{
    if(!initialized)
    {
        initialized = true;
        mesh.copyVertices(quadVerts, 4);
        mesh.copyIndices(quadIndices, 6);
    }

    mainCam.transform.position = Vec3(0.0f, 0.0f, 3.0f);
    rot += 1.0f;
}

void Game::render(Renderer *renderer)
{
    mainCam.targetWidth = renderer->width;
    mainCam.targetHeight = renderer->height;

    Mat4 world_to_clip = mainCam.getViewProjectionMatrix();

    Quaternion rotQ = Quaternion::AngleAxis(rot, Vec3(0.0f, 1.0f, 0.0f));
    Mat4 model_to_world = Mat4::Rotation(rotQ);

    Mat4 model_to_clip = world_to_clip * model_to_world;
    //Mat4 model_to_clip = Mat4::Identity();

    renderer->clearScreen(Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    renderer->renderMesh(&mesh, renderer->defaultMaterial, &model_to_clip);
}
