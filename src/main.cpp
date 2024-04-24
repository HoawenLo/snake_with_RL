#include <iostream>
#include <deque>
#include "include/raylib.h"
#include "include/raymath.h"

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
Color white = {255, 255, 255, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

// Keep track of time snake was last updated
double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque) {
	for (int i = 0; i < deque.size(); i++) {
		if(Vector2Equals(deque[i], element)) {
			return true;
		}
	}
	return false;
}

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
	bool addSegment = false;

	// Draw snake
	void Draw() {
		for(int i = 0; i < body.size(); i++) {
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle {offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
			DrawRectangleRounded(segment, 0.5, 6, darkGreen);
		}
	}

	// Update snake position when moving.
	void Update () {
		// use deque method to put Vector2 element with added direction
		// component to the front of the deque to simulate snake moving
		body.push_front(Vector2Add(body[0], direction));
		if(addSegment == true) {
			addSegment = false;
		} else {
			// Use deque method to remove back Vector2 element.
			body.pop_back();
		}
	}

	void Reset() {
		body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
		direction = {1, 0};
	}
};

class Food {

public:
	Vector2 position;
	Texture2D texture;

	// Initialise position of food.
	Food(std::deque<Vector2> snakeBody) {
		position = GenerateRandomPos(snakeBody);
	}

	Vector2 GenerateRandomCell() {
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{x, y};
	}

	// Generate random position for the food.
	Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody) {
		Vector2 position = GenerateRandomCell();
		while(ElementInDeque(position, snakeBody)) {
			position = GenerateRandomCell();
		}
		return position;
	}

	// Draw food
	void Draw() {
		DrawRectangle(offset + position.x * cellSize, offset + position.y * cellSize, cellSize, cellSize, white);
	}


};

class Game {
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool gameRunning = true;
	int score = 0;

	void Draw() {
		food.Draw();
		snake.Draw();
	}

	void Update() {
		if(gameRunning) {
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdges();
			CheckCollisionWithTail();
		}
		
	}

	void CheckCollisionWithFood() {
		if(Vector2Equals(snake.body[0], food.position)) {
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score ++;
		}
	}

	void CheckCollisionWithEdges() {
		if(snake.body[0].x == cellCount || snake.body[0].x == -1) {
			GameOver();
		}
		if(snake.body[0].y == cellCount || snake.body[0].y == -1) {
			GameOver();
		}
	}

	void GameOver() {
		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);
		gameRunning = false;
		score = 0;
	}

	void CheckCollisionWithTail () {
		std::deque<Vector2> headlessBody = snake.body;
		headlessBody.pop_front();
		if(ElementInDeque(snake.body[0], headlessBody)) {
			GameOver();
		}
	}
};

int main() {
	// Initialise game, create game window and set FPS.
	std::cout << "Starting new game..." << std::endl;
	InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
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
			game.gameRunning = true;
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
			game.snake.direction = {0, 1};
			game.gameRunning = true;
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
			game.snake.direction = {-1, 0};
			game.gameRunning = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
			game.snake.direction = {1, 0};
			game.gameRunning = true;
		}

		// Drawing
		ClearBackground(green);
		DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
		DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
		DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
		game.Draw();
		EndDrawing();
	};

	CloseWindow();
	return 0;
}
