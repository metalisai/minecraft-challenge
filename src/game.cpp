#include "game.h"
#include "Renderer/renderer.h"
#include "Renderer/mesh.h"
#include "Renderer/texture.h"
#include "GameWorld/world.h"
#include "GameWorld/chunk.h"
#include "GameWorld/blockstore.h"

#include <cstdio>
#include <SFML/Window.hpp>

#define STBI_NO_JPEG
//#define        STBI_NO_PNG
#define        STBI_NO_BMP
#define        STBI_NO_PSD
#define        STBI_NO_TGA
#define        STBI_NO_GIF
#define        STBI_NO_HDR
#define        STBI_NO_PIC
#define        STBI_NO_PNM 
#define STB_IMAGE_IMPLEMENTATION
#include "Libs/stb_image.h"

static Vec3 quadVerts[] =
{
    {-0.5f, -0.5f, 0.0f},
    { 0.5f, -0.5f, 0.0f},
    { 0.5f,  0.5f, 0.0f},
    {-0.5f,  0.5f, 0.0f}
};

static Vec3 quadTexCoords[] = 
{
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};

static uint16_t quadIndices[] = 
{
    0, 1, 2, 0, 2, 3, 2, 1, 0, 3, 2, 0
};

Mesh mesh;
//Mesh *chunkMesh;
Texture *grass;
TextureArray *atlas;

//Block grassBlock;
BlockStore bs;

World* world;

float rot;

void Game::simulate(Renderer *renderer, float dt)
{
    if(!initialized)
    {
        initialized = true;
        mesh.copyVertices(quadVerts, 4);
        mesh.copyTexCoords(quadTexCoords, 4);
        mesh.copyIndices(quadIndices, 12);

        this->mainCam.transform.position = Vec3(0.0f, 0.0f, 3.0f);
        sf::Vector2i globalPosition = sf::Mouse::getPosition();
        mousePosLast = Vec2((float)globalPosition.x, (float)globalPosition.y);
        camRot = Vec2(0.0f, 0.0f);

        // texture
        
        atlas = new TextureArray(16, 16, 4, 256);

        int width, height, comps;
        unsigned char *data;

        data = stbi_load("Resources/mcatlas.png", &width, &height, &comps, 0);
        uint8_t texData[16*16*4];
        for(int i = 0; i < 16; i++)
        for(int j = 0; j < 16; j++)
        {
            for(int x = 0; x < 16; x++)
            for(int y = 0; y < 16; y++)
            {
                memcpy(&texData[x*16*4 + y*4], &data[(i*16+x)*width*4 + (j*16+y)*4], 4);
                atlas->copyLayer(texData, 16, 16, 4, i*16+j);
            }
        }
        stbi_image_free(data);

        Renderer::defaultMaterial->addTextureArray("texArr", atlas);

        bs.createBlock(1, {"Stone", {1, 1, 1, 1, 1, 1}});
        bs.createBlock(2, {"Grass", {3, 3, 0, 2, 3, 3}});
        bs.createBlock(3, {"Dirt", {2, 2, 2, 2, 2, 2}});
        bs.createBlock(4, {"Cobblestone", {16, 16, 16, 16, 16, 16}});
        bs.createBlock(9, {"Water", {205, 205, 205, 205, 205, 205}});
        bs.createBlock(12, {"Sand", {176, 176, 176, 176, 176, 176}});
        bs.createBlock(13, {"Gravel", {19, 19, 19, 19, 19, 19}});
        bs.createBlock(17, {"Wood", {20, 20, 21, 21, 20, 20}});
        bs.createBlock(18, {"Leaves", {52, 52, 52, 52, 52, 52}});
        bs.createBlock(45, {"Brick", {7, 7, 7, 7, 7, 7}});

        world = new World(renderer, &bs, &mainCam);
    }

    // TODO: input system
    const float moveSpeed = 3.0f;
    const float rotSpeed = 0.4f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->mainCam.transform.position += dt*moveSpeed*mainCam.transform.forward();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->mainCam.transform.position -= dt*moveSpeed*mainCam.transform.forward();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->mainCam.transform.position -= dt*moveSpeed*mainCam.transform.right();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->mainCam.transform.position += dt*moveSpeed*mainCam.transform.right();
    }

    // get the global mouse position (relative to the desktop)
    sf::Vector2i globalPosition = sf::Mouse::getPosition();
    this->mouseDelta = Vec2(globalPosition.x - mousePosLast.x, globalPosition.y - mousePosLast.y);
    mousePosLast = Vec2((float)globalPosition.x, (float)globalPosition.y);
    camRot += this->mouseDelta;
    //camRot.x = fmod(camRot.x, 360.0f);
    //camRot.y = fmod(camRot.y, 360.0f);

    Quaternion rotX = Quaternion::AngleAxis(rotSpeed * camRot.x, Vec3(0.0f, 1.0f, 0.0f));
    Quaternion rotY = Quaternion::AngleAxis(rotSpeed * camRot.y, Vec3(1.0f, 0.0f, 0.0f));
    this->mainCam.transform.rotation = rotX * rotY;

    rot += 1.0f;

    world->update();
}

Vec3 tempVec;

void Game::updateAndRender(Renderer *renderer, float dt)
{    
    simulate(renderer, dt);

    this->mainCam.targetWidth = renderer->width;
    this->mainCam.targetHeight = renderer->height;

    Mat4 world_to_clip = mainCam.getViewProjectionMatrix();

    Quaternion rotQ = Quaternion::AngleAxis(rot, Vec3(0.0f, 1.0f, 0.0f));

    RaycastHit hit;
    if(world->lineCast(hit, mainCam.transform.position, mainCam.transform.position + 10.0f*mainCam.transform.forward()))
    {
        tempVec = hit.point;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            world->setBlockId(hit.block, 0);
        }
    }

    Mat4 model_to_world = Mat4::TRS(tempVec, rotQ, Vec3(1.0f, 1.0f, 1.0f));
    //Mat4 model_to_world = Mat4::Identity();

    Mat4 model_to_clip = world_to_clip * model_to_world;

    //Mat4 chunkmodel_to_clip = world_to_clip;
    //Mat4 model_to_clip = Mat4::Identity();

    renderer->clearScreen(Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    renderer->renderMesh(&mesh, renderer->defaultMaterial, &model_to_clip);
    //renderer->renderMesh(chunkMesh, renderer->defaultMaterial, &world_to_clip);

    world->render();
}
