#pragma once
#include <glad/glad.h>
#include <optional>
#include <iostream>

struct ShaderProgram{
    unsigned int ID;

    explicit ShaderProgram(unsigned int id) noexcept : ID(id) {}

    ~ShaderProgram() {
        if (!ID) {
            return;
        }

        std::cout << "Destructor called" << std::endl;

        glDeleteProgram(ID);
        ID = 0;
    }
};

std::optional<std::string> read_shader_from_disk(const char* shaderFilePath);
std::optional<ShaderProgram> init_from_disk(const char* vertexShaderPath, const char* fragmentShaderPath);
std::optional<ShaderProgram> init(const char* vertexShaderSource, const char* fragmentShaderSource);
