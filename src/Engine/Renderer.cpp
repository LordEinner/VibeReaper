#include "Renderer.h"
#include "../Utils/Logger.h"

namespace VibeReaper {

Renderer::Renderer() 
    : m_wireframeMode(false), m_vsyncEnabled(true) {
    // Default clear color: dark blue/black
    m_clearColor[0] = 0.05f;
    m_clearColor[1] = 0.05f;
    m_clearColor[2] = 0.1f;
    m_clearColor[3] = 1.0f;
}

Renderer::~Renderer() {
}

bool Renderer::Initialize() {
    // Enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable face culling (don't render back faces)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW); // Counter-clockwise winding order for front faces

    // Set clear color
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOG_ERROR("OpenGL initialization error: " + std::to_string(error));
        return false;
    }

    LOG_INFO("Renderer initialized successfully");
    return true;
}

void Renderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetClearColor(float r, float g, float b, float a) {
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
    glClearColor(r, g, b, a);
}

void Renderer::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Renderer::SetWireframeMode(bool enabled) {
    m_wireframeMode = enabled;
    if (enabled) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer::SetVSync(bool enabled) {
    m_vsyncEnabled = enabled;
    SDL_GL_SetSwapInterval(enabled ? 1 : 0);
    LOG_INFO(std::string("VSync ") + (enabled ? "enabled" : "disabled"));
}

void Renderer::SwapBuffers(SDL_Window* window) {
    SDL_GL_SwapWindow(window);
}

} // namespace VibeReaper
