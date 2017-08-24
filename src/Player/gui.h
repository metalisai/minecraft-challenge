#include "../Maths/maths.h"

class Gui
{
public:
    Gui(class Renderer *renderer, class Player *player, class BlockStore *blockStore);
    void render(float dt);
    void renderOutline(Vec2 pos, Vec2 size, float thickness, Vec4 color);

    class Renderer *renderer;
    class Player *player;
    class BlockStore *blockStore;
};
