
namespace sf {
    class Window;
};

class Application
{
public:
    Application();
    ~Application();
    void start();
    void doEvents();
    void doFrame();
    void exit();
    bool isRunning();

    sf::Window* window;
private:
    bool initialized;
    class Renderer *mainRenderer;
    class Game *game;
};
