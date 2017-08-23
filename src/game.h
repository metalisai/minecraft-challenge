#include "Maths/maths.h"
#include "camera.h"
#include "Player/player.h"
#include "GameWorld/blockstore.h"

#include <SFML/Window.hpp>

class Game
{
public:

    enum Mode
    {
        Mode_FreeView,
        Mode_Player
    };

    ~Game();

    void simulate(class Renderer* renderer, float dt);
    void updateAndRender(class Renderer *renderer, float dt);
    void mouseClick(int button);
    void keyPress(sf::Keyboard::Key key);
    void setMode(uint32_t mode);

    bool initialized = false;
    uint32_t mode = Mode::Mode_FreeView;

    Camera *activeCam;
    Player player;
    Camera freeCam;

    Vec2 mousePosLast;
    Vec2 mouseDelta;
    Vec2 camRot;

    BlockStore blockStore;

    // TODO: TEMPORARY
    sf::Window* window;
    class Gui *gui = nullptr;
    class World* world;
    class TextureArray *atlas; // texture atlas
};
