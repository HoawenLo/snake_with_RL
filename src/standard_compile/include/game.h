#ifndef GAME_H
#define GAME_H

#include <deque>
#include <iostream>
#include "../external_libraries/include/raylib.h"
#include "../external_libraries/include/raymath.h"

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque);
bool eventTriggered(double interval);

class Snake {
public:
    std::deque<Vector2> body;
    Vector2 direction;
    bool addSegment;

    void Draw();
    void Update();
    void Reset();
};

class Food {
public:
    Vector2 position;
    Texture2D texture;

    Food(std::deque<Vector2> snakeBody);

    Vector2 GenerateRandomCell();
    Vector2 GenerateRandomPos();
    void Draw();
};

class Game {
public:
    Snake snake;
    Food food;
    bool gameRunning;
    int score;

    void Draw();
    void Update();
    void CheckCollisionWithFood();
    void CheckCollisionWithEdges();
    void CheckCollisionWithTail();
    void GameOver();
};


#endif 