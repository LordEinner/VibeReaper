#pragma once

#include <glad/glad.h>
#include <string>

namespace VibeReaper {

    class Texture {
    public:
        Texture();
        ~Texture();

        // Load texture from file
        bool LoadFromFile(const std::string& path);

        // Bind texture to a specific texture unit
        void Bind(int textureUnit = 0);

        // Unbind texture
        void Unbind();

        // Getters
        unsigned int GetID() const { return textureID; }
        int GetWidth() const { return width; }
        int GetHeight() const { return height; }
        bool IsLoaded() const { return loaded; }

    private:
        unsigned int textureID;
        int width, height, channels;
        bool loaded;

        void Cleanup();
    };

} // namespace VibeReaper
