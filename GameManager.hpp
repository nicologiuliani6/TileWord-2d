#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Variable.hpp"
#include "TextureLoader.hpp"  // mantiene TextureRender::RenderTexture


// ---------- COSTANTI MONDO ----------
const float WORLD_X_MIN = 0.0f;
const float WORLD_Y_MIN = -2.0f;
const float WORLD_X_MAX = 51.0f;
const float WORLD_Y_MAX = 45.3f;

// ---------- DECORATION ----------
struct Decoration {
    std::string texturePath;
    int x, y;         // coordinate nella griglia
    int width, height; // dimensioni in tiles
};

// ---------- HITBOX ----------
struct Hitbox {
    float x0, y0, x1, y1; // coordinate float
};

// ---------- TILE ----------
struct Tile {
    int id = 0;
};

// ---------- LEVEL ----------
struct Level {
    int width, height;
    std::vector<Tile> tiles;
    std::vector<Decoration> decorations;
    std::vector<Hitbox> hitboxes;

    Level(int w, int h) : width(w), height(h), tiles(w*h) {}

    Tile& getTile(int x, int y) { return tiles[y*width + x]; }
    const Tile& getTile(int x, int y) const { return tiles[y*width + x]; }
};

// ---------- FUNZIONE DI CARICAMENTO ----------
inline Level loadLevelFromFile(const std::string& filename, int w, int h) {
    Level lvl(w, h);
    std::ifstream file(filename);
    if (!file) { 
        std::cerr << "Errore: impossibile aprire " << filename << std::endl; 
        exit(1); 
    }

    // calcolo dimensione di ogni tile nello spazio del mondo
    float TILE_SIZE_X = (WORLD_X_MAX - WORLD_X_MIN) / GRID_SIZE;
    float TILE_SIZE_Y = (WORLD_Y_MAX - WORLD_Y_MIN) / GRID_SIZE;

    std::string line;
    int row = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line[0] == 'D') { // decorazione
            std::stringstream ss(line.substr(2));
            Decoration dec;
            ss >> dec.texturePath >> dec.x >> dec.y >> dec.width >> dec.height;
            lvl.decorations.push_back(dec);

            // correzione scalabile per hitbox
            float correctFactorX = 0.04f * dec.x;                 // piccolo offset orizzontale
            float correctFactorY = 0.10f * dec.y;         // più alto se dec.y grande

            Hitbox hb;
            hb.x0 = WORLD_X_MIN + dec.x * TILE_SIZE_X * (1.0f+0.04f);
            hb.y0 = WORLD_Y_MIN + dec.y * TILE_SIZE_Y + TILE_SIZE_Y * correctFactorY;
            hb.x1 = WORLD_X_MIN + (dec.x + dec.width) * TILE_SIZE_X + TILE_SIZE_X * correctFactorX;
            hb.y1 = WORLD_Y_MIN + (dec.y + dec.height) * TILE_SIZE_Y + TILE_SIZE_Y * correctFactorY;

            lvl.hitboxes.push_back(hb);

            std::cout << "Decorazione trovata: " << dec.texturePath
                      << " -> hitbox float: x0=" << hb.x0
                      << " y0=" << hb.y0
                      << " x1=" << hb.x1
                      << " y1=" << hb.y1 << std::endl;
        } else { // tile normale
            std::stringstream ss(line);
            for (int x = 0; x < w; x++) {
                int id;
                ss >> id;
                lvl.getTile(x, row).id = id;
            }
            row++;
        }
    }

    return lvl;
}



// ---------- PLAYER ----------
struct Player {
    float x = 0.0f, y = -2.0f;
    int frameWidth = 10, frameHeight = 13;
    int framesPerRow = 8, framesPerCol = 8;
    std::string texturePath;

    float speed = 10.0f;
    int currentFrameX = 0, currentFrameY = 0;
    float animTimer = 0.0f, animDelay = 0.1f;

    float playerWidth = 1.0f;  // dimensioni player in unità reali
    float playerHeight = 1.0f;

    Player(const std::string& path) : texturePath(path) {}

    bool collidesWithHitboxes(const Level& lvl, float newX, float newY) {
        for(auto& hb : lvl.hitboxes) {
            if(newX < hb.x1 && newX + playerWidth > hb.x0 &&
               newY < hb.y1 && newY + playerHeight > hb.y0) {
                //std::cout << "COLLISION! Player at x=" << newX << " y=" << newY << std::endl;
                return true;
            }
        }
        return false;
    }

