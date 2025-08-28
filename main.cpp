#include <iostream>
#include <GL/glew.h> //manager input
#include <GLFW/glfw3.h> //manager window
#include <ctime>
#define TEXTURE_LOADER_IMPLEMENTATION  // solo **una volta** in un file .cpp
#include "TextureLoader.hpp"
#include "GameManager.hpp"
#include "Variable.hpp"


int main(int argc, char* argv[]) {
    srand(time(nullptr));
    // 1. Inizializza GLFW
    if (!glfwInit()) {
        std::cerr << "Errore inizializzazione GLFW\n";
        return -1;
    }

    // 2. Crea finestra
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "TileWord-2d", nullptr, nullptr);
    if (!window) {
        std::cerr << "Errore apertura finestra\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(VSync); // 0 = nessun V-Sync, 1 = V-Sync attivo
    glEnable(GL_TEXTURE_2D); //Abilita la texturizzazione
    // Abilita blending per la trasparenza
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 3. Inizializza GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Errore inizializzazione GLEW\n";
        return -1;
    }
    // Stampa informazioni sulla versione OpenGL
    const GLubyte* renderer = glGetString(GL_RENDERER); // nome GPU
    const GLubyte* vendor   = glGetString(GL_VENDOR);   // vendor GPU
    const GLubyte* version  = glGetString(GL_VERSION);  // versione OpenGL
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION); // versione GLSL

    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "Vendor: " << vendor << std::endl;
    std::cout << "OpenGL Version: " << version << std::endl;
    std::cout << "GLSL Version: " << glslVersion << std::endl;

    //colore sfondo
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //R, G, B, clear
    
    //inizializziamo il GameManager
    GameManager GameManager;
    // Carico TUTTI i livelli
    loadAllLevels(GameManager, "levels", GRID_SIZE, GRID_SIZE);
    for (const auto& pair : LevelMap) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }
    const double dt = 1.0 / HZ; // logica a 60Hz
    double accumulator = 0.0;

    double lastTime = glfwGetTime();
    double lastInputTime = lastTime; // tempo dell'ultimo input
    const double idleThreshold = 0.5; // secondi di inattività prima di frame 0,0

    int fps_counter = 0;
    double fpsTime = lastTime;
    int current_lvl;
    auto it = LevelMap.find("levels/entity_animation.txt");
    if (it != LevelMap.end()) {
        std::cout << "Livello iniziale trovato! path=" << it->first << "; id=" << it->second << std::endl;
        current_lvl = it->second;
    } else {
        std::cerr << "Livello iniziale non esistente !" << std::endl;
        return 1;
    }
    //loop gioco
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - lastTime;
        lastTime = currentTime;
        accumulator += frameTime;

        bool inputDetected = false;

        // LOGICA (movimento a timestep fisso)
        while (accumulator >= dt) {
            const Level& lvl = GameManager.getLevel(current_lvl);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                GameManager.player.moveUp(dt, lvl);
                inputDetected = true;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                GameManager.player.moveDown(dt, lvl);
                inputDetected = true;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                GameManager.player.moveLeft(dt, lvl);
                inputDetected = true;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                GameManager.player.moveRight(dt, lvl);
                inputDetected = true;
            }

            if (inputDetected) lastInputTime = currentTime;
            accumulator -= dt;
        }

        // RENDERING
        glClear(GL_COLOR_BUFFER_BIT);
        GameManager.renderLevel(current_lvl, frameTime, (currentTime - lastInputTime < idleThreshold));

        glfwSwapBuffers(window);
        glfwPollEvents();

        // FPS COUNTER
        fps_counter++;
        if(currentTime - fpsTime >= 1.0){
            std::cout << "\r" << (VSync ? "(VSync: on) " : "(VSync: off) ")
                    << "FPS: " << fps_counter << std::flush;
            fps_counter = 0;
            fpsTime = currentTime;
        }
    }

    // 5. Pulizia
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

