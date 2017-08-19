#include "Maths/maths.h"
#include "camera.h"

class Game
{
public:
    void simulate(float dt);
    void render(class Renderer *renderer);

    bool initialized = false;

    Camera mainCam;

    Vec2 mousePosLast;
    Vec2 mouseDelta;

    Vec2 camRot;
};
