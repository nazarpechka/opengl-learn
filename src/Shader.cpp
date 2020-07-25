#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Renderer.h"
// Vertex shader is run for every vertex once
// It tells where on screen vertex should be positioned

// Fragment shader is run for every pixel, it tells its colour

Shader::Shader(const std::string &filepath)
    : m_FilePath(filepath), m_RendererID(0) {
    ShaderProgramSource source = ParseShader(filepath);

    // Shaders are combined (linked) in one program which will run on GPU
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}
Shader::~Shader() { GLCall(glDeleteProgram(m_RendererID)); }

ShaderProgramSource Shader::ParseShader(const std::string &filepath) {
    // Open file which is divided to shaders by #shader statemenets
    std::ifstream stream(filepath);

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        } else {
            ss[(int)type] << line << '\n';
        }
    }

    stream.close();

    return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::CompileShader(unsigned int type,
                                   const std::string &source) {
    // Create shader in VRAM, load its source, compile it
    unsigned int id;
    GLCall(id = glCreateShader(type));
    const char *src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Check for compilation errors
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE) {
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

unsigned int Shader::CreateShader(const std::string &vertexShader,
                                  const std::string &fragmentShader) {
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

void Shader::Bind() const { GLCall(glUseProgram(m_RendererID)); }
void Shader::Unbind() const { GLCall(glUseProgram(0)); }

void Shader::SetUniform1i(const std::string &name, int value) {
    GLCall(glUniform1i(GetUniformLocation(name), value));
}
void Shader::SetUniform1f(const std::string &name, float value) {
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2,
                          float v3) {
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &matrix) {
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE,
                              &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string &name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' unused or not found!\n";

    m_UniformLocationCache[name] = location;
    return location;
}