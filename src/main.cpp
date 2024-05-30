#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <tuple>
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


// Q Learning Algorithm

// Neural network

// Deep Q Network, inherits from the base neural network class.
struct DQN : torch::nn:Module {
	// Setup three linear layers
	torch::nn:Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

	// Constructor to take inputs for the input size (number of parameters of a state)
	// and the output size (number of actions - 4)
	DQN(int state_size, int action_size) {
		// register_module comes from the custom neural network module torch::nn::Module
		// Allows registering of child modules within the current module. Inputs are
		// name and layer - module.
		fc1 = register_module("fc1", torch::nn:Linear(state_size, 128));
		fc2 = register_module("fc2", torch::nn:Linear(128, 64));
		fc3 = register_module("fc3", torch::nn:Linear(64, action_size));
	}

	// Forward pass method
	// Layers are inherited from base class, they are pointers, hence need to dereference
	// to access its methods.
	torch::Tensor forward(torch::Tensor x) {
		x = torch::relu(fc1->forward(x));
		x = torch::relu(fc2->forward(x));
		x = fc3->forward(x);
		return x;
	}
}

// Create a type Experience where elements are
// torch::Tensor state
// int action
// double reward
// torch::Tensor next_state
// bool done - indicates whether an episode terminated after taking an action, required
// as end states are handled differently, the next state is omitted as it does not
// exist.
using Experience = std::tuple<torch::Tensor, int, double, torch::Tensor, bool>;

// deque to store memory
std::deque<Experience> replay_memory;
const int MEMORY_CAPACITY = 10000;

void store_experience(const torch::Tensor state, int action, double reward, const torch::Tensor& next_state, bool done) {
	if (replay_memory.size() >= MEMORY_CAPACITY) {
		replay_memory.pop_front();
	}
	replay_memory.push_back(std::make_tuple(state, action, reward, next_state, done));
}

// Initialise hyperparameters

const int BATCH_SIZE = 64; // Number of experiences to sample for each training step
const double GAMMA = 0.99; // Discount factor for future rewards - measure importance of future rewards
const double EPSILON_START = 1.0; // Parameters for epsilon greedy with decay - if below epsilon threshold action will be random.
const double EPSILON_END = 0.1;
const double EPSILON_DECAY = 1000;
const int TARGET_UPDATE = 10;

DQN policy_net(state_size, action_size); // Neural network used for selecting actions
DQN target_net(state_size, action_size); // Neural network copy used for stable Q-value updates
torch::optim::Adam optimizer(policy_net.parameters(), torch::optim::AdamOptions(0.001)); // Setup optimiser

target_net.load_state_dict(policy_net.state_dict()) // Copy the weights and biases of policy_net to target_net
target_net.eval(); // Set to evaluation mode

std::random_device rd; // Create random device to produce random numbers
std::mt19937 gen(rd()); // Use Mersenne Twister generator with seed provided by rd
std::uniform_real_distribution<> dis(0.0, 1.0); // Defines a uniform real distribution that produce random floating point number in range 0.0 to 1.0.

int steps_done = 0; // Count the number of steps done to decay epsilon

// Action selection function
int select_action(const torch::Tensor& state) {
	double epsilon = EPSILON_END + (EPSILON_START - EPSILON_END) * exp(-1.0 * steps_done / EPSILON_DECAY); // Decay function
	steps_done++; // Increment steps done
	if (dis(gen) > epsilon) { // If generated value above epsilon threshold choose best action.
		auto q_values = policy_net.forward(state); // Forward pass state through policy_net 
		return q_values.argmax(1).item<int>(); // First find the index of the maximum element of the tensor, returns a tensor corresponding to that index, then convert this index to an integer.
	} else {
		std::uniform_int_distribution<> action_dis(0, action_size - 1); // Select random action
		return action_dis(gen)
	}
}

void optimizer_model() {
	
}


// Debugging

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
