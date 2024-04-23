#include <iostream>
#include <deque>
#include "include/raylib.h"
#include "include/raymath.h"

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
Color white = {255, 255, 255, 255};

int cellSize = 30;
int cellCount = 25;

// Keep track of time snake was last updated
double lastUpdateTime = 0;

bool eventTriggered(double interval) {
	// Raylib method to get current time
	double currentTime = GetTime();

	if(currentTime - lastUpdateTime >= interval) {
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snake {
public:
	// Initialise snake body
	std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
	Vector2 direction = {1, 0};

	// Draw snake
	void Draw() {
		for(int i = 0; i < body.size(); i++) {
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle {x * cellSize, y * cellSize, (float)cellSize, (float)cellSize};
			DrawRectangleRounded(segment, 0.5, 6, darkGreen);
		}
	}

	// Update snake position when moving.
	void Update () {
		// Use deque method to remove back Vector2 element.
		body.pop_back();
		// use deque method to put Vector2 element with added direction
		// component to the front of the deque to simulate snake moving.
		body.push_front(Vector2Add(body[0], direction));

	}
};

class Food {

public:
	Vector2 position;
	Texture2D texture;

	// Initialise position of food.
	Food() {
		position = GenerateRandomPos();
	}

	// Generate random position for the food.
	Vector2 GenerateRandomPos() {
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		
		return Vector2 {x, y};
	}

	// Draw food
	void Draw() {
		DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, white);
	}


};

class Game {
public:
	Snake snake = Snake();
	Food food = Food();

	void Draw() {
		food.Draw();
		snake.Draw();
	}

	void Update() {
		snake.Update();
	}

	void CheckCollisionWithFood() {
		if(Vector2Equals(snake.body[0], food.position)) {
			std::cout << "Eating food" << std::endl;
		}
	}
};

int main() {
	// Initialise game, create game window and set FPS.
	std::cout << "Starting new game..." << std::endl;
	InitWindow(cellSize * cellCount, cellSize * cellCount, "Retro Snake");
	SetTargetFPS(60);

	Game game = Game();

	// Game loop:
	// Check if the esc key pressed to close the window
	while(WindowShouldClose() == false) {
		BeginDrawing();

		if (eventTriggered(0.2)) {
			game.Update();
		}

		// Player input for moving
		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
			game.snake.direction = {0, -1};
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
			game.snake.direction = {0, 1};
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
			game.snake.direction = {-1, 0};
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
			game.snake.direction = {1, 0};
		}

		// Drawing
		ClearBackground(green);
		game.Draw();

		EndDrawing();
	};

	CloseWindow();
	return 0;
}
