#include "../renderer.h"

#include <SFML/Graphics.hpp>
#include <cstring>
#include <assert.h>
#include "../mesh.h"
#include "../texture.h"
#include "../../macros.h"

const char* vertex_shader_str = 
R"foo(#version 130
in vec4 position; 
in vec3 uv;
in vec4 color;

uniform mat4 MVP;

out vec4 fragColor;
out vec3 fragUv;

void main() {
	gl_Position = MVP*position;
    //fragColor = color;
    fragUv = uv;
    //gl_Position = position;
})foo";

const char* frag_shader_str = 
R"foo(#version 130
out vec4 outColor;

in vec4 fragColor;
in vec3 fragUv;

uniform sampler2D tex;
uniform sampler2DArray texArr;

void main() {
    //vec2 uv = vec2(fragUv.x, 1.0f - fragUv.y);
    vec3 uv3 = vec3(fragUv.x, 1.0f - fragUv.y, fragUv.z);
    //outColor = texture(tex, uv);
    outColor = texture(texArr, uv3);
})foo";

Shader *Renderer::defaultShader;
Material *Renderer::defaultMaterial;

Renderer::Renderer(float width, float height)
{
    if(defaultShader == nullptr)
    {
        defaultShader = new Shader();
        defaultShader->addCode(Shader::CodeType::OpenGL_VertexShader, vertex_shader_str, strlen(vertex_shader_str)); 
        defaultShader->addCode(Shader::CodeType::OpenGL_FragmentShader, frag_shader_str, strlen(frag_shader_str)); 
        defaultMaterial = new Material(defaultShader);
    }
    this->width = width;
    this->height = height;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glAlphaFunc(GL_GREATER, 0.1f);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void Renderer::clearScreen(Vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
}

bool CheckShaderCompileStatus(const char *sname, GLint shader)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
        char formatBuf[1024];
		char errorBuf[1024];
		glGetShaderInfoLog(shader, 1024, NULL, errorBuf);
        sprintf(formatBuf, "%s: %s\n", sname, errorBuf);
        fprintf(stderr, formatBuf);
	}
	return status == GL_TRUE;
}

static bool compile_shader(GLuint *compiledProgram, const char *vertexShader, const char *fragmentShader)
{
	bool success = true;
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	int vert_shader_len = strlen(vertexShader);
	int frag_shader_len = strlen(fragmentShader);
	glShaderSource(vert_shader, 1, &vertexShader, &vert_shader_len);
	glShaderSource(frag_shader, 1, &fragmentShader, &frag_shader_len);
	glCompileShader(vert_shader);
	glCompileShader(frag_shader);
	if (!CheckShaderCompileStatus("VertexShader", vert_shader) || !CheckShaderCompileStatus("FragmentShader", frag_shader))
	{
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);
		return false;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		char errorBuf[1024];
		glGetProgramInfoLog(program, 1024, NULL, errorBuf);
        fprintf(stderr, errorBuf);
		success = false;
	}
	else
	{
		*compiledProgram = program;
	}
	glDetachShader(program, vert_shader);
	glDetachShader(program, frag_shader);
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	return success;
}

void Renderer::checkTexture(Texture *texture)
{
    // texture used but doesn't even have data
    assert(FLAGSET(texture->flags, Texture::Flags::HasData));

    if(!FLAGSET(texture->flags, Texture::Flags::Uploaded))
    {
        GLuint gltex;
        glGenTextures(1, &gltex);
        glBindTexture(GL_TEXTURE_2D, gltex);
        assert(texture->components == 3 || texture->components == 4);
        GLuint format = texture->components == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, format,
                    GL_UNSIGNED_BYTE, texture->data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        texture->flags |= Texture::Flags::Uploaded;
        texture->renderer_handle = gltex;
    }
}

void Renderer::checkTextureArray(TextureArray *texarr)
{
    assert(FLAGSET(texarr->flags, Texture::Flags::HasData));
    if(!FLAGSET(texarr->flags, Texture::Flags::Uploaded))
    {
        GLuint gltex;
        glGenTextures(1, &gltex);
        glBindTexture(GL_TEXTURE_2D_ARRAY, gltex);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, texarr->width, texarr->height, texarr->layers);
        assert(texarr->components == 3 || texarr->components == 4);
        GLuint format = texarr->components == 3 ? GL_RGB : GL_RGBA;
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, texarr->width, texarr->height, texarr->layers, format, GL_UNSIGNED_BYTE, texarr->data);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        texarr->flags |= Texture::Flags::Uploaded;
        texarr->renderer_handle = gltex;
    }
}

