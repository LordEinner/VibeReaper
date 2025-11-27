#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace VibeReaper {

class Shader {
public:
    Shader();
    ~Shader();

    // Load and compile shaders from file paths
    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    // Activate this shader program
    void Use() const;

    // Uniform setters for various types
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

    // Get the program ID
    GLuint GetProgramID() const { return m_programID; }

private:
    GLuint m_programID;

    // Helper functions
    std::string ReadFile(const std::string& filePath);
    GLuint CompileShader(GLenum type, const std::string& source);
    bool LinkProgram(GLuint vertexShader, GLuint fragmentShader);
    void CheckCompileErrors(GLuint shader, const std::string& type);
    void CheckLinkErrors(GLuint program);
};

} // namespace VibeReaper
