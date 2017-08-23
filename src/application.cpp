#include "application.h"

#include <SFML/Graphics.hpp>
#include "Renderer/renderer.h"
#include "game.h"

Application::Application()
{
    initialized = false;
    window = NULL;
}

Application::~Application()
{
    this->exit();
}

bool Application::isRunning()
{
    if(initialized)
        return window->isOpen();
    return false;
}

void Application::start()
{
    sf::ContextSettings settings;
    settings.attributeFlags = sf::ContextSettings::Core;
    settings.depthBits = 24;
    settings.stencilBits = 0;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    this->window = new sf::Window(sf::VideoMode(1024, 768), "Minecraft clone", sf::Style::Default, settings);
    this->mainRenderer = new Renderer(1024.0f, 768.0f);
    this->game = new Game();
    this->game->window = this->window;
    initialized = true;

    glewInit();
}

void Application::doEvents()
{
    if(!initialized)
        return;

    sf::Event event;
    while (window->pollEvent(event))
    {
        switch(event.type)
        {
            case sf::Event::Closed:
                this->exit();
                return;
            case sf::Event::Resized:
                this->mainRenderer->resize(event.size.width, event.size.height);
                break;
            case sf::Event::MouseButtonPressed:
                if(event.mouseButton.button == sf::Mouse::Right)
                    game->mouseClick(1);
                if(event.mouseButton.button == sf::Mouse::Left)
                    game->mouseClick(0);
                break;
            case sf::Event::KeyPressed:
                game->keyPress(event.key.code);
                break;
            default:
                break;
        }
    }
}

void Application::doFrame()
{
    if(initialized)
    {
        float dt60 = 1.0f / 60.0f;
        game->updateAndRender(mainRenderer, dt60);
        mainRenderer->presentFrame(this->window);
    }
}

void Application::exit()
{
    if(initialized)
    {
        window->close();
        delete this->game;
        this->game = NULL;
        delete this->mainRenderer;
        this->mainRenderer = NULL;
        delete this->window;
        this->window = NULL;
        initialized = false;
    }
}

