#include "renderer.h"

#include <SFML/Graphics.hpp>
#include <cstring>
#include <assert.h>
#include "mesh.h"
#include "texture.h"
#include "macros.h"

const char* vertex_shader_str = 
R"foo(#version 130
in vec4 position; 
in vec2 uv;
in vec4 color;

uniform mat4 MVP;

out vec4 fragColor;
out vec2 fragUv;

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
in vec2 fragUv;

uniform sampler2D tex;

void main() {
    vec2 uv = vec2(fragUv.x, 1.0f - fragUv.y);
    outColor = texture(tex, uv);
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
}

void Renderer::clearScreen(Vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
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
        if(texture->components == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGB,
                    GL_UNSIGNED_BYTE, texture->data);

        }
        else if(texture->components == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, texture->data);
        }
        else
        {
            fprintf(stderr, "Renderer doesn't support component count: %d\n", texture->components);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        texture->flags |= Texture::Flags::Uploaded;
        texture->renderer_handle = gltex;
    }
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
        size_t vbufsize = 0;
        size_t offset = 0;
        vbufsize += mesh->numVertices*sizeof(Vec3);
        if(FLAGSET(mesh->flags, Mesh::Flags::HasTexCoords))
        {
            vbufsize += mesh->numVertices*sizeof(Vec2);
        }

        // TODO: replace assert with logging an error?
        assert(FLAGSET(mesh->flags, Mesh::Flags::HasVertices));
        assert(FLAGSET(mesh->flags, Mesh::Flags::HasIndices));
        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vbufsize, 0, GL_STATIC_DRAW);

        // copy vertices
        glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->numVertices*sizeof(Vec3), (GLvoid*)mesh->vertices);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
        offset += mesh->numVertices*sizeof(Vec3);

        // copy texture coordinates 
        if(FLAGSET(mesh->flags, Mesh::Flags::HasTexCoords))
        {
            glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->numVertices*sizeof(Vec2), (GLvoid*)mesh->texCoords);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
            offset += mesh->numVertices*sizeof(Vec2);
            //printf("Set Tex coords\n");
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numIndices*sizeof(mesh->indices[0]), mesh->indices, GL_STATIC_DRAW);
        mesh->rendererHandle = vao;
        mesh->rendererHandle2 = vbo;
        mesh->rendererHandle3 = ebo;
        mesh->flags |= Mesh::Flags::Uploaded;
        mesh->flags &= ~Mesh::Flags::Dirty;
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
    window->display();
}
