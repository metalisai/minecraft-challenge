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
    bool tryRemoveBlock(uint32_t slotId);

    Slot mainSlots[10];   
};
