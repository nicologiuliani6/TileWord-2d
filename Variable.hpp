#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <utility> // per std::pair

// Enum con risoluzioni predefinite
enum class Resolution {
    R_800x600,
    R_1024x768,
    R_1280x720,
    R_1920x1080
};

// Funzione constexpr che restituisce WIDTH e HEIGHT
constexpr std::pair<int,int> getResolution(Resolution res) {
    switch(res) {
        case Resolution::R_800x600:   return {800, 600};
        case Resolution::R_1024x768:  return {1024, 768};
        case Resolution::R_1280x720:  return {1280, 720};
        case Resolution::R_1920x1080: return {1920, 1080};
    }
    return {1024, 768}; // default
}

// Seleziona la risoluzione che vuoi usare
constexpr Resolution CURRENT_RES = Resolution::R_1024x768;
constexpr int WIDTH  = getResolution(CURRENT_RES).first;
constexpr int HEIGHT = getResolution(CURRENT_RES).second;

// Aspect ratio calcolato automaticamente
constexpr float ASPECT_RATIO = float(WIDTH) / float(HEIGHT);

#define GRID_SIZE 16
#define VSync true
#define HZ 60.0

#endif // VARIABLE_HPP
