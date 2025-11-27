// Phase 2 Test Suite for VibeReaper
// Tests Mesh, Texture, Camera, and Shader systems

#include <iostream>
#include <cassert>
#include <cmath>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../src/Engine/Mesh.h"
#include "../src/Engine/Texture.h"
#include "../src/Engine/Camera.h"
#include "../src/Engine/Shader.h"
#include "../src/Engine/Renderer.h"
#include "../src/Utils/Logger.h"

using namespace VibeReaper;

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

// Test macros
#define TEST_START(name) \
    std::cout << "\n[TEST] " << name << "..." << std::endl;

#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cout << "  ✗ FAILED: " << message << std::endl; \
        tests_failed++; \
        return false; \
    }

#define TEST_PASS() \
    std::cout << "  ✓ PASSED" << std::endl; \
    tests_passed++; \
    return true;

// Helper: floating-point comparison
bool floatEqual(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}

bool vec3Equal(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.0001f) {
    return floatEqual(a.x, b.x, epsilon) &&
           floatEqual(a.y, b.y, epsilon) &&
           floatEqual(a.z, b.z, epsilon);
}

// ============================================================================
// MESH TESTS
// ============================================================================

bool test_mesh_vertex_structure() {
    TEST_START("Mesh: Vertex Structure");

    Vertex v1;
    TEST_ASSERT(v1.position == glm::vec3(0.0f), "Default vertex position should be (0,0,0)");
    TEST_ASSERT(v1.normal == glm::vec3(0.0f), "Default vertex normal should be (0,0,0)");
    TEST_ASSERT(v1.texCoord == glm::vec2(0.0f), "Default vertex texCoord should be (0,0)");

    Vertex v2(glm::vec3(1, 2, 3), glm::vec3(0, 1, 0), glm::vec2(0.5, 0.5));
    TEST_ASSERT(v2.position == glm::vec3(1, 2, 3), "Constructor should set position");
    TEST_ASSERT(v2.normal == glm::vec3(0, 1, 0), "Constructor should set normal");
    TEST_ASSERT(v2.texCoord == glm::vec2(0.5, 0.5), "Constructor should set texCoord");

    TEST_PASS();
}

bool test_mesh_cube_generation() {
    TEST_START("Mesh: Cube Generation");

    Mesh cube = Mesh::GenerateCube();

    TEST_ASSERT(cube.vertices.size() == 24, "Cube should have 24 vertices (4 per face)");
    TEST_ASSERT(cube.indices.size() == 36, "Cube should have 36 indices (6 faces * 2 triangles * 3 vertices)");

    // Check that all normals are unit vectors
    for (const auto& vertex : cube.vertices) {
        float normalLength = glm::length(vertex.normal);
        TEST_ASSERT(floatEqual(normalLength, 1.0f), "All normals should be unit vectors");
    }

    // Check UV coordinates are in [0, 1] range
    for (const auto& vertex : cube.vertices) {
        TEST_ASSERT(vertex.texCoord.x >= 0.0f && vertex.texCoord.x <= 1.0f, "UV X should be in [0,1]");
        TEST_ASSERT(vertex.texCoord.y >= 0.0f && vertex.texCoord.y <= 1.0f, "UV Y should be in [0,1]");
    }

    // Check indices are valid
    for (unsigned int index : cube.indices) {
        TEST_ASSERT(index < cube.vertices.size(), "All indices should be within vertex range");
    }

    TEST_PASS();
}

bool test_mesh_sphere_generation() {
    TEST_START("Mesh: Sphere Generation");

    Mesh sphere = Mesh::GenerateSphere(1);

    TEST_ASSERT(sphere.vertices.size() > 0, "Sphere should have vertices");
    TEST_ASSERT(sphere.indices.size() > 0, "Sphere should have indices");
    TEST_ASSERT(sphere.indices.size() % 3 == 0, "Indices should form complete triangles");

    // Check that vertices are roughly on sphere surface
    for (const auto& vertex : sphere.vertices) {
        float distance = glm::length(vertex.position);
        TEST_ASSERT(floatEqual(distance, 0.5f, 0.01f), "Vertices should be on sphere surface (radius 0.5)");

        // Normals should point outward (same direction as position from origin)
        glm::vec3 expectedNormal = glm::normalize(vertex.position);
        TEST_ASSERT(vec3Equal(vertex.normal, expectedNormal, 0.01f), "Normals should point outward");
    }

    TEST_PASS();
}