    bool moveRight(float dt, const Level& lvl) {
        float nextX = x + speed * dt;
        if(nextX + playerWidth > WORLD_X_MAX) return false;
        if(collidesWithHitboxes(lvl,nextX,y)) return false;
        x = nextX; currentFrameY = 6; updateAnimation(dt); return true;
    }

    bool moveLeft(float dt, const Level& lvl) {
        float nextX = x - speed * dt;
        if(nextX < WORLD_X_MIN) return false;
        if(collidesWithHitboxes(lvl,nextX,y)) return false;
        x = nextX; currentFrameY = 7; updateAnimation(dt); return true;
    }

    bool moveUp(float dt, const Level& lvl) {
        float nextY = y + speed * dt;
        if(nextY + playerHeight > WORLD_Y_MAX) return false;
        if(collidesWithHitboxes(lvl,x,nextY)) return false;
        y = nextY; currentFrameY = 5; updateAnimation(dt); return true;
    }

    bool moveDown(float dt, const Level& lvl) {
        float nextY = y - speed * dt;
        if(nextY < WORLD_Y_MIN) return false;
        if(collidesWithHitboxes(lvl,x,nextY)) return false;
        y = nextY; currentFrameY = 4; updateAnimation(dt); return true;
    }

    void updateAnimation(float dt) {
        animTimer += dt;
        if(animTimer >= animDelay) {
            animTimer -= animDelay;
            currentFrameX = (currentFrameX + 1) % (framesPerRow - 2);
        }
    }
};


// ---------- GAME MANAGER ----------
class GameManager {
private:
    std::vector<Level> levels;
public:
    Player player{"texture/char_a_p1/char_a_p1_0bas_humn_v01.png"};

    void addLevel(const std::string& filename, int w, int h) {
        levels.push_back(loadLevelFromFile(filename, w, h));
    }

    Level& getLevel(int idx) {
        return levels[idx];
    }

    void renderLevel(int lvl_number) {
        if (lvl_number < 0 || lvl_number >= (int)levels.size()) {
            std::cerr << "Errore: livello " << lvl_number << " inesistente!\n";
            return;
        }

        const Level& lvl = levels[lvl_number];
        float quadSizeX = 2.0f / lvl.width;
        float quadSizeY = 2.0f / lvl.height;

        for (int y = 0; y < lvl.height; y++) {
            for (int x = 0; x < lvl.width; x++) {
                int id = lvl.getTile(x, y).id;
                auto it = tileTextures.find(id);
                if (it != tileTextures.end()) {
                    TextureRender::RenderTexture(it->second,
                                                 -1.0f + x * quadSizeX,
                                                 -1.0f + y * quadSizeY,
                                                 -1.0f + (x + 1) * quadSizeX,
                                                 -1.0f + (y + 1) * quadSizeY);
                }
            }
        }

        for (const auto& dec : lvl.decorations) {
            float x0 = -1.0f + dec.x * quadSizeX;
            float y0 = -1.0f + dec.y * quadSizeY;
            float x1 = x0 + dec.width * quadSizeX;
            float y1 = y0 + dec.height * quadSizeY;
            TextureRender::RenderTexture(dec.texturePath, x0, y0, x1, y1);
        }
    }

    void renderPlayer() {
        renderPlayer(player.currentFrameX, player.currentFrameY);
    }

    void renderPlayer(int frameX, int frameY) {
        float scale = 0.037f;
        float x0 = -1.13f + player.x * scale;
        float y0 = -1.05f + player.y * scale;
        float x1 = x0 + player.frameWidth * scale;
        float y1 = y0 + player.frameHeight * scale;

        float tx0 = frameX / float(player.framesPerRow);
        float ty0 = frameY / float(player.framesPerCol);
        float tx1 = (frameX + 1) / float(player.framesPerRow);
        float ty1 = (frameY + 1) / float(player.framesPerCol);

        GLuint texID = TextureRender::LoadTextureFromFile(player.texturePath);
        glBindTexture(GL_TEXTURE_2D, texID);

        glBegin(GL_QUADS);
            glTexCoord2f(tx0, ty1); glVertex2f(x0, y0);
            glTexCoord2f(tx1, ty1); glVertex2f(x1, y0);
            glTexCoord2f(tx1, ty0); glVertex2f(x1, y1);
            glTexCoord2f(tx0, ty0); glVertex2f(x0, y1);
        glEnd();
    }
};

#endif // GAME_MANAGER_HPP