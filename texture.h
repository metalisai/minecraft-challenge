#include <stdint.h>

class Texture
{
public:
    enum Flags
    {
        HasData = 1 << 0,
        Uploaded = 1 << 1,
        IsTextureArray = 1 << 2
    };

    Texture(int width, int height, int components);
    ~Texture();
    void copyData(void *src, int width, int height, int components);

    uintptr_t renderer_handle;
    unsigned int flags;
    void *data;
    int width;
    int height;
    int components;
};

class TextureArray : public Texture
{
public:
    TextureArray(int width, int height, int components, int layers);
    void copyLayer(void *src, int width, int height, int components, int layer);
    int layers;
};
