#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Variable.hpp"
#include "TextureLoader.hpp"  // mantiene TextureRender::RenderTexture
#include <algorithm>
#include <functional>
#include <filesystem>

// ---------- COSTANTI MONDO ----------
const float WORLD_X_MIN = 0.0f;
const float WORLD_Y_MIN = -2.0f;
const float WORLD_X_MAX = 51.0f;
const float WORLD_Y_MAX = 45.3f;

// ---------- DECORATION ----------
struct Decoration {
    std::string texturePath;
    float x, y;         // coordinate nella griglia
    float width, height; // dimensioni in tiles
    float render_height_y=0; //serve per il rendering
};
// ---------- PORTAL ---------- (ovvero delle decorazioni con caratteristiche extra)
struct Portal : public Decoration {
    std::string path_new_level;
    int new_player_x_cord, new_player_y_cord;
    float render_height_x=0; //serve per i portali per la logica di teletrasporto
};
// ---------- ENTITY ----------
struct Entity {
    std::string texturePath;
    int x, y;         // coordinate nella griglia (in tiles)
    float width, height; // dimensioni base in tiles
    float scaleX = 1.0f, scaleY = 1.0f; // scala dimensionale

    int frameWidth = 10, frameHeight = 13;
    int framesPerRow = 8, framesPerCol = 8;
    float speed = 10.0f;
    int currentFrameX = 0, currentFrameY = 0;
    int stop_frame_y = 8;
    float animTimer = 0.0f, animDelay = 0.15f;

    float render_height_y=0; //serve per il rendering

    void updateAnimation(float dt) {
        animTimer += dt;
        if(animTimer >= animDelay) {
            animTimer -= animDelay;
            currentFrameX = (currentFrameX + 1) % (stop_frame_y);
        }
    }
    void renderEntity(float quadSizeX, float quadSizeY) const {
        int frameX = currentFrameX, frameY = currentFrameY;

        // Dimensioni finali del quad (scala applicata)
        float finalWidth  = quadSizeX * width  * scaleX;   // larghezza in tiles moltiplicata per la scala
        float finalHeight = quadSizeY * height * scaleY;   // altezza in tiles moltiplicata per la scala

        // Posizione del centro sulla griglia
        float centerX = -1.0f + x * quadSizeX + quadSizeX * 0.5f;
        float centerY = -1.0f + y * quadSizeY + quadSizeY * 0.5f;

        // Coordinate finali del quad (centro in centerX/centerY)
        float x0 = centerX - finalWidth  / 2.0f;
        float y0 = centerY - finalHeight / 2.0f;
        float x1 = centerX + finalWidth  / 2.0f;
        float y1 = centerY + finalHeight / 2.0f;

        // Coordinate texture
        float tx0 = frameX / float(framesPerRow);
        float ty0 = frameY / float(framesPerCol);
        float tx1 = (frameX + 1) / float(framesPerRow);
        float ty1 = (frameY + 1) / float(framesPerCol);

        GLuint texID = TextureRender::LoadTextureFromFile(texturePath);
        glBindTexture(GL_TEXTURE_2D, texID);

        glBegin(GL_QUADS);
            glTexCoord2f(tx0, ty1); glVertex2f(x0, y0);
            glTexCoord2f(tx1, ty1); glVertex2f(x1, y0);
            glTexCoord2f(tx1, ty0); glVertex2f(x1, y1);
            glTexCoord2f(tx0, ty0); glVertex2f(x0, y1);
        glEnd();
    }

};
// ---------- HITBOX ----------
struct Hitbox {
    float x0, y0, x1, y1; // coordinate float
};
// ---------- TILE ----------
struct Tile {
    int id = 0;
    std::string texturePath; // path diretta alla texture
};
//mappa per memorizzare il percorso dei livelli
static std::map<std::string, int> LevelMap;
// ---------- LEVEL ----------
struct Level {
    int width, height;
    std::vector<Tile> tiles;
    std::vector<Decoration> decorations;
    std::vector<Portal> portals;
    std::vector<Entity> entity;
    std::vector<Hitbox> hitboxes;

