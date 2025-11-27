#version 330 core

// Input from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Output color
out vec4 FragColor;

// Uniform for testing
uniform vec3 uColor;  // Test color

void main() {
    // For Phase 1: Simple solid color output
    // This will be extended in Phase 2 with textures and lighting
    FragColor = vec4(uColor, 1.0);
}
