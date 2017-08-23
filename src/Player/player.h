
#include "../camera.h"
#include "inventory.h"
#include "../Maths/maths.h"

class Player
{
public:
    Player();
    void update(float dt, Vec2 mouseDelta);

    Camera camera;
    Inventory inventory;
    Transform transform;
    Vec3 velocity;

    float rotX; // horizontal
    float rotY;

    class World *world;
};
