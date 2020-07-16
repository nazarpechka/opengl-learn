#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
  std::string VertexSource;
  std::string FragmentSource;
};

struct vec4
{
  float v0, v1, v2, v3;
};

static ShaderProgramSource ParseShader(const std::string &filepath)
{
  // Open file which is divided to shaders by #shader statemenets
  std::ifstream stream(filepath);

  enum class ShaderType
  {
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
  };

  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;

  while (getline(stream, line))
  {
    if (line.find("#shader") != std::string::npos)
    {
      if (line.find("vertex") != std::string::npos)
        type = ShaderType::VERTEX;
      else if (line.find("fragment") != std::string::npos)
        type = ShaderType::FRAGMENT;
    }
    else
    {
      ss[(int)type] << line << '\n';
    }
  }

  stream.close();

  return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type,
                                  const std::string &source)
{
  // Create shader in VRAM, load its source, compile it
  unsigned int id;
  GLCall(id = glCreateShader(type));
  const char *src = source.c_str();
  GLCall(glShaderSource(id, 1, &src, nullptr));
  GLCall(glCompileShader(id));

  // Check for compilation errors
  int result;
  GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

  if (result == GL_FALSE)
  {
    int length;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

    // Hacky allocation of char[] on stack with variable size
    char *message = (char *)alloca(length * sizeof(char));
    GLCall(glGetShaderInfoLog(id, length, &length, message));

    std::cout << "Failed to compile "
              << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
              << " shader:\n"
              << message << "\n";

    GLCall(glDeleteShader(id));
    return 0;
  }

  return id;
}

static unsigned int CreateShader(const std::string &vertexShader,
                                 const std::string &fragmentShader)
{
  // Create a program and compile two shaders
  unsigned int program;
  GLCall(program = glCreateProgram());
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  // Attach both shaders to program
  GLCall(glAttachShader(program, vs));
  GLCall(glAttachShader(program, fs));
  // Link the program (like c++ linking)
  GLCall(glLinkProgram(program));
  GLCall(glValidateProgram(program));

  // Delete shaders, we don't need them anymore
  GLCall(glDeleteShader(vs));
  GLCall(glDeleteShader(fs));
  // Also you can delete shader sources, but then you can't debug properly

  return program;
}

int main(void)
{

  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  // Set this for macOS to use latest OpenGL 4.1
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Change fps, sync with monitor 60hz

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();

  // Three vertices with one attribute - position
  float positions[] = {
      -0.5f, -0.5f, // 0
      0.5f, -0.5f,  // 1
      0.5f, 0.5f,   // 2
      -0.5f, 0.5f,  // 3
  };

  // Indices are needed not to duplicate vertices in memory with
  // same position, just refer to them by indexes - index Buffers
  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  // Vertex array object is needed to bind vertex bbuffer to specific
  // vertex layout for easy later use.
  // Then, when rendering you need to only bind vertex array instead of
  // vertex buffer + vertex attrib array
  // for example glBindVertexArray(vao);
  GLuint vao;
  GLCall(glGenVertexArrays(1, &vao));
  GLCall(glBindVertexArray(vao));

  // Create one buffer in VRAM, bind it as GL_ARRAY_BUFFER, load data in it - 24
  // bytes from positions GL_STATIC_DRAW - STATIC means we use it many times,
  // change once, and DRAW means we would DRAW it

  // unsigned int buffer;
  // GLCall(glGenBuffers(1, &buffer));
  // GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
  // GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions,
  //                     GL_STATIC_DRAW));

  VertexBuffer vBuffer(positions, 4 * 2 * sizeof(float));

  // Let OpenGL know what type our attributes are
  // Attribute id, amount of values in attribute (1...4), type itself, should it
  // be normalized or not, size of vertex, offset to attribute from vertex start
  GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
  GLCall(glEnableVertexAttribArray(0));

  // Generating index buffers
  // Type should always be unsigned!

  // unsigned int ibo;
  // GLCall(glGenBuffers(1, &ibo));
  // GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
  // GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int),
  //                     indices, GL_STATIC_DRAW));

  IndexBuffer iBuffer(indices, 6);

  // Vertex shader is run for every vertex once
  // It tells where on screen vertex should be positioned

  // Fragment shader is run for every pixel, it tells its colour

  ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

  // Shaders are combined (linked) in one program which will run on GPU
  unsigned int shader =
      CreateShader(source.VertexSource, source.FragmentSource);
  GLCall(glUseProgram(shader));

  vec4 color = {0.2f, 0.3f, 0.8f, 1.0f};
  GLCall(int location = glGetUniformLocation(shader, "u_Color"));
  ASSERT(location !=
         -1); // If uniform is unused in shader, it will also return -1
  GLCall(glUniform4f(location, color.v0, color.v1, color.v2, color.v3));

  // Print OpenGL version
  std::cout << glGetString(GL_VERSION) << std::endl;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    color.v0 = (color.v0 >= 1.0f) ? 0.0f : color.v0 + 0.05f;
    color.v1 = (color.v1 >= 1.0f) ? 0.0f : color.v1 + 0.05f;
    color.v2 = (color.v2 >= 1.0f) ? 0.0f : color.v2 + 0.05f;
    color.v3 = (color.v3 >= 1.0f) ? 0.0f : color.v3 + 0.05f;

    GLCall(glUniform4f(location, color.v0, color.v1, color.v2, color.v3));

    // Draw six vertices forming two triangles
    // glDrawArrays(GL_TRIANGLES, 0, 6); - if drawing without index buffers
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  GLCall(glDeleteProgram(shader));

  glfwTerminate();
  return 0;
}