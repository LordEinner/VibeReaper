#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

namespace VibeReaper {

    // Vertex structure for 3D rendering
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;

        Vertex() : position(0.0f), normal(0.0f), texCoord(0.0f) {}
        Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv)
            : position(pos), normal(norm), texCoord(uv) {}
    };

    class Mesh {
    public:
        // Mesh data
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Constructor
        Mesh();
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        
        // Move semantics
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        // Disable copying to prevent double-free of OpenGL buffers
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        ~Mesh();

        // Setup mesh buffers on GPU
        void SetupMesh();

        // Draw the mesh
        void Draw(Shader& shader);

        // Procedural geometry generators
        static Mesh GenerateCube();
        static Mesh GenerateSphere(int subdivisions = 2);
        static Mesh GeneratePlane(float width = 10.0f, float depth = 10.0f);

    private:
        // OpenGL buffer objects
        unsigned int VAO, VBO, EBO;
        bool isSetup;

        // Cleanup
        void Cleanup();
    };

} // namespace VibeReaper
