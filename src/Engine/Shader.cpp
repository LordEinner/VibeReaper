#include "Shader.h"
#include "../Utils/Logger.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace VibeReaper {

Shader::Shader() : m_programID(0) {
}

Shader::~Shader() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
    }
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    // Read shader source code from files
    std::string vertexCode = ReadFile(vertexPath);
    std::string fragmentCode = ReadFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        LOG_ERROR("Failed to read shader files");
        return false;
    }

    // Compile shaders
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    if (vertexShader == 0 || fragmentShader == 0) {
        LOG_ERROR("Failed to compile shaders");
        return false;
    }

    // Link program
    bool success = LinkProgram(vertexShader, fragmentShader);

    // Clean up shader objects (they're linked into the program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (success) {
        LOG_INFO("Shader program created successfully: " + vertexPath + " + " + fragmentPath);
    }

    return success;
}

void Shader::Use() const {
    glUseProgram(m_programID);
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

std::string Shader::ReadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open file: " + filePath);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    CheckCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);

    // Check for linking errors
    CheckLinkErrors(m_programID);

    GLint success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteProgram(m_programID);
        m_programID = 0;
        return false;
    }

    return true;
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        LOG_ERROR("Shader compilation error (" + type + "): " + std::string(infoLog));
    }
}

void Shader::CheckLinkErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        LOG_ERROR("Shader linking error: " + std::string(infoLog));
    }
}

} // namespace VibeReaper
