#include "Maths/maths.h"
#include "camera.h"

class Game
{
public:
    void simulate(class Renderer* renderer, float dt);
    void updateAndRender(class Renderer *renderer, float dt);

    bool initialized = false;

    Camera mainCam;

    Vec2 mousePosLast;
    Vec2 mouseDelta;

    Vec2 camRot;
};
