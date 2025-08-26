#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <map>

// --- STB_IMAGE --- 
#ifdef TEXTURE_LOADER_IMPLEMENTATION 
#define STB_IMAGE_IMPLEMENTATION 
#endif 
#include "stb_image.h"

namespace TextureRender {

    // Cache delle texture già caricate: filename -> GLuint
    static std::unordered_map<std::string, GLuint> textureCache;

    // Funzione per caricare una texture da file usando stb_image
    inline GLuint LoadTextureFromFile(const std::string& filename) {
        // Controlla se è già in cache
        auto it = textureCache.find(filename);
        if (it != textureCache.end()) {
            //std::cout << "Caricamento texture da cache: " << filename << std::endl;
            return it->second; // già caricata
        }

        int width, height, channels;
        unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        if (!image) {
            std::cerr << "Errore caricamento immagine: " << filename << std::endl;
            return LoadTextureFromFile("texture/block/null.png"); //diamo una texture di default per quelle non trovate 
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);

        stbi_image_free(image);

        // Salva nella cache
        textureCache[filename] = textureID;

        return textureID;
    }

    // Funzione principale: accetta filename, carica se serve e renderizza
    inline void RenderTexture(const std::string& filename, float x0, float y0, float x1, float y1){
        GLuint texID = LoadTextureFromFile(filename);
        if (texID == 0) return; // errore nel caricamento

        glBindTexture(GL_TEXTURE_2D, texID);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x0, y0);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x1, y0);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x1, y1);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x0, y1);
        glEnd();
    }

} // namespace TextureRender

