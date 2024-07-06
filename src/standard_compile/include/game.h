#ifndef GAME_H
#define GAME_H

#include <deque>
#include <iostream>
#include "../include/game_params.h"
#include "../external_libraries/include/raylib.h"
#include "../external_libraries/include/raymath.h"

struct eventResult {
    bool triggered;
    float last_update_time;
};

bool elementInDeque(Vector2 element, std::deque<Vector2> deque);
bool eventTriggered(float interval);

class Snake {
public:
    std::deque<Vector2> body;
    Vector2 direction;
    bool add_segment;
    GameParams params;

    Snake(GameParams params);

    void draw();
    void update();
    void reset();
};

class Food {
public:
    Vector2 position;
    GameParams params;

    Food(GameParams params);

    Vector2 generateRandomCell();
    Vector2 generateRandomPos(std::deque<Vector2> snake_body);
    void draw();
};

class Game {
public:
    Snake snake;
    Food food;
    bool game_running;
    int score;
    GameParams params;

    Game(bool game_running, int score, const GameParams& params);

    void draw();
    void update();
    void checkCollisionWithFood();
    void checkCollisionWithEdges();
    void checkCollisionWithTail();
    void gameOver();
};


#endif 