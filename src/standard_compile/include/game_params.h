#include "../external_libraries/include/raylib.h"
#include "../external_libraries/include/raymath.h"

struct gameParams {
    Color green = {173, 204, 96, 255};
    Color darkGreen = {43, 51, 24, 255};
    Color white = {255, 255, 255, 255};

    int cellSize = 30;
    int cellCount = 25;
    int offset = 75;
};