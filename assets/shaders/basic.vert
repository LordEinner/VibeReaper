#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 aPos;       // Position
layout(location = 1) in vec3 aNormal;    // Normal
layout(location = 2) in vec2 aTexCoord;  // Texture coordinates

// Uniforms (set from C++ code)
uniform mat4 uModel;        // Model matrix
uniform mat4 uView;         // View matrix
uniform mat4 uProjection;   // Projection matrix

// Output to fragment shader
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {
    // Transform vertex position to world space
    FragPos = vec3(uModel * vec4(aPos, 1.0));
    
    // Transform normal to world space
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    
    // Pass texture coordinates to fragment shader
    TexCoord = aTexCoord;
    
    // Transform vertex to clip space
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
