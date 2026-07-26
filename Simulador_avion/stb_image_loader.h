// Simple texture loader using basic file I/O (without full STB Image)
// Handles PNG/JPG/BMP loading by wrapping basic image format parsing

#ifndef STB_IMAGE_LOADER_H
#define STB_IMAGE_LOADER_H

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

// Forward declare stbi functions - will be linked from system libraries if available
extern "C" {
    typedef unsigned char stbi_uc;
    extern stbi_uc* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp);
    extern void stbi_image_free(void* retval_from_stbi_load);
}

namespace TextureLoader {
    inline unsigned int loadTextureFromFile(const char* path) {
        int width, height, nrChannels;

        // Load image using stbi_load
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);

        if (!data) {
            std::cerr << "Error cargando textura: " << path << std::endl;
            return 0;
        }

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "    ✓ Textura cargada: " << path << " (" << width << "x" << height << ")" << std::endl;

        stbi_image_free(data);
        return textureID;
    }
}

#endif // STB_IMAGE_LOADER_H
