#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include "include/raylib.h"
#include "include/raymath.h"
#include <torch/torch.h>
#include "raylib.h"


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
		// food.position = food.GenerateRandomPos(snake.body);
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

enum Actions { UP, DOWN, LEFT, RIGHT };
const int numActions = 4;

// A function to get the state representation
std::string GetState(const Snake &snake, const Food &food) {
	Vector2 head = snake.body[0];
	Vector2 foodPos = food.position;
	Vector2 direction = snake.direction;

	bool obstacleUp = false, obstacleDown = false, obstacleLeft = false, obstacleRight = false;

	// Check for obstacles in each direction.
	// ElementInDeque iterates over all elements in the deque and it it finds
	// the input element in the deque it returns true.
	// In this case iterate over elements in the snake body and see if the snake
	// is pointing towards its own body (ElementInDeque) or (||) it is pointing
	// to outside the game boundary.
	if (ElementInDeque(Vector2{head.x, head.y - 1}, snake.body) || head.y - 1 < 0) obstacleUp = true;
	if (ElementInDeque(Vector2{head.x, head.y + 1}, snake.body) || head.y + 1 >= cellCount) obstacleDown = false;
	if (ElementInDeque(Vector2{head.x - 1, head.y}, snake.body) || head.x - 1 < 0) obstacleLeft = true;
	if (ElementInDeque(Vector2{head.x + 1, head.y}, snake.body) || head.x + 1 >= cellCount) obstacleRight = true;

	std::string state = std::to_string((int)head.x) + "_" + std::to_string((int)head.y) + "_" +
						std::to_string((int)foodPos.x) + "_" + std::to_string((int)foodPos.y) + "_" +
						std::to_string((int)direction.x) + "_" + std::to_string((int)direction.y) + "_" +
						std::to_string(obstacleUp) + "_" + std::to_string(obstacleDown) + "_" +
						std::to_string(obstacleLeft) + "_" + std::to_string(obstacleRight);

	return state;
}

std::unordered_map<std::string, std::vector<double>> QTable;

std::vector<double>& GetQValues(const std::string& state) {
	if (QTable.find(state) == QTable.end()) { // Search the QTable for state, if it does not find the state, 
	// it will return QTable.end() iterator, which suggests it has reached the end of the QTable.
		QTable[state] = std::vector<double>(numActions, 0.0); // Initialise the q values in the q table.
	}
	return QTable[state];
}

// Q Learning Algorithm

// Choose action, either select the best known action or try a random action to potentially find better option, epsilon greedy.

double alpha = 0.1; // Learning rate: the size of step update.
double gamma_ = 0.9; // Discount factor: measures the importance of future rewards.


// Function to choose next action, epsilon greedy.

Actions ChooseAction(const std::string &state, double epsilon) {
	if (GetRandomValue(0, 100) < epsilon * 100) { // If random value is below exploration rate threshold, choose random action.
		return static_cast<Actions>(GetRandomValue(0, numActions - 1)); // Cast an integer to actions type.
	} else {
		GetQValues(state); // Search the QTable for the state if it does not exist set it up
		auto &qValues = QTable[state]; // Return the q values from the Q table.
		return static_cast<Actions>(std::max_element(qValues.begin(), qValues.end()) - qValues.begin()); // Return the position of the maximum element. 
		// Which corresponds to an action. Find the max element (returning an iterator - a pointer to the element in the vector) and the  minus it away
		// from the first q value element of the vector to attain the position of the element.
	}
}

// Updating Q values. Will utilise the Q value update formula and also get the q values for current and next state.

void UpdateQTable(const std::string &state, Actions action, double reward, const std::string &nextState) {
	GetQValues(nextState); // Search the QTable for nextState, if it does not exist initialise it.
	auto &qValues = QTable[state];
	double maxFutureQ = *std::max_element(QTable[nextState].begin(), QTable[nextState].end()); // Dereference the max q value of the q values of the next state.
	// Need to dereference as the variable maxFutureQ will point to the element in the vector instead of the actual value. The type would be std::vector<double>
	// ::iterator.
	qValues[action] += alpha * (reward + gamma_ * maxFutureQ - qValues[action]); // Q value update formula
}

// Reward function

double GetReward(const Snake &snake, const Food &food) {
	if (Vector2Equals(snake.body[0], food.position)) return 50.0; // Food eaten
	// if (ElementInDeque(snake.body[0], snake.body)) return -10.0; // Collision with self.
	if (snake.body[0].x < 0 || snake.body[0].x >= cellCount || snake.body[0].y < 0 || snake.body[0].y >= cellCount) return -50.0; // Collision with walls
	return -0.1; // Time penalty
}

template<typename T>

void outputDebugFiles(std::string outputFilename, const T& outputVariable, std::string outputDescription) {
	std::ofstream outputFile(outputFilename); // Create an output file stream

    if (outputFile.is_open()) { // Check if the file is successfully opened
        // Write data to the file
        outputFile << outputDescription << " " << outputVariable << std::endl;

        // Close the file stream
        outputFile.close();
        std::cout << "Data has been written to output.txt" << std::endl;
    } else {
        std::cerr << "Error: Unable to open output file." << std::endl;
    }
}



