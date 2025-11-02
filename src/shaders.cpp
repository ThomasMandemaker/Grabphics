#include "shaders.hpp"
#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>
#include <utility>

unsigned int compile_shader(const char* shaderSource, int shaderType)
{
    unsigned int shader;
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
        return 0;
    }

    return shader;
}

std::optional<std::string> read_shader_from_disk(const char* shaderFilePath)
{
    std::string code;
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(shaderFilePath);
        std::stringstream shaderStream;

        shaderStream << shaderFile.rdbuf();
        shaderFile.close();

        code = shaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return std::nullopt;
    }

    return std::optional<std::string>(code);
}

std::optional<ShaderProgram> init_from_disk(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    std::optional<std::string> vertexShader = read_shader_from_disk(vertexShaderPath);
    if (!vertexShader)
    {
        return std::nullopt;
    }

    std::optional<std::string> fragmentShader = read_shader_from_disk(fragmentShaderPath);
    if (!fragmentShader)
    {
        return std::nullopt;
    }

    return init(vertexShader->c_str(), fragmentShader->c_str());
}

std::optional<ShaderProgram> init(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    unsigned int vertexShader = compile_shader(vertexShaderSource, GL_VERTEX_SHADER);
    if (!vertexShader) {
        return std::nullopt;
    }

    unsigned int fragmentShader = compile_shader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    if (!fragmentShader) {
        return std::nullopt;
    }

    unsigned int shader_program;
    shader_program = glCreateProgram();
    if (shader_program == 0)
    {
        std::cout << "Failed to initialize shader program" << std::endl;
        return std::nullopt;
    }

    glAttachShader(shader_program, vertexShader);
    glAttachShader(shader_program, fragmentShader);
    glLinkProgram(shader_program);

    int success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog;
        glDeleteProgram(shader_program);
        return std::nullopt;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return std::optional<ShaderProgram>(std::in_place, shader_program);
}

