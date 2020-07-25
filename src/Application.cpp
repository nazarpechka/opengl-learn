#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct vec4 {
    float v0, v1, v2, v3;
};

int main(void) {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit()) return -1;

    // Set this for macOS to use latest OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Change fps, sync with monitor 60hz

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // Three vertices with one attribute - position
    float positions[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,  // 0
        0.5f,  -0.5f, 1.0f, 0.0f,  // 1
        0.5f,  0.5f,  1.0f, 1.0f,  // 2
        -0.5f, 0.5f,  0.0f, 1.0f   // 3
    };

    // Indices are needed not to duplicate vertices in memory with
    // same position, just refer to them by indexes - index Buffers
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    // Blending setup
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray va;

    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

    // Shaders are combined (linked) in one program which will run on GPU
    Shader shader("res/shaders/Basic.shader");
    shader.Bind();

    vec4 color = {0.2f, 0.3f, 0.8f, 1.0f};
    shader.SetUniform4f("u_Color", color.v0, color.v1, color.v2, color.v3);
    shader.SetUniformMat4f("u_MVP", proj);

    Texture texture("res/textures/masyanya_logo.png");
    texture.Bind();

    // u_Texture - slot where texture is bound to
    shader.SetUniform1i("u_Texture", 0);

    // Print OpenGL version
    std::cout << glGetString(GL_VERSION) << std::endl;

    // Simulate unbinding
    va.Unbind();
    vb.Unbind();
    ib.Unbind();

    shader.Unbind();

    Renderer renderer;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        renderer.Clear();

        shader.Bind();
        color.v0 = (color.v0 >= 1.0f) ? 0.0f : color.v0 + 0.05f;
        color.v1 = (color.v1 >= 1.0f) ? 0.0f : color.v1 + 0.05f;
        color.v2 = (color.v2 >= 1.0f) ? 0.0f : color.v2 + 0.05f;
        color.v3 = (color.v3 >= 1.0f) ? 0.0f : color.v3 + 0.05f;

        shader.SetUniform4f("u_Color", color.v0, color.v1, color.v2, color.v3);

        renderer.Draw(va, ib, shader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}