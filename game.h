#include "Maths/maths.h"

class Game
{
public:
    void simulate(float dt);
    void render(class Renderer *renderer);

    bool initialized = false;
};