// ---------- TILE TEXTURES ----------
static std::map<int, std::string> tileTextures = {
    //id: 0 trasparente 
    {1, "texture/block/Brick/Brick_01-256x256.png"},
    {2, "texture/block/Brick/Brick_02-256x256.png"},
    {3, "texture/block/Brick/Brick_03-256x256.png"},
    {4, "texture/block/Brick/Brick_04-256x256.png"},
    {5, "texture/block/Brick/Brick_05-256x256.png"},
    {6, "texture/block/Brick/Brick_06-256x256.png"},
    {7, "texture/block/Brick/Brick_07-256x256.png"},
    {8, "texture/block/Brick/Brick_08-256x256.png"},
    {9, "texture/block/Brick/Brick_09-256x256.png"},
    {10, "texture/block/Brick/Brick_10-256x256.png"},
    {11, "texture/block/Brick/Brick_11-256x256.png"},
    {12, "texture/block/Brick/Brick_12-256x256.png"},
    {13, "texture/block/Brick/Brick_13-256x256.png"},
    {14, "texture/block/Brick/Brick_14-256x256.png"},
    {15, "texture/block/Brick/Brick_15-256x256.png"},
    {16, "texture/block/Brick/Brick_16-256x256.png"},
    {17, "texture/block/Brick/Brick_17-256x256.png"},
    {18, "texture/block/Brick/Brick_18-256x256.png"},
    {19, "texture/block/Brick/Brick_19-256x256.png"},
    {20, "texture/block/Brick/Brick_20-256x256.png"},
    {21, "texture/block/Dirt/Dirt_01-256x256.png"},
    {22, "texture/block/Dirt/Dirt_02-256x256.png"},
    {23, "texture/block/Dirt/Dirt_03-256x256.png"},
    {24, "texture/block/Dirt/Dirt_04-256x256.png"},
    {25, "texture/block/Dirt/Dirt_05-256x256.png"},
    {26, "texture/block/Dirt/Dirt_06-256x256.png"},
    {27, "texture/block/Dirt/Dirt_07-256x256.png"},
    {28, "texture/block/Dirt/Dirt_08-256x256.png"},
    {29, "texture/block/Dirt/Dirt_09-256x256.png"},
    {30, "texture/block/Dirt/Dirt_10-256x256.png"},
    {31, "texture/block/Dirt/Dirt_11-256x256.png"},
    {32, "texture/block/Dirt/Dirt_12-256x256.png"},
    {33, "texture/block/Dirt/Dirt_13-256x256.png"},
    {34, "texture/block/Dirt/Dirt_14-256x256.png"},
    {35, "texture/block/Dirt/Dirt_15-256x256.png"},
    {36, "texture/block/Dirt/Dirt_16-256x256.png"},
    {37, "texture/block/Dirt/Dirt_17-256x256.png"},
    {38, "texture/block/Dirt/Dirt_18-256x256.png"},
    {39, "texture/block/Dirt/Dirt_19-256x256.png"},
    {40, "texture/block/Dirt/Dirt_20-256x256.png"},
    {41, "texture/block/Elements/Elements_01-256x256.png"},
    {42, "texture/block/Elements/Elements_02-256x256.png"},
    {43, "texture/block/Elements/Elements_03-256x256.png"},
    {44, "texture/block/Elements/Elements_04-256x256.png"},
    {45, "texture/block/Elements/Elements_05-256x256.png"},
    {46, "texture/block/Elements/Elements_06-256x256.png"},
    {47, "texture/block/Elements/Elements_07-256x256.png"},
    {48, "texture/block/Elements/Elements_08-256x256.png"},
    {49, "texture/block/Elements/Elements_09-256x256.png"},
    {50, "texture/block/Elements/Elements_10-256x256.png"},
    {51, "texture/block/Elements/Elements_11-256x256.png"},
    {52, "texture/block/Elements/Elements_12-256x256.png"},
    {53, "texture/block/Elements/Elements_13-256x256.png"},
    {54, "texture/block/Elements/Elements_14-256x256.png"},
    {55, "texture/block/Elements/Elements_15-256x256.png"},
    {56, "texture/block/Elements/Elements_16-256x256.png"},
    {57, "texture/block/Elements/Elements_17-256x256.png"},
    {58, "texture/block/Elements/Elements_18-256x256.png"},
    {59, "texture/block/Elements/Elements_19-256x256.png"},
    {60, "texture/block/Elements/Elements_20-256x256.png"},
    {61, "texture/block/Metal/Metal_01-256x256.png"},
    {62, "texture/block/Metal/Metal_02-256x256.png"},
    {63, "texture/block/Metal/Metal_03-256x256.png"},
    {64, "texture/block/Metal/Metal_04-256x256.png"},
    {65, "texture/block/Metal/Metal_05-256x256.png"},
    {66, "texture/block/Metal/Metal_06-256x256.png"},
    {67, "texture/block/Metal/Metal_07-256x256.png"},
    {68, "texture/block/Metal/Metal_08-256x256.png"},
    {69, "texture/block/Metal/Metal_09-256x256.png"},
    {70, "texture/block/Metal/Metal_10-256x256.png"},
    {71, "texture/block/Metal/Metal_11-256x256.png"},
    {72, "texture/block/Metal/Metal_12-256x256.png"},
    {73, "texture/block/Metal/Metal_13-256x256.png"},
    {74, "texture/block/Metal/Metal_14-256x256.png"},
    {75, "texture/block/Metal/Metal_15-256x256.png"},
    {76, "texture/block/Metal/Metal_16-256x256.png"},
    {77, "texture/block/Metal/Metal_17-256x256.png"},
    {78, "texture/block/Metal/Metal_18-256x256.png"},
    {79, "texture/block/Metal/Metal_19-256x256.png"},
    {80, "texture/block/Metal/Metal_20-256x256.png"},
    {81, "texture/block/Plaster/Plaster_01-256x256.png"},
    {82, "texture/block/Plaster/Plaster_02-256x256.png"},
    {83, "texture/block/Plaster/Plaster_03-256x256.png"},
    {84, "texture/block/Plaster/Plaster_04-256x256.png"},
    {85, "texture/block/Plaster/Plaster_05-256x256.png"},
    {86, "texture/block/Plaster/Plaster_06-256x256.png"},
    {87, "texture/block/Plaster/Plaster_07-256x256.png"},
    {88, "texture/block/Plaster/Plaster_08-256x256.png"},
    {89, "texture/block/Plaster/Plaster_09-256x256.png"},
    {90, "texture/block/Plaster/Plaster_10-256x256.png"},
    {91, "texture/block/Plaster/Plaster_11-256x256.png"},
    {92, "texture/block/Plaster/Plaster_12-256x256.png"},
    {93, "texture/block/Plaster/Plaster_13-256x256.png"},
    {94, "texture/block/Plaster/Plaster_14-256x256.png"},
    {95, "texture/block/Plaster/Plaster_15-256x256.png"},
    {96, "texture/block/Plaster/Plaster_16-256x256.png"},
    {97, "texture/block/Plaster/Plaster_17-256x256.png"},
    {98, "texture/block/Plaster/Plaster_18-256x256.png"},
    {99, "texture/block/Plaster/Plaster_19-256x256.png"},
    {100, "texture/block/Plaster/Plaster_20-256x256.png"},
    {101, "texture/block/Stone/Stone_01-256x256.png"},
    {102, "texture/block/Stone/Stone_02-256x256.png"},
    {103, "texture/block/Stone/Stone_03-256x256.png"},
    {104, "texture/block/Stone/Stone_04-256x256.png"},
    {105, "texture/block/Stone/Stone_05-256x256.png"},
    {106, "texture/block/Stone/Stone_06-256x256.png"},
    {107, "texture/block/Stone/Stone_07-256x256.png"},
    {108, "texture/block/Stone/Stone_08-256x256.png"},
    {109, "texture/block/Stone/Stone_09-256x256.png"},
    {110, "texture/block/Stone/Stone_10-256x256.png"},
    {111, "texture/block/Stone/Stone_11-256x256.png"},
    {112, "texture/block/Stone/Stone_12-256x256.png"},
    {113, "texture/block/Stone/Stone_13-256x256.png"},
    {114, "texture/block/Stone/Stone_14-256x256.png"},
    {115, "texture/block/Stone/Stone_15-256x256.png"},
    {116, "texture/block/Stone/Stone_16-256x256.png"},
    {117, "texture/block/Stone/Stone_17-256x256.png"},
    {118, "texture/block/Stone/Stone_18-256x256.png"},
    {119, "texture/block/Stone/Stone_19-256x256.png"},
    {120, "texture/block/Stone/Stone_20-256x256.png"},
    {121, "texture/block/Tile/Tile_01-256x256.png"},
    {122, "texture/block/Tile/Tile_02-256x256.png"},
    {123, "texture/block/Tile/Tile_03-256x256.png"},
    {124, "texture/block/Tile/Tile_04-256x256.png"},
    {125, "texture/block/Tile/Tile_05-256x256.png"},
    {126, "texture/block/Tile/Tile_06-256x256.png"},
    {127, "texture/block/Tile/Tile_07-256x256.png"},
    {128, "texture/block/Tile/Tile_08-256x256.png"},
    {129, "texture/block/Tile/Tile_09-256x256.png"},
    {130, "texture/block/Tile/Tile_10-256x256.png"},
    {131, "texture/block/Tile/Tile_11-256x256.png"},
    {132, "texture/block/Tile/Tile_12-256x256.png"},
    {133, "texture/block/Tile/Tile_13-256x256.png"},
    {134, "texture/block/Tile/Tile_14-256x256.png"},
    {135, "texture/block/Tile/Tile_15-256x256.png"},
    {136, "texture/block/Tile/Tile_16-256x256.png"},
    {137, "texture/block/Tile/Tile_17-256x256.png"},
    {138, "texture/block/Tile/Tile_18-256x256.png"},
    {139, "texture/block/Tile/Tile_19-256x256.png"},
    {140, "texture/block/Tile/Tile_20-256x256.png"},
    {141, "texture/block/Wood/Wood_01-256x256.png"},
    {142, "texture/block/Wood/Wood_02-256x256.png"},
    {143, "texture/block/Wood/Wood_03-256x256.png"},
    {144, "texture/block/Wood/Wood_04-256x256.png"},
    {145, "texture/block/Wood/Wood_05-256x256.png"},
    {146, "texture/block/Wood/Wood_06-256x256.png"},
    {147, "texture/block/Wood/Wood_07-256x256.png"},
    {148, "texture/block/Wood/Wood_08-256x256.png"},
    {149, "texture/block/Wood/Wood_09-256x256.png"},
    {150, "texture/block/Wood/Wood_10-256x256.png"},
    {151, "texture/block/Wood/Wood_11-256x256.png"},
    {152, "texture/block/Wood/Wood_12-256x256.png"},
    {153, "texture/block/Wood/Wood_13-256x256.png"},
    {154, "texture/block/Wood/Wood_14-256x256.png"},
    {155, "texture/block/Wood/Wood_15-256x256.png"},
    {156, "texture/block/Wood/Wood_16-256x256.png"},
    {157, "texture/block/Wood/Wood_17-256x256.png"},
    {158, "texture/block/Wood/Wood_18-256x256.png"},
    {159, "texture/block/Wood/Wood_19-256x256.png"},
    {160, "texture/block/Wood/Wood_20-256x256.png"}
};
#endif // TEXTURE_LOADER_HPP