    Level(int w, int h) : width(w), height(h), tiles(w*h) {}

    Tile& getTile(int x, int y) { return tiles[y*width + x]; }
    const Tile& getTile(int x, int y) const { return tiles[y*width + x]; }
};
// ---------- FUNZIONE DI CARICAMENTO ----------
inline Level loadLevelFromFile(const std::string& filename, int w, int h) {
    std::cout << "CARICO LIVELLO: " << filename << std::endl;
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

            // correzione scalabile per hitbox
            float correctFactorX = 0.04f * dec.x;                 // piccolo offset orizzontale
            float correctFactorY = 0.10f * dec.y;         // più alto se dec.y grande

            Hitbox hb;
            hb.x0 = WORLD_X_MIN + dec.x * TILE_SIZE_X * (1.0f+0.04f);
            hb.y0 = WORLD_Y_MIN + dec.y * TILE_SIZE_Y + TILE_SIZE_Y * correctFactorY;
            hb.x1 = WORLD_X_MIN + (dec.x + dec.width) * TILE_SIZE_X + TILE_SIZE_X * correctFactorX;
            float maxHeight = std::min(static_cast<float>(dec.height), 3.0f);
            hb.y1 = WORLD_Y_MIN + (dec.y + maxHeight) * TILE_SIZE_Y + TILE_SIZE_Y * correctFactorY;
            dec.render_height_y = static_cast<float>(hb.y0);
            lvl.decorations.push_back(dec);
            lvl.hitboxes.push_back(hb);

            std::cout << "Decorazione trovata: " << dec.texturePath
                      << " -> hitbox float: x0=" << hb.x0
                      << " y0=" << hb.y0
                      << " x1=" << hb.x1
                      << " y1=" << hb.y1 << std::endl;
        } 
        else if (line[0] == 'P') { //portale
            std::stringstream ss(line.substr(2));
            Portal port;
            ss >> port.texturePath >> port.x >> port.y >> port.width >> port.height >> port.path_new_level >> port.new_player_x_cord >> port.new_player_y_cord;

            // correzione scalabile per hitbox
            float correctFactorX = 0.04f * port.x;                 // piccolo offset orizzontale
            float correctFactorY = 0.10f * port.y;         // più alto se port.y grande

            Hitbox hb;
            hb.x0 = WORLD_X_MIN + port.x * TILE_SIZE_X * (1.0f+0.04f);
            hb.y0 = WORLD_Y_MIN + port.y * TILE_SIZE_Y + TILE_SIZE_Y * correctFactorY;
            hb.x1 = WORLD_X_MIN + (port.x + port.width) * TILE_SIZE_X + TILE_SIZE_X * correctFactorX;
            float maxHeight = std::min(static_cast<float>(port.height), 3.0f);
            hb.y1 = WORLD_Y_MIN + (port.y + maxHeight) * TILE_SIZE_Y + TILE_SIZE_Y * correctFactorY;
            port.render_height_x = static_cast<float>((hb.x0 + hb.x1)/2);   
            port.render_height_y = static_cast<float>(hb.y0);
            lvl.portals.push_back(port);
            lvl.hitboxes.push_back(hb);

            std::cout << "Portale trovata: " << port.texturePath
                      << " -> hitbox float: x0=" << hb.x0
                      << " y0=" << hb.y0
                      << " x1=" << hb.x1
                      << " y1=" << hb.y1 << std::endl;
        }
        else if (line[0] == 'E') { // entita
            std::stringstream ss(line.substr(2));
            Entity ent;
            ss >> ent.texturePath >> ent.x >> ent.y >> ent.width >> ent.height >> ent.scaleX >> ent.currentFrameX >> ent.stop_frame_y >> ent.framesPerRow >> ent.framesPerCol >> ent.frameWidth >> ent.frameHeight;
            ent.scaleY = ent.scaleX;

            // Dimensioni scalate
            float scaledWidth  = ent.width  * TILE_SIZE_X * ent.scaleX * 0.4f;
            float scaledHeight = ent.height * TILE_SIZE_Y * ent.scaleY * 0.4f;

            // Centro corretto del tile
            float centerX = WORLD_X_MIN + ent.x * TILE_SIZE_X + TILE_SIZE_X * 0.5f;
            float centerY = WORLD_Y_MIN + ent.y * TILE_SIZE_Y + TILE_SIZE_Y * 0.8f;

            // Hitbox centrata sulla texture
            Hitbox hb;
            hb.x0 = centerX - scaledWidth  / 2.0f;
            hb.y0 = centerY - scaledHeight / 2.0f;
            hb.x1 = centerX + scaledWidth  / 3.0f;
            hb.y1 = centerY + scaledHeight / 2.0f;

            // Piccoli offset di correzione opzionali
            hb.x0 += TILE_SIZE_X * 0.01f;
            hb.y0 += TILE_SIZE_Y * 0.01f;
            hb.x1 -= TILE_SIZE_X * 0.01f;
            if (hb.y1-hb.y1*0.40f > hb.y0) hb.y1 -= hb.y1*0.40f;

            ent.render_height_y =  static_cast<float>(hb.y0);
            lvl.entity.push_back(ent);
            lvl.hitboxes.push_back(hb);

            std::cout << "Entita trovata: " << ent.texturePath
                      << " -> hitbox float: x0=" << hb.x0
                      << " y0=" << hb.y0
                      << " x1=" << hb.x1
                      << " y1=" << hb.y1 << std::endl;
        } 
        else { // tile normale
            std::stringstream ss(line);
            for (int x = 0; x < w; x++) {
                int id;
                ss >> id;
                Tile& tile = lvl.getTile(x, row);
                tile.id = id;

                // assegna la path della texture direttamente
                auto it = tileTextures.find(id);
                if (it != tileTextures.end()) {
                    tile.texturePath = it->second; 
                } else {
                    tile.texturePath = "texture/block/null.png"; // fallback
                }
            }
            row++;
        }

    }

    return lvl;
}
// ---------- PLAYER ----------
struct Player {
    float x = 25.0f, y = 22.0f;
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
            currentFrameX = (currentFrameX + 1) % (framesPerRow - 2); //saltiamo i frame del salto
        }
    }
};
// ---------- GAME MANAGER ----------
class GameManager {
    private:
        std::vector<Level> levels;
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
    public:
        Player player{"texture/char_a_p1/char_a_p1_0bas_humn_v01.png"};