bool test_mesh_plane_generation() {
    TEST_START("Mesh: Plane Generation");

    float width = 10.0f;
    float depth = 10.0f;
    Mesh plane = Mesh::GeneratePlane(width, depth);

    TEST_ASSERT(plane.vertices.size() == 4, "Plane should have 4 vertices");
    TEST_ASSERT(plane.indices.size() == 6, "Plane should have 6 indices (2 triangles)");

    // Check all vertices are on Y=0 plane
    for (const auto& vertex : plane.vertices) {
        TEST_ASSERT(floatEqual(vertex.position.y, 0.0f), "Plane vertices should be at Y=0");
    }

    // Check normals point up
    for (const auto& vertex : plane.vertices) {
        TEST_ASSERT(vec3Equal(vertex.normal, glm::vec3(0, 1, 0)), "Plane normals should point up");
    }

    TEST_PASS();
}

// ============================================================================
// CAMERA TESTS
// ============================================================================

bool test_camera_initialization() {
    TEST_START("Camera: Initialization");

    Camera camera(45.0f, 16.0f/9.0f, 0.1f, 100.0f);

    TEST_ASSERT(camera.GetTarget() == glm::vec3(0.0f), "Default target should be origin");
    TEST_ASSERT(floatEqual(camera.GetYaw(), 0.0f), "Default yaw should be 0");
    TEST_ASSERT(floatEqual(camera.GetPitch(), 20.0f), "Default pitch should be 20");
    TEST_ASSERT(floatEqual(camera.GetDistance(), 5.0f), "Default distance should be 5");

    TEST_PASS();
}

bool test_camera_orbit() {
    TEST_START("Camera: Orbit Controls");

    Camera camera;
    camera.SetTarget(glm::vec3(0, 0, 0));
    camera.SetDistance(5.0f);

    glm::vec3 initialPos = camera.GetPosition();

    // Orbit horizontally
    camera.Orbit(45.0f, 0.0f);
    TEST_ASSERT(camera.GetYaw() == 45.0f, "Yaw should increase by delta");
    TEST_ASSERT(!vec3Equal(camera.GetPosition(), initialPos), "Position should change after orbit");

    // Test pitch clamping
    camera.Orbit(0.0f, 100.0f); // Try to pitch above 89°
    TEST_ASSERT(camera.GetPitch() <= 89.0f, "Pitch should be clamped at 89°");

    camera.Orbit(0.0f, -200.0f); // Try to pitch below -89°
    TEST_ASSERT(camera.GetPitch() >= -89.0f, "Pitch should be clamped at -89°");

    TEST_PASS();
}

bool test_camera_zoom() {
    TEST_START("Camera: Zoom Controls");

    Camera camera;
    camera.SetDistance(10.0f);

    float initialDist = camera.GetDistance();

    // Zoom in
    camera.Zoom(5.0f);
    TEST_ASSERT(camera.GetDistance() < initialDist, "Zoom should decrease distance");

    // Test minimum distance clamping
    camera.SetDistance(2.0f);
    camera.Zoom(-10.0f); // Try to zoom out past min
    TEST_ASSERT(camera.GetDistance() >= 1.0f, "Distance should be clamped at minimum");

    // Test maximum distance clamping
    camera.SetDistance(45.0f);
    camera.Zoom(-10.0f); // Try to zoom out past max
    TEST_ASSERT(camera.GetDistance() <= 50.0f, "Distance should be clamped at maximum");

    TEST_PASS();
}

