#include "inventory.h"

#include "../macros.h"
#include <assert.h>

Inventory::Inventory()
{
}

bool Inventory::tryStoreBlock(uint32_t blockId)
{
    int count = ARRAY_COUNT(mainSlots);
    // try active block
    if(mainSlots[activeSlot].stacks == 0 || mainSlots[activeSlot].blockId == blockId)
    {
        mainSlots[activeSlot].blockId = blockId;
        mainSlots[activeSlot].stacks++;
        return true;
    }
    // try to find some other slot with that block
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

uint32_t Inventory::findBlockSlot(uint32_t blockId)
{
    int count = ARRAY_COUNT(mainSlots);
    for(int i = 0; i < count; i++)
    {
        if(mainSlots[i].blockId == blockId && mainSlots[i].stacks > 0)
        {
            return i;
        }
    }
    return 0;
}

void Inventory::swapToFilledSlot(uint32_t prevBlockId)
{
    if(prevBlockId != 0)
    {
        // try to swap to block of same type
        uint32_t slotId = findBlockSlot(prevBlockId);
        if(slotId != 0)
        {
            activeSlot = slotId;
            return;
        }
        // pick any filled slot otherwise
        else
        {
            int count = ARRAY_COUNT(mainSlots);
            for(int i = 0; i < count; i++)
            {
                if(mainSlots[i].blockId != 0 && mainSlots[i].stacks > 0)
                {
                    activeSlot = i;
                    return;
                }
            }
        }
    }
}

uint8_t Inventory::tryRemoveBlock(uint32_t slotId)
{
    assert(slotId >= 0 && slotId < ARRAY_COUNT(mainSlots));
    if(mainSlots[slotId].stacks > 0 && mainSlots[slotId].blockId != 0)
    {
        mainSlots[slotId].stacks--;
        if(mainSlots[slotId].stacks == 0)
        {
            swapToFilledSlot(mainSlots[slotId].blockId);
            mainSlots[slotId].blockId = 0;
        }
        return mainSlots[slotId].blockId;
    }
    return 0;
}

void Inventory::nextSlot()
{
    activeSlot++;
    activeSlot %= ARRAY_COUNT(mainSlots);
}

void Inventory::prevSlot()
{
    activeSlot--;
    if(activeSlot > ARRAY_COUNT(mainSlots))
        activeSlot = ARRAY_COUNT(mainSlots)-1;
}

