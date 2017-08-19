
#include <stdint.h>
#include <vector>
#include <map>
#include "../Maths/maths.h"


class Shader
{
public:
    enum Flags
    {
        HasCode = 1 << 0,
        Loaded = 1 << 1,
        Dirty = 1 << 2
    };

    enum CodeType
    {
        OpenGL_VertexShader,
        OpenGL_FragmentShader
    };

    enum ShaderType
    {
        OpenGL_Surface,
        OpenGL_Compute
    };

    struct ShaderCode
    {
        CodeType type;
        char *code = nullptr;
    };

    Shader();
    ~Shader();
    void addCode(CodeType type, const char* code, uint32_t size);
    const char* getCode(CodeType type);

    uintptr_t renderer_handle;
    uint16_t flags;
    std::vector<ShaderCode> shaderCodes;
};

class Material
{
public:
    Material(Shader *shader);
    void setInt(const char* name, int value);
    void setFloat(const char* name, float value);
    void setVec2(const char* name, Vec2 value);
    void setVec3(const char* name, Vec3 value);
    void setVec4(const char* name, Vec4 value);
    void setMatrix4(const char* name, const Mat4 *value);
    void addTexture(const char* name, class Texture *tex);
    void addTextureArray(const char* name, class TextureArray *texArr);

    class Shader *shader;

    std::map<std::string, class Texture*> textures;
    std::map<std::string, class TextureArray*> textureArrays;
};