void PrintQValues(const std::string& state) {
    if (QTable.find(state) != QTable.end()) {
        const std::vector<double>& qValues = QTable[state];
        std::cout << "Q-values for state \"" << state << "\":" << std::endl;
        for (size_t i = 0; i < qValues.size(); ++i) {
            std::cout << "Action " << i << ": " << qValues[i] << std::endl;
        }
    } else {
        std::cout << "State \"" << state << "\" not found in Q-table." << std::endl;
    }
}

// Function to print QTable for debugging
void PrintQTable() {
    for (const auto& pair : QTable) {
        const std::string& state = pair.first;
        const std::vector<double>& qValues = pair.second;

        std::ostringstream oss;
        oss << "Q-values for state \"" << state << "\":\n";
        for (size_t i = 0; i < qValues.size(); ++i) {
            oss << "Action " << i << ": " << qValues[i] << "\n";
        }
        std::cout << oss.str() << std::endl;
    }
}

void OutputQTableToFile(const std::string& filename) {
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    for (const auto& pair : QTable) {
        const std::string& state = pair.first;
        const std::vector<double>& qValues = pair.second;

        outFile << "State: " << state << std::endl;
        for (int action = 0; action < numActions; ++action) {
            outFile << "  Action " << action << ": " << qValues[action] << std::endl;
        }
    }

    outFile.close();
    std::cout << "QTable output to file " << filename << " successfully." << std::endl;
}

void outputVector2(const Vector2& vectorValues, std::string vectorName, std::ofstream& outFile) {
	outFile << "  " << vectorName << " x: " << vectorValues.x << std::endl;
	outFile << "  " << vectorName << " y: " << vectorValues.y << std::endl;
}

std::string actionToString(Actions action) {
    switch (action) {
        case UP: return "UP";
        case DOWN: return "DOWN";
        case LEFT: return "LEFT";
        case RIGHT: return "RIGHT";
        // Add other cases for other actions
        default: return "UNKNOWN";
    }
}

void outputAllActions(const std::string& filename, std::string& state, const Actions& chosenAction, const Vector2& snakeDirection, const Vector2& snakeHead, const Vector2& foodPosition, const double& reward) {
	std::ofstream outFile(filename, std::ios::app);

    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

	outFile << "Current state: " << state << std::endl;
	outFile << "  Chosen action: " << actionToString(chosenAction) << std::endl;
	outFile << "  Reward value: " << reward << std::endl;
	outputVector2(snakeHead, "Snake head position", outFile);
	outputVector2(foodPosition, "Food position", outFile);
	outputVector2(snakeDirection, "Snake direction", outFile);

}

double decayEpsilon(double epsilon, double decayRate) {
	return epsilon * decayRate;
}

int main() {

	torch::Tensor tensor = torch::eye(3);
    std::cout << "Tensor:\n" << tensor << std::endl;


	double epsilon = 0.9; // Exploration rate: the probability of choosing a random action instead of best known action.
	double epsilonDecayRate = 0.999; // The epsilon decay rate.
	// Initialise game, create game window and set FPS.
	InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
	SetTargetFPS(60);

	Game game = Game();

	// Game loop:
	// Check if the esc key pressed to close the window
	while(WindowShouldClose() == false) {
		BeginDrawing();

		if (eventTriggered(0.2)) {
			std::string state = GetState(game.snake, game.food);
			Actions action = ChooseAction(state, epsilon);

			std::cout << "state: " << state << std::endl;
			std::cout << "Chosen action: " << action << std::endl;

			switch (action) {
				case UP:
					// Check if the current direction is not already downwards
					if (game.snake.direction.y != 1) {
						game.snake.direction = {0, -1};
						game.gameRunning = true;
					}
					break;
				case DOWN:
					// Check if the current direction is not already upwards
					if (game.snake.direction.y != -1) {
						game.snake.direction = {0, 1};
						game.gameRunning = true;
					}
					break;
				case LEFT:
					// Check if the current direction is not already to the right
					if (game.snake.direction.x != 1) {
						game.snake.direction = {-1, 0};
						game.gameRunning = true;
					}
					break;
				case RIGHT:
					// Check if the current direction is not already to the left
					if (game.snake.direction.x != -1) {
						game.snake.direction = {1, 0};
						game.gameRunning = true;
					}
					break;
			}

			std::cout << "Snake direction: (" << game.snake.direction.x << ", " << game.snake.direction.y << ")" << std::endl;

			game.Update();

			std::string nextState = GetState(game.snake, game.food);
			double reward = GetReward(game.snake, game.food);

			UpdateQTable(state, action, reward, nextState);

            outputAllActions("action_history.txt", state, action, game.snake.direction, game.snake.body[0], game.food.position, reward);

			// Apply decay rate
			std::cout << "Epsilon before" << epsilon << std::endl;
			epsilon = decayEpsilon(epsilon, epsilonDecayRate);
			std::cout << "Epsilon after" << epsilon << std::endl;

		}

		// Player input for moving
		// if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
		// 	game.snake.direction = {0, -1};
		// 	game.gameRunning = true;
		// }
		// if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
		// 	game.snake.direction = {0, 1};
		// 	game.gameRunning = true;
		// }
		// if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
		// 	game.snake.direction = {-1, 0};
		// 	game.gameRunning = true;
		// }
		// if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
		// 	game.snake.direction = {1, 0};
		// 	game.gameRunning = true;
		// }

		// Drawing
		ClearBackground(green);
		DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
		DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
		DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
		game.Draw();
		EndDrawing();
	};

	OutputQTableToFile("qtable_output.txt");

	CloseWindow();
	return 0;
}
