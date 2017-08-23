

class Gui
{
public:
    Gui(class Renderer *renderer, class Player *player, class BlockStore *blockStore);
    void render(float dt);

    class Renderer *renderer;
    class Player *player;
    class BlockStore *blockStore;
};
