#include <GL/glew.h>
#include "Maths/maths.h"
#include "material.h"

namespace sf
{
    class Window;
}

class Renderer
{
public:
    Renderer(float width, float height);
    void presentFrame(sf::Window *window);
    void clearScreen(Vec4 color);
    void checkTexture(class Texture *texture);
    void renderMesh(class Mesh *mesh, class Material *material, struct Mat4 *modelM);
    void resize(float width, float height);

    static Shader *defaultShader;
    static Material *defaultMaterial;

    float width;
    float height;
};
