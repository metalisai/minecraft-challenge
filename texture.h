#include <stdint.h>

class Texture
{
public:
    enum Flags
    {
        HasData = 1 << 0,
        Uploaded = 1 << 1
    };

    Texture();
    Texture(int width, int height);
    ~Texture();
    void copyData(void *src, int width, int height, int components);

    uintptr_t renderer_handle;
    unsigned int flags;
    void *data;
    int width;
    int height;
    int components;
};
