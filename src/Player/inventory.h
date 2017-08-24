#pragma once

#include <stdint.h>

class Inventory
{
public:
    struct Slot
    {
        uint32_t blockId = 0;
        uint32_t stacks = 0;
    };

    Inventory();
    bool tryStoreBlock(uint32_t blockId);
    uint8_t tryRemoveBlock(uint32_t slotId);
    uint32_t findBlockSlot(uint32_t blockId);
    void swapToFilledSlot(uint32_t prevBlockId);
    void nextSlot();
    void prevSlot();

    Slot mainSlots[10];   
    uint32_t activeSlot = 0;
};
