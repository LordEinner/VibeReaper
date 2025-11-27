#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>

namespace VibeReaper {

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Initialize OpenGL state
    bool Initialize();

    // Clear buffers
    void Clear();

    // Set clear color
    void SetClearColor(float r, float g, float b, float a);

    // Set viewport
    void SetViewport(int x, int y, int width, int height);

    // Toggle wireframe mode
    void SetWireframeMode(bool enabled);

    // Toggle VSync
    void SetVSync(bool enabled);

    // Swap buffers (call after rendering)
    void SwapBuffers(SDL_Window* window);

private:
    float m_clearColor[4];
    bool m_wireframeMode;
    bool m_vsyncEnabled;
};

} // namespace VibeReaper
