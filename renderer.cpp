#include "renderer.h"

#include <SFML/Graphics.hpp>
#include <cstring>
#include <assert.h>
#include "mesh.h"
#include "macros.h"

const char* vertex_shader_str = 
R"foo(#version 130
in vec4 position; 
in vec4 color;
in vec2 uv;

uniform mat4 projection;

out vec4 fragColor;
out vec2 fragUv;

void main() {
	//gl_Position = projection*position;
    //fragColor = color;
    //fragUv = uv;
    gl_Position = position;
})foo";

const char* frag_shader_str = 
R"foo(#version 130
out vec4 outColor;

in vec4 fragColor;
in vec2 fragUv;

uniform sampler2D tex;

void main() {
	//float a = texture(tex, fragUv).r;
    vec3 col = vec3(1.0f, 1.0f, 1.0f);
    outColor = vec4(col, 1.0f);
})foo";

Shader *Renderer::defaultShader;
Material *Renderer::defaultMaterial;

Renderer::Renderer()
{
    if(defaultShader == nullptr)
    {
        defaultShader = new Shader();
        defaultShader->addCode(Shader::CodeType::OpenGL_VertexShader, vertex_shader_str, strlen(vertex_shader_str)); 
        defaultShader->addCode(Shader::CodeType::OpenGL_FragmentShader, frag_shader_str, strlen(frag_shader_str)); 
        defaultMaterial = new Material(defaultShader);
    }
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

void Renderer::renderMesh(Mesh *mesh, Material *material, Mat4 *modelM)
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
        // TODO: replace assert with logging an error?
        assert(FLAGSET(mesh->flags, Mesh::Flags::HasVertices));
        assert(FLAGSET(mesh->flags, Mesh::Flags::HasIndices));
        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh->numVertices*sizeof(Vec3), (GLvoid*)mesh->vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numIndices*sizeof(mesh->indices[0]), mesh->indices, GL_STATIC_DRAW);
        mesh->rendererHandle = vao;
        mesh->rendererHandle2 = vbo;
        mesh->rendererHandle3 = ebo;
        mesh->flags |= Mesh::Flags::Uploaded;
        mesh->flags &= ~Mesh::Flags::Dirty;
    }
    glUseProgram((GLuint)material->shader->renderer_handle);
    glBindVertexArray((GLuint)mesh->rendererHandle);
    glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::resize(float width, float height)
{
    glViewport(0, 0, width, height);
}

void Renderer::presentFrame(sf::Window *window)
{
    window->display();
}
