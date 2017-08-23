#include "inventory.h"

#include "../macros.h"
#include <assert.h>

Inventory::Inventory()
{
}

bool Inventory::tryStoreBlock(uint32_t blockId)
{
    int count = ARRAY_COUNT(mainSlots);
    // try to find slot with that block
    for(int i = 0; i < count; i++)
    {
        if(mainSlots[i].blockId == blockId && mainSlots[i].stacks < 64)
        {
            mainSlots[i].stacks++;
            return true;
        }
    }
    // try find empty slot
    for(int i = 0; i < count; i++)
    {
        if(mainSlots[i].stacks == 0)
        {
            mainSlots[i].blockId = blockId;
            mainSlots[i].stacks = 1;
            return true;
        }
    }
    return false;
}

bool Inventory::tryRemoveBlock(uint32_t slotId)
{
    assert(slotId >= 0 && slotId < ARRAY_COUNT(mainSlots));
    if(mainSlots[slotId].stacks > 0 && mainSlots[slotId].blockId != 0)
    {
        mainSlots[slotId].stacks--;
        return true;
    }
    return false;
}