        void addLevel(const std::string& filename, int w, int h) {
            LevelMap.insert({filename, levels.size()}); //inserisce nella HashMap l'indice del arrau della posizione del livello 
            levels.push_back(loadLevelFromFile(filename, w, h)); //inseriamo nel array il livello (sara nel indice trovato prima)
        }

        Level& getLevel(int idx) {
            return levels[idx];
        }

        void renderLevel(int& lvl_number, double frameTime, bool playerActive) {
            if (lvl_number < 0 || lvl_number >= (int)levels.size()) {
                std::cerr << "Errore: livello " << lvl_number << " inesistente!\n";
                return;
            }

            Level& lvl = levels[lvl_number];
            float quadSizeX = 2.0f / lvl.width;
            float quadSizeY = 2.0f / lvl.height;

            // --- 1. Renderizza il background direttamente ---
            for (int y = 0; y < lvl.height; y++) {
                for (int x = 0; x < lvl.width; x++) {
                    Tile& tile = lvl.getTile(x, y);
                    TextureRender::RenderTexture(tile.texturePath,
                                                -1.0f + x * quadSizeX,
                                                -1.0f + y * quadSizeY,
                                                -1.0f + (x + 1) * quadSizeX,
                                                -1.0f + (y + 1) * quadSizeY);
                    
                }
            }

            // --- 2. Metti tutto in un vector di Drawable ---
            struct Drawable {
                float y;                      // Y per ordinamento
                std::function<void()> draw;    // funzione di rendering
                std::string name;              // Player, Decorazione, Entità
            };

            std::vector<Drawable> drawables;

            // PLAYER
            drawables.push_back(Drawable{
                player.y, 
                [this, playerActive]() {
                    if (playerActive) renderPlayer();
                    else renderPlayer(0, 0);
                },
                "Player"
            });

            // DECORAZIONI
            for (const auto& dec : lvl.decorations) {
                float x0 = -1.0f + dec.x * quadSizeX;
                float y0 = -1.0f + dec.y * quadSizeY;
                float x1 = x0 + dec.width * quadSizeX;
                float y1 = y0 + dec.height * quadSizeY;

                drawables.push_back(Drawable{
                    dec.render_height_y,  // base della decorazione
                    [=]() { TextureRender::RenderTexture(dec.texturePath, x0, y0, x1, y1); },
                    "Decorazione"
                });
            }
            // PORTALI
            for (const auto& port : lvl.portals) {
                float x0 = -1.0f + port.x * quadSizeX;
                float y0 = -1.0f + port.y * quadSizeY;
                float x1 = x0 + port.width * quadSizeX;
                float y1 = y0 + port.height * quadSizeY;

                drawables.push_back(Drawable{
                    port.render_height_y,  // base del portale
                    [=]() { TextureRender::RenderTexture(port.texturePath, x0, y0, x1, y1); },
                    "Portale"
                });
            }

            // ENTITY
            for (Entity& ent : lvl.entity) {
                drawables.push_back(Drawable{
                    ent.render_height_y,
                    [&ent, quadSizeX, quadSizeY, frameTime]() {
                        ent.renderEntity(quadSizeX, quadSizeY);
                        ent.updateAnimation(frameTime);
                    },
                    "Entità"
                });
            }

            // --- 3. Ordina per Y crescente ---
            std::sort(drawables.begin(), drawables.end(),
                    [](const Drawable& a, const Drawable& b) {
                        return a.y > b.y;
                    });

            // --- 4. Disegna in ordine ---
            for (auto& d : drawables) {
                //std::cout << d.name << " : Y = " << d.y << std::endl;  // debug
                d.draw();
            }
            
            // controlliamo se il player interagisce con un portale
            for (const auto& port : getLevel(lvl_number).portals) {
                #define MARGIN_PORTAL_X 0.12f
                #define MARGIN_PORTAL_Y 0.3f
                //se la x,y del player sono vicine alla x,y del portale allora 
                //carica il nuovo livello
                if (port.render_height_x * (1.0f - MARGIN_PORTAL_X) <= player.x && port.render_height_x * (1.0f + MARGIN_PORTAL_X) >= player.x){
                    //se il player e' nel margine del portale
                    if (port.render_height_y * (1.0f - ((port.height <=2) ? MARGIN_PORTAL_Y : 0.05f)) <= player.y && port.render_height_y * (1.0f) >= player.y){
                        //se il player e' nel margine anche delle y del portale
                        printf("INTERAZIONE PORTALE!!!!! \n");
                        auto it = LevelMap.find(port.path_new_level);
                        if (it != LevelMap.end()) {
                            std::cout << "Cambio scena, nuovo livello = " << it->first << std::endl;
                            std::cout << "SPOSTO PLAYER A: x= " << port.new_player_x_cord << "; y= " << port.new_player_y_cord << std::endl;
                            lvl_number = it->second;
                            player.x = port.new_player_x_cord;
                            player.y = port.new_player_y_cord;
                            //TODO: sistemare animazioni dopo passaggio portale

                            TextureRender::RenderBlackTransition(1.0f, -1.0f, -1.0f, 1.0f, 1.0f);
                        } else {
                            std::cerr << "ERRORE GRAVE: livello non trovato nella HashMap! (" << port.path_new_level << ")" << std::endl;
                        }

                    }

                }
            }
        }
};

void loadAllLevels(GameManager& gameManager, const std::string& folder, int gridX, int gridY) {
    namespace fs = std::filesystem;

    try {
        for (const auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                gameManager.addLevel(entry.path().string(), gridX, gridY);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Errore nell'accesso alla cartella " << folder 
                  << ": " << e.what() << std::endl;
    }
}

#endif // GAME_MANAGER_HPP