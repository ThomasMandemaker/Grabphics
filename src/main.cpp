#include "shaders.hpp"
#include "stb_image.h"
#include <OpenGL/OpenGL.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
#include <ostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void init_triangle(uint *VAO, uint *VBO, float *vertices, size_t vertices_size);
void draw_indices(uint VAO, uint triangle_count);
void init_ebo(uint *EBO, uint *indices, size_t indices_size);
void init_attribute(unsigned char attribute_index, uint size_index, uint size_vertex, uint offset);
void init_vertices(uint initializedVAO, uint initializedVBO, float *vertices);
uint init_texture();

const char* vertexShaderPath = "shader_files/inverse_vertex.vert";
const char* normalVertexPath = "shader_files/normal_vert.vert";
const char* yellow_fragment = "shader_files/yellow_fragment.frag";
const char* multiColorFragShaderPath = "shader_files/multicolor_fragment.frag";
const char* textureFragShaderPath = "shader_files/texture_fragment.frag";

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //uncomment this statement to fix compilation on OS X
#endif
	// glfw window creation
	// --------------------
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    std::optional<ShaderProgram> shader_program = init_from_disk(vertexShaderPath, textureFragShaderPath);
    if (!shader_program)
    {
        return -1;
    }

    float vertices[] = {
        -.5f, -.5f, .0f, 0.0f, 0.0f,
        .5f, -.5f, .0f, 1.0f, 0.0f,
        .5f, .5f, .0f, 1.0f, 1.0f,
        -.5f, .5f, .0f, 0.0f, 1.0f
    };

    uint indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    uint texture = init_texture();
    
    uint VBO, VAO, EBO;
    init_triangle(&VAO, &VBO, vertices, sizeof(vertices));
    init_ebo(&EBO, indices, sizeof(indices));

    init_attribute(0, 3, 5, 0);
    init_attribute(1, 2, 5, 3);
    
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program->ID);

        // draw
        glBindTexture(GL_TEXTURE_2D, texture);
        draw_indices(VAO, 6);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    shader_program.reset();
//    shader_program2.reset();
	// glfw: terminate, clearing all previously allocated GLFWresources.
	//---------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void init_attribute(unsigned char attribute_index, uint size_index, uint size_vertex, uint offset)
{
    glVertexAttribPointer(attribute_index, size_index, GL_FLOAT, GL_FALSE, size_vertex * sizeof(float), (void*)(offset*sizeof(float)));
    glEnableVertexAttribArray(attribute_index);
}

void draw_indices(uint VAO, uint triangle_count)
{
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, triangle_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
}

void init_triangle(uint* VAO, uint* VBO, float *vertices, size_t vertices_size)
{
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
}

void init_ebo(uint *EBO, uint *indices, size_t indices_size)
{
    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
}

uint init_texture()
{
    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/smallmonst.png", &width, &height, &nrChannels, 0);
    
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load in image." << std::endl;
    }

    stbi_image_free(data);
    return texture;
}

void init_vertices(uint initializedVAO, uint initializedVBO, float *vertices, size_t size)
{
    glBindVertexArray(initializedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, initializedVBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void init_other()
{
    float vertices[] = {
        -.5f, -.5f, .0f, 1.0f, 0.0f, 0.0f,
        .5f, -.5f, .0f, 0.0f, 1.0f, 0.0f,
        .0f, .5f, .0f, 0.0f, 0.0f, 1.0f,
    };
    unsigned int VBOS[2], VAOS[2];
    glGenVertexArrays(2, VAOS);
    glGenBuffers(2, VBOS);

    glBindVertexArray(VAOS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float vertices2[] = {
        .5f, -.5f, .0f,
        1.0f, -.5f, .0f,
        .75f, .5f, .0f
    };

    glBindVertexArray(VAOS[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
