# Phase 1: Engine Foundation - Educational Guide

This guide explains every step of implementing the engine foundation for VibeReaper. Use this to understand what's being done and why.

---

## Overview: What Are We Building?

Phase 1 creates the **foundation** of our game engine. Think of it as building the skeleton that everything else will attach to. We need:

1. **Build System (CMake)** - Tells the compiler how to build our project
2. **Window & Graphics Context (SDL2 + OpenGL)** - Creates a window and sets up 3D rendering
3. **Shader System** - Programs that run on the GPU to render graphics
4. **Renderer** - Manages rendering state and operations
5. **Logger** - Helps us debug by printing messages
6. **Main Loop** - The heartbeat of the game (update → render → repeat)

---

## Part 1: Understanding Third-Party Libraries

### What is SDL2?
**Simple DirectMedia Layer** is a cross-platform library that handles:
- Window creation
- Input (keyboard, mouse, gamepad)
- Audio playback
- Event handling

**Why?** We don't want to write platform-specific code for Windows/Linux/Mac. SDL2 abstracts this away.

### What is OpenGL?
A graphics API for rendering 2D and 3D graphics. It talks to your GPU (graphics card) to draw triangles, textures, and entire 3D worlds.

**Why OpenGL 3.3+?** Modern enough for features we need, old enough to run on most hardware.

### What is GLAD?
A loader for OpenGL functions. OpenGL functions need to be loaded at runtime on modern systems. GLAD generates code to do this automatically.

**Why?** Without GLAD, we'd have to manually load hundreds of OpenGL functions. GLAD does it for us.

### What is GLM?
**OpenGL Mathematics** - A header-only C++ library for vector and matrix math. It provides:
- `vec3` (3D vectors for positions, directions)
- `vec4` (4D vectors for colors, homogeneous coords)
- `mat4` (4x4 matrices for transformations)
- Math functions (dot product, cross product, matrix multiplication)

**Why?** 3D graphics require heavy math. GLM makes it easy and matches OpenGL's conventions.

---

## Part 2: The Build System (CMake)

### What is CMake?
A build system generator. You write `CMakeLists.txt` that describes your project, and CMake generates platform-specific build files (Visual Studio projects on Windows, Makefiles on Linux).

### Why Use CMake?
- Cross-platform (Windows, Linux, Mac)
- Industry standard
- Handles dependencies automatically
- Supports multiple compilers

### What Does Our CMakeLists.txt Do?

```cmake
cmake_minimum_required(VERSION 3.15)
```
Specifies minimum CMake version needed.

```cmake
project(VibeReaper VERSION 1.0.0 LANGUAGES CXX)
```
Names our project and sets version.

```cmake
set(CMAKE_CXX_STANDARD 17)
```
Uses C++17 features (auto, structured bindings, std::filesystem, etc.)

```cmake
find_package(SDL2 REQUIRED)
```
Finds SDL2 on your system. If not found, build fails (REQUIRED).

```cmake
add_executable(VibeReaper ${SOURCES})
```
Creates an executable from our source files.

```cmake
target_link_libraries(VibeReaper SDL2::SDL2 OpenGL::GL)
```
Links SDL2 and OpenGL libraries to our executable.

---

## Part 3: The Main Loop

### Game Loop Architecture

Every game has a main loop that runs ~60 times per second:

```
Initialize SDL & OpenGL
↓
LOOP:
  1. Handle Events (input, window close, etc.)
  2. Update (game logic, physics, AI)
  3. Render (draw to screen)
  4. Calculate delta time (time since last frame)
UNTIL user quits
↓
Cleanup & shutdown
```

### Why Delta Time?

```cpp
float deltaTime = (currentTime - lastTime) / 1000.0f;
```

Delta time is the time elapsed since the last frame. This makes our game frame-rate independent.

**Example:**
- If player moves at 5 units/second
- Frame 1: 16ms elapsed → move 5 * 0.016 = 0.08 units
- Frame 2: 20ms elapsed → move 5 * 0.020 = 0.10 units

Without delta time, movement speed would vary based on frame rate!

### SDL2 Initialization

```cpp
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
```

