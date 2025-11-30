#include "Mesh.h"
#include "../Utils/Logger.h"
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <utility>

namespace VibeReaper {

    Mesh::Mesh()
        : VAO(0), VBO(0), EBO(0), isSetup(false) {
    }

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
        : vertices(vertices), indices(indices), VAO(0), VBO(0), EBO(0), isSetup(false) {
    }

    Mesh::~Mesh() {
        Cleanup();
    }

    Mesh::Mesh(Mesh&& other) noexcept
        : vertices(std::move(other.vertices)),
          indices(std::move(other.indices)),
          VAO(other.VAO),
          VBO(other.VBO),
          EBO(other.EBO),
          isSetup(other.isSetup) {
        
        // Reset other
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
        other.isSetup = false;
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept {
        if (this != &other) {
            Cleanup(); // Clean up existing resources

            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            isSetup = other.isSetup;

            // Reset other
            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
            other.isSetup = false;
        }
        return *this;
    }

    void Mesh::SetupMesh() {
        if (isSetup) return;

        // Generate buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Bind VAO
        glBindVertexArray(VAO);

        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Upload index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers

        // Position attribute (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        // Normal attribute (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        // Texture coordinate attribute (location = 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        // Unbind VAO
        glBindVertexArray(0);

        isSetup = true;
        LOG_INFO("Mesh setup complete: " + std::to_string(vertices.size()) + " vertices, " + 
                 std::to_string(indices.size()) + " indices");
    }

    void Mesh::Draw(Shader& shader) {
        if (!isSetup) {
            LOG_ERROR("Mesh::Draw() called before SetupMesh()");
            return;
        }

        shader.Use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::Cleanup() {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO != 0) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
        isSetup = false;
    }

    // Generate a textured cube (24 vertices for proper normals/UVs)
    Mesh Mesh::GenerateCube() {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Front face (Z+)
        vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)));

        // Back face (Z-)
        vertices.push_back(Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)));

        // Right face (X+)
        vertices.push_back(Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));

        // Left face (X-)
        vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));

        // Top face (Y+)
        vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));

        // Bottom face (Y-)
        vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
        vertices.push_back(Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));

        // Indices for 6 faces (2 triangles per face)
        for (unsigned int i = 0; i < 6; i++) {
            unsigned int base = i * 4;
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }

        Mesh cube(vertices, indices);
        cube.SetupMesh();
        LOG_INFO("Procedural cube generated: 24 vertices, 36 indices");
        return cube;
    }

    // Generate a UV sphere using icosphere subdivision
    Mesh Mesh::GenerateSphere(int subdivisions) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Simple UV sphere (latitude/longitude)
        const int segments = 16 * (subdivisions + 1);
        const int rings = 8 * (subdivisions + 1);
        const float radius = 0.5f;

        for (int ring = 0; ring <= rings; ring++) {
            float phi = glm::pi<float>() * float(ring) / float(rings);
            for (int segment = 0; segment <= segments; segment++) {
                float theta = 2.0f * glm::pi<float>() * float(segment) / float(segments);

                float x = radius * sin(phi) * cos(theta);
                float y = radius * cos(phi);
                float z = radius * sin(phi) * sin(theta);

                glm::vec3 position(x, y, z);
                glm::vec3 normal = glm::normalize(position);
                glm::vec2 texCoord(float(segment) / float(segments), float(ring) / float(rings));

                vertices.push_back(Vertex(position, normal, texCoord));
            }
        }

        // Generate indices
        for (int ring = 0; ring < rings; ring++) {
            for (int segment = 0; segment < segments; segment++) {
                unsigned int current = ring * (segments + 1) + segment;
                unsigned int next = current + segments + 1;

                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        Mesh sphere(vertices, indices);
        sphere.SetupMesh();
        LOG_INFO("Procedural sphere generated: " + std::to_string(vertices.size()) + " vertices, " + 
                 std::to_string(indices.size()) + " indices");
        return sphere;
    }

    // Generate a plane
    Mesh Mesh::GeneratePlane(float width, float depth) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float halfWidth = width * 0.5f;
        float halfDepth = depth * 0.5f;

        // 4 vertices for a simple quad
        vertices.push_back(Vertex(glm::vec3(-halfWidth, 0.0f,  halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfWidth, 0.0f,  halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(width, 0.0f)));
        vertices.push_back(Vertex(glm::vec3( halfWidth, 0.0f, -halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(width, depth)));
        vertices.push_back(Vertex(glm::vec3(-halfWidth, 0.0f, -halfDepth), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, depth)));

        // 2 triangles
        indices = { 0, 1, 2, 0, 2, 3 };

        Mesh plane(vertices, indices);
        plane.SetupMesh();
        LOG_INFO("Procedural plane generated: 4 vertices, 6 indices");
        return plane;
    }

} // namespace VibeReaper
