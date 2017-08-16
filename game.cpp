#include "game.h"
#include "renderer.h"
#include "mesh.h"
#include "texture.h"

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
#include "stb_image.h"

Vec3 quadVerts[] =
{
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};

Vec2 quadTexCoords[] = 
{
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
};

uint16_t quadIndices[] = 
{
    0, 1, 2, 0, 2, 3
};

Mesh mesh;
Texture grass;

float rot;

void Game::simulate(float dt)
{
    if(!initialized)
    {
        initialized = true;
        mesh.copyVertices(quadVerts, 4);
        mesh.copyTexCoords(quadTexCoords, 4);
        mesh.copyIndices(quadIndices, 6);

        this->mainCam.transform.position = Vec3(0.0f, 0.0f, 3.0f);
        sf::Vector2i globalPosition = sf::Mouse::getPosition();
        mousePosLast = Vec2((float)globalPosition.x, (float)globalPosition.y);
        camRot = Vec2(0.0f, 0.0f);

        // texture
        int x,y,n;
        unsigned char *data = stbi_load("Resources/grass.png", &x, &y, &n, 0);
        grass.copyData(data, x, y, n);

        stbi_image_free(data);
        Renderer::defaultMaterial->addTexture("tex", &grass);
    }

    // TODO: input system
    const float moveSpeed = 1.0f;
    const float rotSpeed = 0.4f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        this->mainCam.transform.position += dt*mainCam.transform.forward();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->mainCam.transform.position -= dt*mainCam.transform.forward();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->mainCam.transform.position -= dt*mainCam.transform.right();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->mainCam.transform.position += dt*mainCam.transform.right();
    }

    // get the global mouse position (relative to the desktop)
    sf::Vector2i globalPosition = sf::Mouse::getPosition();
    this->mouseDelta = Vec2(globalPosition.x - mousePosLast.x, globalPosition.y - mousePosLast.y);
    mousePosLast = Vec2((float)globalPosition.x, (float)globalPosition.y);
    camRot += this->mouseDelta;
    //camRot.x = fmod(camRot.x, 360.0f);
    //camRot.y = fmod(camRot.y, 360.0f);

    Quaternion rotX = Quaternion::AngleAxis(rotSpeed * -camRot.x, Vec3(0.0f, 1.0f, 0.0f));
    Quaternion rotY = Quaternion::AngleAxis(rotSpeed * -camRot.y, Vec3(1.0f, 0.0f, 0.0f));
    this->mainCam.transform.rotation = rotX * rotY;

    rot += 1.0f;
}

void Game::render(Renderer *renderer)
{    
    this->mainCam.targetWidth = renderer->width;
    this->mainCam.targetHeight = renderer->height;

    Mat4 world_to_clip = mainCam.getViewProjectionMatrix();

    Quaternion rotQ = Quaternion::AngleAxis(rot, Vec3(0.0f, 1.0f, 0.0f));
    //Mat4 model_to_world = Mat4::Rotation(rotQ);
    Mat4 model_to_world = Mat4::Identity();

    Mat4 model_to_clip = world_to_clip * model_to_world;
    //Mat4 model_to_clip = Mat4::Identity();

    renderer->clearScreen(Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    renderer->renderMesh(&mesh, renderer->defaultMaterial, &model_to_clip);
}
