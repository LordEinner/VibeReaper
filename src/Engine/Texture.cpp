#include "Texture.h"
#include "../Utils/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace VibeReaper {

    Texture::Texture()
        : textureID(0), width(0), height(0), channels(0), loaded(false) {
    }

    Texture::~Texture() {
        Cleanup();
    }

    Texture::Texture(Texture&& other) noexcept
        : textureID(other.textureID),
          width(other.width),
          height(other.height),
          channels(other.channels),
          loaded(other.loaded) {
        
        // Reset other
        other.textureID = 0;
        other.width = 0;
        other.height = 0;
        other.channels = 0;
        other.loaded = false;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            Cleanup(); // Clean up existing resources

            textureID = other.textureID;
            width = other.width;
            height = other.height;
            channels = other.channels;
            loaded = other.loaded;

            // Reset other
            other.textureID = 0;
            other.width = 0;
            other.height = 0;
            other.channels = 0;
            other.loaded = false;
        }
        return *this;
    }

    bool Texture::LoadFromFile(const std::string& path) {
        if (loaded) {
            LOG_WARNING("Texture already loaded, cleaning up first");
            Cleanup();
        }

        // Load image using stb_image
        stbi_set_flip_vertically_on_load(true); // OpenGL expects texture origin at bottom-left
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!data) {
            LOG_ERROR("Failed to load texture: " + path);
            return false;
        }

        // Determine format based on number of channels
        GLenum format = GL_RGB;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;

        // Generate OpenGL texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        // Generate mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Free image data
        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);

        loaded = true;
        LOG_INFO("Texture loaded: " + path + " (" + std::to_string(width) + "x" + 
                 std::to_string(height) + ", " + std::to_string(channels) + " channels)");

        return true;
    }

    void Texture::CreateWhiteTexture() {
        if (loaded) {
            Cleanup();
        }

        width = 1;
        height = 1;
        channels = 4;

        unsigned char data[] = { 255, 255, 255, 255 };

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);

        loaded = true;
        LOG_INFO("Created fallback white texture");
    }

    void Texture::Bind(int textureUnit) {
        if (!loaded) {
            LOG_ERROR("Cannot bind texture that hasn't been loaded");
            return;
        }

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void Texture::Unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Cleanup() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
        loaded = false;
    }

} // namespace VibeReaper
