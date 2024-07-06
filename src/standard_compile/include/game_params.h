#ifndef GAME_PARAMS_H
#define GAME_PARAMS_H

#include "../external_libraries/include/raylib.h"
#include "../external_libraries/include/raymath.h"

struct GameParams {
    // Colours
    Color green = {173, 204, 96, 255};
    Color dark_green = {43, 51, 24, 255};
    Color white = {255, 255, 255, 255};

    // Window parameters
    const char* game_title = "Retro Snake";
    int cell_size = 30;
    int cell_count = 25;
    int offset = 75;

    // Total window size includes two border regions (the offset) and game region (cell_size / cell_count).
    int game_window_height = 2 * offset + cell_size * cell_count;
    int game_window_width = 2 * offset + cell_size * cell_count;

    // Snake parameters
    std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
	bool add_segment = false;
};

#endif