void Renderer::meshInit(Mesh *mesh)
{
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    mesh->rendererHandle = vao;
    mesh->rendererHandle2 = vbo;
    mesh->rendererHandle3 = ebo;
}

void Renderer::meshLoadData(Mesh *mesh)
{
    size_t vbufsize = 0;
    size_t offset = 0;
    vbufsize += mesh->numVertices*sizeof(Vec3);
    if(FLAGSET(mesh->flags, Mesh::Flags::HasTexCoords))
    {
        vbufsize += mesh->numVertices*sizeof(Vec3);
    }

    // TODO: replace assert with logging an error?
    assert(FLAGSET(mesh->flags, Mesh::Flags::HasVertices));
    assert(FLAGSET(mesh->flags, Mesh::Flags::HasIndices));
   
    glBindVertexArray((GLuint)mesh->rendererHandle);
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)mesh->rendererHandle2);
    glBufferData(GL_ARRAY_BUFFER, vbufsize, 0, GL_STATIC_DRAW);

    // copy vertices
    glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->numVertices*sizeof(Vec3), (GLvoid*)mesh->vertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
    offset += mesh->numVertices*sizeof(Vec3);

    // copy texture coordinates 
    if(FLAGSET(mesh->flags, Mesh::Flags::HasTexCoords))
    {
        glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->numVertices*sizeof(Vec3), (GLvoid*)mesh->texCoords);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
        offset += mesh->numVertices*sizeof(Vec3);
        //printf("Set Tex coords\n");
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)mesh->rendererHandle3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numIndices*sizeof(mesh->indices[0]), mesh->indices, GL_STATIC_DRAW);
    mesh->flags |= Mesh::Flags::Uploaded;
    mesh->flags &= ~Mesh::Flags::Dirty;
}

void Renderer::renderMesh(Mesh *mesh, Material *material, Mat4 *MVP)
{
    // load shader if not loaded
    if(!FLAGSET(material->shader->flags, Shader::Flags::Loaded))
    {
        const char* vertCode = material->shader->getCode(Shader::CodeType::OpenGL_VertexShader);
        const char* fragCode = material->shader->getCode(Shader::CodeType::OpenGL_FragmentShader);
        if(vertCode == nullptr || fragCode == nullptr)
        {
            fprintf(stderr, "Material not completely initialized!\n");
        }
        printf("compile shader!\n");
        GLuint program;
        compile_shader(&program, vertCode, fragCode);
        glBindAttribLocation(program, 0, "position");
        material->shader->renderer_handle = program;
        material->shader->flags |= Shader::Flags::Loaded;
        material->shader->flags &= ~Shader::Flags::Dirty;
    }
    // load mesh if not loaded
    if(!FLAGSET(mesh->flags, Mesh::Flags::Uploaded))
    {
        meshInit(mesh);
        meshLoadData(mesh);
    }
    if(FLAGSET(mesh->flags, Mesh::Flags::Dirty) && FLAGSET(mesh->flags, Mesh::Flags::Uploaded))
    {
        meshLoadData(mesh);
    }

    // TODO: move to separate function
    GLuint useProgram = ((GLuint)material->shader->renderer_handle);
    glUseProgram(useProgram);
    GLuint mvpLoc = glGetUniformLocation(useProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (GLfloat*)MVP);
    int texUnit = 0;
    for (auto it = material->textures.begin(); it != material->textures.end(); ++it)
    {
        checkTexture(it->second);
        GLuint texLoc = glGetUniformLocation(useProgram, it->first.c_str());
        glUniform1i(texLoc, texUnit);
        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D, (GLuint)it->second->renderer_handle);
        texUnit++;
    }
    for (auto it = material->textureArrays.begin(); it != material->textureArrays.end(); ++it)
    {
        checkTextureArray(it->second);
        GLuint texLoc = glGetUniformLocation(useProgram, it->first.c_str());
        glUniform1i(texLoc, texUnit);
        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, (GLuint)it->second->renderer_handle);
        texUnit++;
    }


    glBindVertexArray((GLuint)mesh->rendererHandle);
    glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::resize(float width, float height)
{
    glViewport(0, 0, width, height);
    this->width = width;
    this->height = height;
}

void Renderer::presentFrame(sf::Window *window)
{
    GLuint gerror = glGetError();
    if(gerror != 0)
        fprintf(stderr, "GL error %x\n", gerror);

    window->display();
}

void Renderer::setBlend(bool enabled)
{
    if(enabled)
    {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}
