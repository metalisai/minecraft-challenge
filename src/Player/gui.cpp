#include "gui.h"

#include "../Renderer/renderer.h"
#include "player.h"
#include "../GameWorld/blockstore.h"

Gui::Gui(Renderer *renderer, Player *player, BlockStore *bs)
{
    this->renderer = renderer;
    this->player = player;
    this->blockStore = bs;
}

void Gui::renderOutline(Vec2 pos, Vec2 size, float thickness, Vec4 color)
{
    renderer->renderImmediateQuad(pos, Vec2(size.x, thickness), color);
    renderer->renderImmediateQuad(pos + Vec2(0.0f, size.y - thickness), Vec2(size.x, thickness), color);
    renderer->renderImmediateQuad(pos + Vec2(0.0f, thickness), Vec2(thickness, size.y - 2.0f*thickness), color);
    renderer->renderImmediateQuad(pos + Vec2(size.x - thickness, thickness), Vec2(thickness, size.y - 2.0f*thickness), color);
}

void Gui::render(float dt)
{
    Mat4 mat = Mat4::Ortho(0.0f, renderer->width, 0.0f, renderer->height, 0.0f, 1.0f);
    renderer->immediateMatrix(&mat);

    Vec2 center(this->renderer->width/2.0f, this->renderer->height/2.0f);

    float padding = 5.0f;
    float width = padding * 9.0f + 64.0f * 10.0f + padding * 2.0f;
    float height = 64.0f + padding * 2.0f;
    float posx = center.x - (width / 2.0f);

    for(int i = 0; i < 10; i++)
    {
        if(player->inventory.activeSlot == i)
        {
            //renderer->renderImmediateQuad(Vec2(posx + i*(64.0f+padding), 0.0f), Vec2(64.0f + 2.0f*padding, 64.0f + 2.0f*padding), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
            renderOutline(Vec2(posx + i*(64.0f+padding), 0.0f), Vec2(64.0f + 2.0f*padding, 64.0f + 2.0f*padding), 5.0f, Vec4(1.0f, 0.0f, 0.0f, 0.5f));
        }

        Inventory::Slot slot = player->inventory.mainSlots[i];
        if(slot.blockId != 0 && slot.stacks != 0)
        {
            renderer->renderImmediateQuad(Vec2(posx + padding + i*(64.0f+padding), padding), Vec2(64.0f, 64.0f), blockStore->blocks[slot.blockId].faceTextureLayers[0]);
        }
        else
        {
            renderer->renderImmediateQuad(Vec2(posx + padding + i*(64.0f+padding), padding), Vec2(64.0f, 64.0f), Vec4(0.0f, 0.0f, 0.0f, 0.6f));
        }
    }
    renderer->flushImmediate();
}

