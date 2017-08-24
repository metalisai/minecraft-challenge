#include "game.h"
#include "Renderer/renderer.h"
#include "Renderer/texture.h"
#include "GameWorld/world.h"
#include "GameWorld/chunk.h"
#include "Player/gui.h"

#include <cstdio>

#define        STBI_NO_JPEG
#define        STBI_NO_BMP
#define        STBI_NO_PSD
#define        STBI_NO_TGA
#define        STBI_NO_GIF
#define        STBI_NO_HDR
#define        STBI_NO_PIC
#define        STBI_NO_PNM 
#define STB_IMAGE_IMPLEMENTATION
#include "Libs/stb_image.h"


Game::~Game()
{
    if(this->gui != nullptr)
    {
        delete this->gui;
    }
}

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

        this->freeCam.transform.position = Vec3(0.0f, 0.0f, 3.0f);
        // NOTE: also hardcoded in frag shader currently
        this->freeCam.zNear = 0.1f;
        this->freeCam.zFar = 100.0f;

        player.transform.position = Vec3(0.0f, 10.0f, 0.0f);

        sf::Vector2i globalPosition = sf::Mouse::getPosition();
        mousePosLast = Vec2((float)globalPosition.x, (float)globalPosition.y);
        camRot = Vec2(0.0f, 0.0f);

        // texture
        
        atlas = new TextureArray(16, 16, 4, 257);

        int width, height, comps;
        unsigned char *data;

        data = stbi_load("Resources/mcatlas.png", &width, &height, &comps, 0);
        uint8_t texData[16*16*4];
        memset(texData, 0xFF, 16*16*4);
        atlas->copyLayer(texData, 16, 16, 4, 0); // first layer white
        for(int i = 0; i < 16; i++)
        for(int j = 0; j < 16; j++)
        {
            for(int x = 0; x < 16; x++)
            {
                memcpy(&texData[x*16*4], &data[(i*16+x)*width*4 + (j*16)*4], 64);
            }
            atlas->copyLayer(texData, 16, 16, 4, i*16+j+1);
        }
        stbi_image_free(data);

        Renderer::defaultMaterial->addTextureArray("texArr", atlas);
        Renderer::solidMaterial->addTextureArray("texArr", atlas);

        blockStore.createBlock(1, {"Stone", {2, 2, 2, 2, 2, 2}});
        blockStore.createBlock(2, {"Grass", {4, 4, 1, 3, 4, 4}});
        blockStore.createBlock(3, {"Dirt", {3, 3, 3, 3, 3, 3}});
        blockStore.createBlock(4, {"Cobblestone", {17, 17, 17, 17, 17, 17}});
        blockStore.createBlock(9, {"Water", {206, 206, 206, 206, 206, 206}});
        blockStore.createBlock(12, {"Sand", {177, 177, 177, 177, 177, 177}});
        blockStore.createBlock(13, {"Gravel", {20, 20, 20, 20, 20, 20}});
        blockStore.createBlock(17, {"Wood", {21, 21, 22, 22, 21, 21}});
        blockStore.createBlock(18, {"Leaves", {53, 53, 53, 53, 53, 53}});
        blockStore.createBlock(45, {"Brick", {8, 8, 8, 8, 8, 8}});

        world = new World(renderer, &blockStore, &player.camera);
        setMode(Mode::Mode_Player);

        this->gui = new Gui(renderer, &this->player, &blockStore);
    }

    // get the global mouse position (relative to the desktop)
    sf::Vector2i globalPosition = sf::Mouse::getPosition();
    this->mouseDelta = Vec2(globalPosition.x - mousePosLast.x, globalPosition.y - mousePosLast.y);
    
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        sf::Mouse::setPosition(sf::Vector2i(renderer->width / 2.0f, renderer->height / 2.0f), *window);
        globalPosition = sf::Mouse::getPosition();
    }
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

        Quaternion rotX = Quaternion::AngleAxis(rotSpeed * camRot.x, Vec3(0.0f, 1.0f, 0.0f));
        Quaternion rotY = Quaternion::AngleAxis(rotSpeed * camRot.y, Vec3(1.0f, 0.0f, 0.0f));
        this->freeCam.transform.rotation = rotX * rotY;
    }
    else
    {
        player.update(dt, mouseDelta);
    }

    world->update();
}

void Game::keyPress(sf::Keyboard::Key key)
{
    if(key == sf::Keyboard::F1)
        setMode(Mode::Mode_Player);
    else if(key == sf::Keyboard::F2)
        setMode(Mode::Mode_FreeView);
    else if(key == sf::Keyboard::E)
    {
        for(int i = 0; i < ARRAY_COUNT(player.inventory.mainSlots); i++)
        {
            Inventory::Slot slot = player.inventory.mainSlots[i];
            if(slot.blockId != 0 && slot.stacks != 0)
            {
                printf("%s: %d\n", blockStore.blocks[slot.blockId].name, slot.stacks);
            }
        }
    }
    else if(key == sf::Keyboard::Add)
    {
        player.inventory.nextSlot();
    }
    else if(key == sf::Keyboard::Subtract)
    {
        player.inventory.prevSlot();
    }
}

void Game::mouseClick(int button)
{
    if(mode == Mode::Mode_Player)
    {
        RaycastHit hit;
        if(world->lineCast(hit, activeCam->transform.position, activeCam->transform.position + 10.0f*activeCam->transform.forward()))
        {
            if (button == 0)
            {
                uint8_t gotBlock = world->setBlockId(hit.block, 0);
                if(gotBlock == 2) // grass changes to dirt when broken
                    gotBlock = 3;
                player.inventory.tryStoreBlock(gotBlock);
            }
            else if(button == 1)
            {
                uint8_t gotBlock = player.inventory.tryRemoveBlock(player.inventory.activeSlot);
                if(gotBlock != 0)
                {
                    world->setBlockId(hit.block + hit.faceDirection, gotBlock);
                }
            }
        }
    }
}

void Game::mouseScroll(int delta)
{
    if(delta > 0)
    {
        player.inventory.nextSlot();
    }
    else if(delta < 0)
    {
        player.inventory.prevSlot();
    }
}

void Game::updateAndRender(Renderer *renderer, float dt)
{    
    simulate(renderer, dt);

    this->activeCam->targetWidth = renderer->width;
    this->activeCam->targetHeight = renderer->height;

    renderer->clearScreen(Vec4(0.8f, 0.8f, 0.8f, 1.0f));
    world->render();
    if(mode == Mode::Mode_Player)
        gui->render(dt);
}
