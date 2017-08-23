#include "game.h"
#include "Renderer/renderer.h"
#include "Renderer/mesh.h"
#include "Renderer/texture.h"
#include "GameWorld/world.h"
#include "GameWorld/chunk.h"
#include "GameWorld/blockstore.h"

#include <cstdio>

#define        STBI_NO_JPEG
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

void Game::setMode(uint32_t mode)
{
    this->mode = mode;

    switch(mode)
    {
        case Mode::Mode_FreeView:
            activeCam = &freeCam;
            world->setCamera(&freeCam);
            break;
        case Mode::Mode_Player:
            activeCam = &player.camera;
            world->setCamera(&player.camera);
            player.world = world;
            break;
    }
}

void Game::simulate(Renderer *renderer, float dt)
{
    if(!initialized)
    {
        initialized = true;
        mesh.copyVertices(quadVerts, 4);
        mesh.copyTexCoords(quadTexCoords, 4);
        mesh.copyIndices(quadIndices, 12);

        this->freeCam.transform.position = Vec3(0.0f, 0.0f, 3.0f);
        // NOTE: also hardcoded in frag shader currently
        this->freeCam.zNear = 0.1f;
        this->freeCam.zFar = 100.0f;

        player.transform.position = Vec3(0.0f, 10.0f, 0.0f);

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
            {
                memcpy(&texData[x*16*4], &data[(i*16+x)*width*4 + (j*16)*4], 64);
            }
            atlas->copyLayer(texData, 16, 16, 4, i*16+j);
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

        world = new World(renderer, &bs, &freeCam);
        setMode(Mode::Mode_FreeView);
    }

    // get the global mouse position (relative to the desktop)
    sf::Vector2i globalPosition = sf::Mouse::getPosition();
    this->mouseDelta = Vec2(globalPosition.x - mousePosLast.x, globalPosition.y - mousePosLast.y);
    sf::Mouse::setPosition(sf::Vector2i(renderer->width / 2.0f, renderer->height / 2.0f), *window);
    globalPosition = sf::Mouse::getPosition();
    mousePosLast = Vec2((float)globalPosition.x, (float)globalPosition.y);

    // TODO: move somewhere else
    if(mode == Mode::Mode_FreeView)
    {
        // TODO: input system
        const float moveSpeed = 30.0f;
        const float rotSpeed = 0.4f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            this->freeCam.transform.position += dt*moveSpeed*freeCam.transform.forward();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            this->freeCam.transform.position -= dt*moveSpeed*freeCam.transform.forward();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            this->freeCam.transform.position -= dt*moveSpeed*freeCam.transform.right();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            this->freeCam.transform.position += dt*moveSpeed*freeCam.transform.right();
        }

        camRot += this->mouseDelta;
        //camRot.x = fmod(camRot.x, 360.0f);
        //camRot.y = fmod(camRot.y, 360.0f);

        Quaternion rotX = Quaternion::AngleAxis(rotSpeed * camRot.x, Vec3(0.0f, 1.0f, 0.0f));
        Quaternion rotY = Quaternion::AngleAxis(rotSpeed * camRot.y, Vec3(1.0f, 0.0f, 0.0f));
        this->freeCam.transform.rotation = rotX * rotY;
    }
    else
    {
        player.update(dt, mouseDelta);
    }

    rot += 1.0f;

    world->update();
}

void Game::keyPress(sf::Keyboard::Key key)
{
    if(key == sf::Keyboard::F1)
        setMode(Mode::Mode_Player);
    else if(key == sf::Keyboard::F2)
        setMode(Mode::Mode_FreeView);
}

void Game::mouseClick(int button)
{
    RaycastHit hit;
    if(world->lineCast(hit, activeCam->transform.position, activeCam->transform.position + 10.0f*activeCam->transform.forward()))
    {
        if (button == 0)
        {
            world->setBlockId(hit.block, 0);
        }
        else if(button == 1)
        {
            world->setBlockId(hit.block + hit.faceDirection, 1);
        }
    }

}

Vec3 tempVec;

void Game::updateAndRender(Renderer *renderer, float dt)
{    
    simulate(renderer, dt);

    this->activeCam->targetWidth = renderer->width;
    this->activeCam->targetHeight = renderer->height;

    Mat4 world_to_clip = activeCam->getViewProjectionMatrix();

    Quaternion rotQ = Quaternion::AngleAxis(rot, Vec3(0.0f, 1.0f, 0.0f));

    RaycastHit hit;
    if(world->lineCast(hit, activeCam->transform.position, activeCam->transform.position + 10.0f*activeCam->transform.forward()))
    {
        tempVec = hit.point;
    }

    Mat4 model_to_world = Mat4::TRS(tempVec, rotQ, Vec3(1.0f, 1.0f, 1.0f));
    Mat4 model_to_clip = world_to_clip * model_to_world;

    renderer->clearScreen(Vec4(0.8f, 0.8f, 0.8f, 1.0f));
    //renderer->renderMesh(&mesh, renderer->defaultMaterial, &model_to_world, &world_to_clip);

    world->render();
}
