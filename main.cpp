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
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "opengv_title_here", nullptr, nullptr);
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
    //colore sfondo
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //R, G, B, clear
    
    //inizializziamo il GameManager
    GameManager GameManager;
    // Carico due livelli
    GameManager.addLevel("levels/level1.txt", 16, 16);
    
    const double dt = 1.0 / HZ; // logica a 60Hz
    double accumulator = 0.0;

    double lastTime = glfwGetTime();
    double lastInputTime = lastTime; // tempo dell'ultimo input
    const double idleThreshold = 0.5; // secondi di inattività prima di frame 0,0

    int fps_counter = 0;
    double fpsTime = lastTime;
    int current_lvl = 0;
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double frameTime = currentTime - lastTime;
        lastTime = currentTime;
        accumulator += frameTime;

        bool inputDetected = false;

    // aggiornamento logica
    while (accumulator >= dt) {
        const Level& lvl = GameManager.getLevel(current_lvl); // livello corrente

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

        // aggiorna l'ultimo tempo di input
        if (inputDetected) lastInputTime = currentTime;

        accumulator -= dt;
}


        // rendering
        glClear(GL_COLOR_BUFFER_BIT);
        GameManager.renderLevel(current_lvl);

        // Se inattivo da troppo tempo, frame 0,0
        if (currentTime - lastInputTime >= idleThreshold) {
            GameManager.renderPlayer(0, 0);
        } else {
            GameManager.renderPlayer(); // usa frame attuale
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        // fps counter
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

