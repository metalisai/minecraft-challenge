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
        int x,y,n;
        unsigned char *data = stbi_load("Resources/grass.png", &x, &y, &n, 0);
        grass = new Texture(x, y, n);
        grass->copyData(data, x, y, n);
        
        atlas = new TextureArray(16, 16, n, 255);
        //atlas->copyLayer(data, x, y, n, 0);

        stbi_image_free(data);
        data = stbi_load("Resources/dirt.png", &x, &y, &n, 0);
        atlas->copyLayer(data, x, y, n, 0);
        stbi_image_free(data);
        data = stbi_load("Resources/grass-side.png", &x, &y, &n, 0);
        atlas->copyLayer(data, x, y, n, 1);
        stbi_image_free(data);
        data = stbi_load("Resources/grass-top.png", &x, &y, &n, 0);
        atlas->copyLayer(data, x, y, n, 2);
        stbi_image_free(data);
        Renderer::defaultMaterial->addTexture("tex", grass);
        Renderer::defaultMaterial->addTextureArray("texArr", atlas);

        Block dirt, grass;
        dirt.name = "Dirt";
        dirt.faceTextureLayers[0] = 0;
        dirt.faceTextureLayers[1] = 0;
        dirt.faceTextureLayers[2] = 0;
        dirt.faceTextureLayers[3] = 0;
        dirt.faceTextureLayers[4] = 0;
        dirt.faceTextureLayers[5] = 0;

        grass.name = "Grass";
        grass.faceTextureLayers[0] = 1;
        grass.faceTextureLayers[1] = 1;
        grass.faceTextureLayers[2] = 2;
        grass.faceTextureLayers[3] = 0;
        grass.faceTextureLayers[4] = 1;
        grass.faceTextureLayers[5] = 1;

        bs.createBlock(1, dirt);
        bs.createBlock(2, grass);

        world = new World(renderer, &bs, &mainCam);

/*        Chunk *chunk;
        chunk = new Chunk(&bs, world, Vec3(-10.0f, -10.0f, -10.0f), 20);
        chunk->regenerateMesh();
        chunkMesh = chunk->mesh;*/
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