Initializes SDL subsystems:
- `SDL_INIT_VIDEO` - Window and graphics
- `SDL_INIT_AUDIO` - Sound playback
- `SDL_INIT_GAMECONTROLLER` - Gamepad support

### OpenGL Context Creation

```cpp
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
```

This requests an **OpenGL 3.3 Core Profile** context:
- **3.3** - Minimum version with modern features
- **Core Profile** - No deprecated legacy functions (cleaner, modern OpenGL)

---

## Part 4: The Shader System

### What Are Shaders?

Shaders are **programs that run on the GPU**. They process vertices and pixels in parallel.

**Two Main Types:**
1. **Vertex Shader** - Processes each vertex (transforms position, passes data to fragment shader)
2. **Fragment Shader** - Processes each pixel/fragment (determines final color)

### Shader Pipeline Flow

```
3D Model (vertices) 
    ↓
Vertex Shader (transform to 2D screen space)
    ↓
Rasterization (GPU converts triangles to pixels)
    ↓
Fragment Shader (determine each pixel's color)
    ↓
Frame Buffer (final image on screen)
```

### GLSL - OpenGL Shading Language

Shaders are written in GLSL, a C-like language.

**Vertex Shader Example:**
```glsl
#version 330 core
layout(location = 0) in vec3 aPos;  // Input: vertex position

uniform mat4 uMVP;  // Model-View-Projection matrix

void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);  // Transform vertex
}
```

**Fragment Shader Example:**
```glsl
#version 330 core
out vec4 FragColor;  // Output: final pixel color

void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);  // Orange color
}
```

### Shader Compilation Process

1. **Load** shader source code from file (`.vert`, `.frag`)
2. **Compile** shader using `glCompileShader()`
3. **Check for errors** (syntax errors, unsupported features)
4. **Link** vertex + fragment shaders into a shader program
5. **Use** the program during rendering

### Uniforms - Passing Data to Shaders

Uniforms are variables you set from C++ code that remain constant for all vertices/pixels in a draw call.

**Common Uniforms:**
- `mat4 uMVP` - Transformation matrix (Model-View-Projection)
- `vec3 uLightPos` - Light position
- `sampler2D uTexture` - Texture data

**Setting Uniforms from C++:**
```cpp
shader.Use();
shader.SetMat4("uMVP", transformMatrix);
shader.SetVec3("uLightPos", glm::vec3(5.0, 10.0, 5.0));
```

---

## Part 5: The Renderer

### What Does the Renderer Do?

The Renderer manages global OpenGL state and provides high-level rendering operations.

**Key Responsibilities:**
- Initialize OpenGL settings (depth testing, face culling, blending)
- Clear buffers (color buffer, depth buffer)
- Set viewport size
- Toggle wireframe mode (for debugging)
- Enable/disable V-Sync

### OpenGL State Machine

OpenGL is a **state machine**. You set states that remain active until changed:

```cpp
glEnable(GL_DEPTH_TEST);      // Enable depth testing (3D occlusion)
glEnable(GL_CULL_FACE);       // Don't draw back-facing triangles
glCullFace(GL_BACK);          // Cull back faces
glDepthFunc(GL_LESS);         // Depth test passes if closer
```

### Clearing Buffers

Every frame, we clear buffers before rendering:

```cpp
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

- **Color Buffer** - Stores pixel colors
- **Depth Buffer** - Stores depth values (for 3D occlusion - "which object is in front?")

### Viewport

Defines the region of the window where OpenGL draws:

```cpp
glViewport(0, 0, windowWidth, windowHeight);
```

This should be updated when the window is resized.

---

## Part 6: The Logger

### Why Do We Need a Logger?

During development, we need to:
- Track program flow ("Initialized SDL successfully")
- Debug issues ("Failed to load texture: file not found")
- Monitor performance ("Frame time: 16.7ms")

**Print statements aren't enough** because:
- No timestamps
- No log levels (can't filter by severity)
- Hard to disable in release builds

### Log Levels

```cpp
enum class LogLevel {
    DEBUG,    // Detailed info for debugging
    INFO,     // General information
    WARNING,  // Something unexpected but not critical
    ERROR     // Critical failure
};
```

**Example Usage:**
```cpp
LOG_INFO("SDL2 initialized successfully");
LOG_WARNING("Texture not found, using default");
LOG_ERROR("Failed to compile shader: " + errorMsg);
```

### Timestamp Formatting

Logs include timestamps for debugging:
```
[2025-11-26 20:48:15] [INFO] SDL2 initialized
[2025-11-26 20:48:15] [INFO] OpenGL 3.3 context created
[2025-11-26 20:48:15] [ERROR] Failed to load shader: basic.vert
```

---

## Part 7: Project Structure

### Why This Structure?

```
src/
├── main.cpp              # Entry point
├── Engine/               # Engine systems (reusable, game-agnostic)
│   ├── Renderer.h/cpp
│   ├── Shader.h/cpp
│   └── ...
├── Game/                 # Game-specific logic
│   ├── Player.h/cpp
│   └── ...
└── Utils/                # Utilities (Logger, Math helpers)
    └── Logger.h/cpp
```

**Benefits:**
- **Separation of Concerns** - Engine code separate from game logic
- **Reusability** - Engine code could be used in other projects
- **Maintainability** - Easy to find and modify specific systems

---

## Part 8: Building and Running

### Build Commands (Windows)

```powershell
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project (Release mode for performance)
cmake --build . --config Release

# Run the game
.\Release\VibeReaper.exe
```

### What Happens During Build?

1. **CMake Configuration** - Finds SDL2, creates Visual Studio project files
2. **Compilation** - Each `.cpp` file compiled to `.obj` (object file)
3. **Linking** - All `.obj` files and libraries linked into `VibeReaper.exe`
4. **Asset Copying** - `assets/` folder copied to build directory

---

## Part 9: What Success Looks Like

After Phase 1, you should see:

1. **Window Opens** - A window titled "VibeReaper" appears
2. **Clear Color** - Background is a solid color (we'll set dark blue/black)
3. **Console Output:**
   ```
   [INFO] SDL2 initialized successfully
   [INFO] OpenGL 3.3 context created
   [INFO] Renderer: NVIDIA GeForce RTX 3070
   [INFO] GLAD loaded OpenGL functions
   [INFO] FPS: 60
   ```
4. **Responsive** - Window can be closed, no crashes

---

## Part 10: Common Issues and Troubleshooting

### "SDL2 not found"
- Install SDL2 development libraries
- Set `SDL2_DIR` environment variable to SDL2 install path

### "GLAD initialization failed"
- OpenGL context must be created BEFORE calling `gladLoadGLLoader()`

### Black screen
- Check if shaders compiled successfully
- Verify OpenGL context is current
- Check for OpenGL errors with `glGetError()`

### Low FPS
- Enable V-Sync initially (limits to monitor refresh rate)
- Check for infinite loops in Update()

---

## Next Steps (After Phase 1)

Phase 2 will add:
- **Mesh system** - Rendering 3D geometry (cubes, spheres)
- **Texture loading** - Reading images and applying to models
- **Camera system** - Third-person camera following player
- **Lighting shaders** - Phong lighting for realistic visuals

---

## Learning Resources

### SDL2
- [Lazy Foo's SDL2 Tutorials](https://lazyfoo.net/tutorials/SDL/)
- [SDL2 Official Documentation](https://wiki.libsdl.org/)

### OpenGL
- [LearnOpenGL.com](https://learnopengl.com/) - **Highly recommended**
- [OpenGL 3.3 Reference](https://www.khronos.org/opengl/wiki/)

### GLM
- [GLM Documentation](https://glm.g-truc.net/0.9.9/index.html)

### CMake
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/)

---

## Summary

Phase 1 establishes:
✅ Build system (CMake)
✅ Window creation (SDL2)
✅ OpenGL context and loader (GLAD)
✅ Shader system (load, compile, link)
✅ Basic renderer (clear, viewport, state management)
✅ Logger (timestamped, leveled logging)
✅ Main loop (event, update, render, delta time)

This is the foundation everything else builds upon. Take your time understanding each piece—it's the most important phase!