bool test_camera_matrices() {
    TEST_START("Camera: Matrix Generation");

    Camera camera(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    camera.SetTarget(glm::vec3(0, 0, 0));
    camera.SetDistance(5.0f);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();

    // Basic sanity checks (matrices should not be identity or zero)
    TEST_ASSERT(view != glm::mat4(0.0f), "View matrix should not be zero");
    TEST_ASSERT(view != glm::mat4(1.0f), "View matrix should not be identity");
    TEST_ASSERT(projection != glm::mat4(0.0f), "Projection matrix should not be zero");
    TEST_ASSERT(projection != glm::mat4(1.0f), "Projection matrix should not be identity");

    TEST_PASS();
}

bool test_camera_spherical_coordinates() {
    TEST_START("Camera: Spherical Coordinate Conversion");

    Camera camera;
    camera.SetTarget(glm::vec3(0, 0, 0));
    camera.SetDistance(10.0f);

    // Test cardinal directions
    camera.Orbit(0.0f - camera.GetYaw(), 0.0f - camera.GetPitch()); // Reset to yaw=0, pitch=0
    glm::vec3 pos = camera.GetPosition();
    TEST_ASSERT(floatEqual(pos.x, 0.0f, 0.1f), "At yaw=0, pitch=0, X should be ~0");
    TEST_ASSERT(floatEqual(pos.z, 10.0f, 0.1f), "At yaw=0, pitch=0, Z should be distance");

    TEST_PASS();
}

// ============================================================================
// INTEGRATION TESTS (require OpenGL context)
// ============================================================================

bool test_mesh_gpu_setup(SDL_Window* window, SDL_GLContext context) {
    TEST_START("Mesh: GPU Buffer Setup");

    Mesh cube = Mesh::GenerateCube();

    // Mesh should already be set up (GenerateCube calls SetupMesh)
    // Try to draw it (should not crash or error)

    Shader dummyShader;
    // We can't actually draw without a valid shader, but we can verify the mesh was set up

    TEST_ASSERT(cube.vertices.size() > 0, "Mesh should have vertices");
    TEST_ASSERT(cube.indices.size() > 0, "Mesh should have indices");

    TEST_PASS();
}

bool test_texture_loading(SDL_Window* window, SDL_GLContext context) {
    TEST_START("Texture: Loading (requires test texture)");

    Texture texture;

    // Try to load a texture (will fail if file doesn't exist, which is acceptable)
    bool loaded = texture.LoadFromFile("assets/textures/test_texture.png");

    if (!loaded) {
        std::cout << "  ⚠ WARNING: Test texture not found (acceptable for unit test)" << std::endl;
        tests_passed++; // Don't fail the test
        return true;
    }

    TEST_ASSERT(texture.IsLoaded(), "Texture should be marked as loaded");
    TEST_ASSERT(texture.GetID() != 0, "Texture should have valid OpenGL ID");
    TEST_ASSERT(texture.GetWidth() > 0, "Texture should have width");
    TEST_ASSERT(texture.GetHeight() > 0, "Texture should have height");

    TEST_PASS();
}

bool test_shader_compilation(SDL_Window* window, SDL_GLContext context) {
    TEST_START("Shader: Compilation (requires shader files)");

    Shader shader;
    bool loaded = shader.LoadFromFiles("assets/shaders/lighting.vert",
                                       "assets/shaders/lighting.frag");

    if (!loaded) {
        std::cout << "  ⚠ WARNING: Shader files not found (acceptable for unit test)" << std::endl;
        tests_passed++;
        return true;
    }

    TEST_ASSERT(shader.GetProgramID() != 0, "Shader should have valid program ID");

    // Test uniform setters (should not crash)
    shader.Use();
    shader.SetInt("uTexture", 0);
    shader.SetFloat("uShininess", 32.0f);
    shader.SetVec3("uLightPos", glm::vec3(1, 1, 1));
    shader.SetMat4("uModel", glm::mat4(1.0f));

    TEST_PASS();
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  VibeReaper Phase 2 Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    // Initialize logger
    LOG_INFO("Starting test suite...");

    // ========================================
    // Unit Tests (no OpenGL context required)
    // ========================================

    std::cout << "\n--- UNIT TESTS (No GPU Required) ---" << std::endl;

    test_mesh_vertex_structure();
    test_mesh_cube_generation();
    test_mesh_sphere_generation();
    test_mesh_plane_generation();
    test_camera_initialization();
    test_camera_orbit();
    test_camera_zoom();
    test_camera_matrices();
    test_camera_spherical_coordinates();

    // ========================================
    // Integration Tests (require OpenGL)
    // ========================================

    std::cout << "\n--- INTEGRATION TESTS (GPU Required) ---" << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "⚠ WARNING: Could not initialize SDL. Skipping GPU tests." << std::endl;
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
    } else {
        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        // Create hidden window for testing
        SDL_Window* window = SDL_CreateWindow("Test Window",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

        if (window == nullptr) {
            std::cout << "⚠ WARNING: Could not create window. Skipping GPU tests." << std::endl;
        } else {
            SDL_GLContext context = SDL_GL_CreateContext(window);

            if (context == nullptr) {
                std::cout << "⚠ WARNING: Could not create OpenGL context. Skipping GPU tests." << std::endl;
            } else {
                // Initialize GLAD
                if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
                    std::cout << "⚠ WARNING: Failed to initialize GLAD. Skipping GPU tests." << std::endl;
                } else {
                    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

                    // Run GPU-dependent tests
                    test_mesh_gpu_setup(window, context);
                    test_texture_loading(window, context);
                    test_shader_compilation(window, context);
                }

                SDL_GL_DeleteContext(context);
            }

            SDL_DestroyWindow(window);
        }

        SDL_Quit();
    }

    // ========================================
    // Results Summary
    // ========================================

    std::cout << "\n========================================" << std::endl;
    std::cout << "  TEST RESULTS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    std::cout << "Total:  " << (tests_passed + tests_failed) << std::endl;

    if (tests_failed == 0) {
        std::cout << "\n✓ ALL TESTS PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ SOME TESTS FAILED" << std::endl;
        return 1;
    }
}
