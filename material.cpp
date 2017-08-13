#include "material.h"
#include <cstring>
#include <cstdio>
#include <algorithm>

Shader::Shader()
{
    this->flags = 0;
}

Shader::~Shader()
{
    for(auto const& value : shaderCodes) 
    {
        if(value.code != nullptr)
        {
            delete value.code;
        }
    }
}

void Shader::addCode(CodeType type, const char* codedata, uint32_t size)
{
    // remove all code with same type
    shaderCodes.erase(std::remove_if(shaderCodes.begin(), shaderCodes.end(), 
        [type](ShaderCode x) { return x.type == type; }), shaderCodes.end());

    ShaderCode code;
    code.code = new char[size+1];
    code.type = type;
    memcpy(code.code, codedata, size+1);
    this->flags |= Flags::HasCode;
    this->flags |= Flags::Dirty;
    this->shaderCodes.push_back(code);
    code.code = nullptr;
}

const char* Shader::getCode(CodeType type)
{
    for(auto const& value : shaderCodes)
    {
        if(value.type == type)
        {
            return (const char*)value.code;
        }
    }
    return nullptr;
}

Material::Material(Shader *shader)
{
    this->shader = shader;
}
