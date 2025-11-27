#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// Material
uniform sampler2D uTexture;
uniform vec3 uColor;

// Lighting
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

// Lighting parameters
uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform float uShininess;

void main() {
    // Sample texture
    vec3 textureColor = texture(uTexture, TexCoord).rgb * uColor;
    
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    vec3 viewDir = normalize(uViewPos - FragPos);
    
    // Ambient
    vec3 ambient = uAmbientStrength * uLightColor;
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), uShininess);
    vec3 specular = uSpecularStrength * spec * uLightColor;
    
    // Combine lighting
    vec3 result = (ambient + diffuse + specular) * textureColor;
    
    FragColor = vec4(result, 1.0);
}
