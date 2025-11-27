# Phase 2: Basic 3D Rendering - Educational Guide

Welcome to Phase 2 of VibeReaper development! This guide explains the concepts and implementation details for rendering textured, lit 3D objects.

---

## Table of Contents

1. [Overview](#overview)
2. [3D Mesh Rendering](#3d-mesh-rendering)
3. [Texture Mapping](#texture-mapping)
4. [Camera Systems](#camera-systems)
5. [Phong Lighting](#phong-lighting)
6. [Implementation Walkthrough](#implementation-walkthrough)

---

## Overview

### What We're Building

In Phase 2, we'll add:
- **Mesh System**: Render 3D geometry (cubes, spheres, planes)
- **Texture System**: Apply images to 3D surfaces
- **Camera System**: Third-person orbital camera
- **Lighting**: Phong lighting model for realistic shading

### End Goal

A spinning, textured cube with proper lighting that you can orbit around with your mouse.

---

## 3D Mesh Rendering

### What is a Mesh?

A **mesh** is a collection of:
- **Vertices**: 3D points in space (x, y, z)
- **Normals**: Direction each vertex "faces" (for lighting)
- **Texture Coordinates (UVs)**: Map texture pixels to 3D surface
- **Indices**: Define which vertices form triangles

### Vertex Structure

```cpp
struct Vertex {
    glm::vec3 position;    // 3D coordinates
    glm::vec3 normal;      // Surface direction
    glm::vec2 texCoord;    // Texture UV (0-1 range)
};
```

**Example - Single vertex of a cube:**
```cpp
Vertex v;
v.position = glm::vec3(0.5f, 0.5f, 0.5f);   // Top-right-front corner
v.normal = glm::vec3(0.0f, 1.0f, 0.0f);     // Pointing up
v.texCoord = glm::vec2(1.0f, 1.0f);         // Top-right of texture
```

### OpenGL Buffer Objects

**VAO (Vertex Array Object)**
- Stores configuration of vertex attributes
- "Remembers" how to interpret VBO data

**VBO (Vertex Buffer Object)**
- Stores actual vertex data on GPU
- Uploaded once, used many times

**EBO (Element Buffer Object)**
- Stores indices to avoid duplicate vertices
- A cube has 8 vertices but 36 indices (6 faces × 2 triangles × 3 vertices)

### Procedural Cube Generation

Instead of loading from a file, we generate a cube in code:

```cpp
std::vector<Vertex> GenerateCube() {
    std::vector<Vertex> vertices;
    
    // Front face (4 vertices)
    vertices.push_back({ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} });
    vertices.push_back({ { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f} });
    vertices.push_back({ { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f} });
    vertices.push_back({ {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} });
    // ... repeat for all 6 faces
    
    return vertices;
}
```

**Why 24 vertices instead of 8?**  
Each corner of a cube belongs to 3 faces with different normals and UVs.

---

## Texture Mapping

### What is Texture Mapping?

**Texture mapping** applies a 2D image onto a 3D surface. Think of wrapping a present with wrapping paper.

### UV Coordinates

UVs map a point on the 3D model to a pixel on the texture:
- **U**: Horizontal axis (0.0 = left, 1.0 = right)
- **V**: Vertical axis (0.0 = bottom, 1.0 = top)

```
Texture (512x512px)
┌──────────────┐ (1.0, 1.0)
│  Image Data  │
│              │
│              │
└──────────────┘
(0.0, 0.0)
```

### Loading Textures with stb_image

**stb_image** is a single-header library that loads PNG, JPG, TGA, BMP:

```cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int width, height, channels;
unsigned char* data = stbi_load("texture.png", &width, &height, &channels, 0);

if (data) {
    // Upload to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}
```

### Texture Parameters

**Filtering** (how texture is sampled):
- `GL_NEAREST`: Pixelated (good for retro look)
- `GL_LINEAR`: Smooth (better for realistic graphics)

**Wrapping** (what happens outside 0-1 range):
- `GL_REPEAT`: Tile the texture
- `GL_CLAMP_TO_EDGE`: Stretch edge pixels
- `GL_MIRRORED_REPEAT`: Mirror and tile

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
```

### Mipmaps

**Mipmaps** are pre-calculated smaller versions of a texture for distant objects:
- Level 0: 1024x1024
- Level 1: 512x512
- Level 2: 256x256
- ...

```cpp
glGenerateMipmap(GL_TEXTURE_2D);  // Auto-generate mipmap chain
```

---

## Camera Systems

### Third-Person Camera

A third-person camera:
- Orbits around a target point
- Maintains a fixed distance from the target
- Follows the target smoothly

### Key Components

**Position**: Where the camera is in 3D space  
**Target**: What the camera is looking at  
**Up Vector**: Which direction is "up" (usually `0, 1, 0`)

### View Matrix

The **view matrix** transforms world coordinates to camera/view space:

```cpp
glm::mat4 view = glm::lookAt(
    cameraPosition,  // Where the camera is
    target,          // What it's looking at
    glm::vec3(0, 1, 0)  // Up direction
);
```

**How it works:**  
Imagine the camera is at the origin (0, 0, 0) looking down the -Z axis. The view matrix transforms the entire world so that becomes true.

### Projection Matrix

The **projection matrix** defines the camera's perspective:

```cpp
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),  // Field of view (FOV)
    aspectRatio,           // Width / height
    0.1f,                  // Near clipping plane
    100.0f                 // Far clipping plane
);
```

**Field of View (FOV):**
- 45° = Natural, modest perspective
- 60-90° = Wide angle (FPS games)
- 30° = Telephoto (zoomed in)

**Clipping Planes:**
- **Near**: Objects closer than this aren't rendered
- **Far**: Objects farther than this aren't rendered

### Orbital Camera Controls

To orbit the camera around a target using mouse input:

```cpp
void OrbitCamera(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    pitch += deltaPitch;
    
    // Limit pitch to avoid flipping
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    
    // Calculate position from spherical coordinates
    float x = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    float y = distance * sin(glm::radians(pitch));
    float z = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    
    position = target + glm::vec3(x, y, z);
}
```

**Spherical Coordinates:**
- **Yaw**: Rotation around Y-axis (left/right)
- **Pitch**: Rotation around X-axis (up/down)
- **Distance**: How far from target

---

## Phong Lighting

### What is Phong Lighting?

The **Phong lighting model** simulates how light interacts with surfaces. It has three components:

1. **Ambient**: Base light (even in shadow)
2. **Diffuse**: Main directional light
3. **Specular**: Shiny highlights

### Ambient Light

Constant light that illuminates everything equally:

```glsl
vec3 ambient = ambientStrength * lightColor;
```

**Example:** `ambientStrength = 0.1` means surfaces are 10% lit even in complete shadow.

### Diffuse Light

Light that depends on angle between surface normal and light direction:

```glsl
vec3 norm = normalize(normal);
vec3 lightDir = normalize(lightPos - fragPos);
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;
```

**How it works:**
- Surface facing light directly: `dot = 1.0` (100% lit)
- Surface perpendicular to light: `dot = 0.0` (0% lit)
- Surface facing away: `dot < 0.0` → clamped to 0

### Specular Light

Shiny reflections (Blinn-Phong variation):

```glsl
vec3 viewDir = normalize(viewPos - fragPos);
vec3 halfwayDir = normalize(lightDir + viewDir);
float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
vec3 specular = specularStrength * spec * lightColor;
```

**Shininess:**
- Low (4-8): Dull, matte surfaces (wood, stone)
- Medium (16-32): Plastic
- High (64-128): Metal, polished surfaces

### Complete Phong Shader

```glsl
// Fragment Shader
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D texture1;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Combine
    vec3 textureColor = texture(texture1, TexCoord).rgb;
    vec3 result = (ambient + diffuse + specular) * textureColor;
    FragColor = vec4(result, 1.0);
}
```

---

## Implementation Walkthrough

### Step 1: Create the Mesh Class

**Goal:** Render a procedurally-generated cube

**Key Files:**
- `src/Engine/Mesh.h`
- `src/Engine/Mesh.cpp`

**What to implement:**
1. `Vertex` struct with position, normal, texCoord
2. VAO/VBO/EBO setup in `SetupMesh()`
3. `Draw(Shader& shader)` to render
4. `GenerateCube()` static method

**Test:** Render a white cube with solid color (no texture yet)

---

### Step 2: Add Texture Loading

**Goal:** Apply a texture to the cube

**Key Files:**
- Download `stb_image.h` to `lib/stb/`
- `src/Engine/Texture.h`
- `src/Engine/Texture.cpp`

**What to implement:**
1. Load image with `stbi_load()`
2. Generate OpenGL texture with `glGenTextures()`
3. Upload image data with `glTexImage2D()`
4. Set filtering and wrapping modes
5. `Bind(int textureUnit)` method

**Test:** Render cube with checkerboard or test texture

---

### Step 3: Implement Camera

**Goal:** Orbit around the cube with mouse

**Key Files:**
- `src/Engine/Camera.h`
- `src/Engine/Camera.cpp`

**What to implement:**
1. Position, target, yaw, pitch, distance
2. `GetViewMatrix()` using `glm::lookAt()`
3. `GetProjectionMatrix(float aspect)` using `glm::perspective()`
4. `Orbit(float deltaYaw, float deltaPitch)`
5. Mouse input handling in main.cpp

**Test:** Camera orbits smoothly, no gimbal lock

---

### Step 4: Add Phong Lighting

**Goal:** Light the cube realistically

**Key Files:**
- `assets/shaders/lighting.vert`
- `assets/shaders/lighting.frag`

**What to implement:**
1. Vertex shader: pass world-space position and normal
2. Fragment shader: Phong ambient + diffuse + specular
3. Set light uniforms (position, color)
4. Set view position uniform (camera position)

**Test:** Cube faces facing light are brighter, specular highlights visible

---

### Step 5: Integration

**Goal:** Put everything together

**Update in main.cpp:**
1. Create mesh (cube)
2. Load texture
3. Create camera
4. Load lighting shader
5. Render loop:
   - Update camera
   - Rotate cube
   - Set shader uniforms
   - Draw mesh

**Test:** Textured, lit, rotating cube with camera orbit

---

## Common Issues & Solutions

### Issue: Cube appears black
**Cause:** Normals pointing wrong direction  
**Solution:** Ensure normals are calculated correctly for each face

### Issue: Texture appears stretched or distorted
**Cause:** Incorrect UV coordinates  
**Solution:** Check UV mapping, use checkerboard texture to debug

### Issue: Camera flips upside down
**Cause:** Gimbal lock when pitch exceeds ±90°  
**Solution:** Clamp pitch to [-89°, 89°]

### Issue: Nothing renders
**Cause:** Matrices not uploaded to shader  
**Solution:** Call `shader.SetMat4()` for model, view, projection

### Issue: Lighting looks flat
**Cause:** Normals not transformed to world space  
**Solution:** Use `transpose(inverse(model))` for normal matrix

---

## Next Steps

After completing Phase 2, you'll have:
- ✅ Mesh rendering system
- ✅ Texture loading
- ✅ Third-person camera
- ✅ Phong lighting

**Phase 3** will add:
- TrenchBroom integration
- MAP file parser
- Level geometry rendering

---

## Additional Resources

**OpenGL Tutorials:**
- [LearnOpenGL - Textures](https://learnopengl.com/Getting-started/Textures)
- [LearnOpenGL - Camera](https://learnopengl.com/Getting-started/Camera)
- [LearnOpenGL - Basic Lighting](https://learnopengl.com/Lighting/Basic-Lighting)

**GLM Documentation:**
- [GLM Manual](https://glm.g-truc.net/0.9.9/api/index.html)

**stb_image:**
- [GitHub Repository](https://github.com/nothings/stb)

---

## Summary

Phase 2 introduces the fundamentals of 3D rendering:

- **Meshes**: Collections of vertices forming 3D shapes
- **Textures**: Images applied to surfaces via UV mapping
- **Cameras**: View and projection transformations
- **Lighting**: Phong model for realistic shading

Master these concepts, and you'll be ready to render complex 3D scenes in Phase 3!
