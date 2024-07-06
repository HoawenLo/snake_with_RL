#include "../external_libraries/include/raylib.h"
#include "../external_libraries/include/raymath.h"

struct GameParams {
    Color green = {173, 204, 96, 255};
    Color dark_green = {43, 51, 24, 255};
    Color white = {255, 255, 255, 255};

    int cell_size = 30;
    int cell_count = 25;
    int offset = 75;

    // Snake parameters
    std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
	bool add_segment = false;
};