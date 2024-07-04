#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include "../external_libraries/include/raylib.h"
#include "../external_libraries/include/raymath.h"

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
std::vector<float> GetState(const Snake &snake, const Food &food) {
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

	float normalisedHeadX = head.x / (float)cellCount;
	float normalisedHeadY = head.y / (float)cellCount;
	float normalisedFoodX = foodPos.x / (float)cellCount;
	float normalisedFoodY = foodPos.y / (float)cellCount;

	std::vector<float> state = std::vector({
		normalisedHeadX,
        normalisedHeadY,
        normalisedFoodX,
        normalisedFoodY,
        direction.x,
        direction.y,
        (float)obstacleUp,
        (float)obstacleDown,
        (float)obstacleLeft,
        (float)obstacleRight
	});

	return state;
}

const int BATCH_SIZE = 64; // Number of experiences to sample for each training step
const double GAMMA = 0.99; // Discount factor for future rewards - measure importance of future rewards
const double EPSILON_START = 1.0; // Parameters for epsilon greedy with decay - if below epsilon threshold action will be random.
const double EPSILON_END = 0.1;
const double EPSILON_DECAY = 1000;
const int TARGET_UPDATE = 10;
int steps_done;

// Action selection function
int selectAction(const std::vector<double>& state, NeuralNetwork policy_net) {
	double epsilon = EPSILON_END + (EPSILON_START - EPSILON_END) * exp(-1.0 * steps_done / EPSILON_DECAY); // Decay function
	steps_done++; // Increment steps done
	if (dis(gen) > epsilon) { // If generated value above epsilon threshold choose best action.
		auto q_values = policy_net.forward(state); // Forward pass state through policy_net 
		return q_values.argmax(1).item<int>(); // First find the index of the maximum element of the tensor, returns a tensor corresponding to that index, then convert this index to an integer.
	} else {
		std::uniform_int_distribution<> action_dis(0, num_actions - 1); // Select random action
		return action_dis(gen)
	};
}

void storeExperience(const std::vector<double>& state, int action, double reward, const std::vector<double>& next_state, int done, int MEMORY_CAPACITY, ReplayMemory replay_memory) {
	if (replay_memory.memory.size() >= MEMORY_CAPACITY) {
		replay_memory.popFront();
	}
}

// Q Learning Algorithm

// Neural network

// Random number generator for weights initialization
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-0.1, 0.1);

// Activation function (ReLU) and its derivative
inline double relu(double x) {
    return std::max(0.0, x);
}

inline double relu_derivative(double x) {
    return x > 0 ? 1.0 : 0.0;
}

// A simple Layer class
class Layer {
public:
    std::vector<std::vector<double>> weights;
    std::vector<double> biases;
    std::vector<double> outputs;
    std::vector<double> inputs;
    std::vector<double> deltas;

    Layer(int input_size, int output_size) {
        weights.resize(output_size, std::vector<double>(input_size));
        biases.resize(output_size);

        for (auto& row : weights)
            for (auto& val : row)
                val = dis(gen);

        for (auto& val : biases)
            val = dis(gen);
    }

    std::vector<double> forward(const std::vector<double>& input) {
        inputs = input;
        outputs.resize(biases.size());
        for (size_t i = 0; i < biases.size(); ++i) {
            outputs[i] = biases[i];
            for (size_t j = 0; j < input.size(); ++j)
                outputs[i] += weights[i][j] * input[j];
            outputs[i] = relu(outputs[i]);
        }
        return outputs;
    }

    std::vector<double> backward(const std::vector<double>& grad) {
        deltas.resize(inputs.size());
        std::fill(deltas.begin(), deltas.end(), 0.0);

        for (size_t i = 0; i < outputs.size(); ++i) {
            double delta = grad[i] * relu_derivative(outputs[i]);
            for (size_t j = 0; j < inputs.size(); ++j)
                deltas[j] += delta * weights[i][j];
            for (size_t j = 0; j < inputs.size(); ++j)
                weights[i][j] -= 0.01 * delta * inputs[j];
            biases[i] -= 0.01 * delta;
        }

        return deltas;
    }
};

// Neural Network class
class NeuralNetwork {
public:
    std::vector<Layer> layers;

    void add_layer(int input_size, int output_size) {
        layers.emplace_back(input_size, output_size);
    }

    std::vector<double> forward(const std::vector<double>& input) {
        std::vector<double> output = input;
        for (auto& layer : layers)
            output = layer.forward(output);
        return output;
    }

    void backward(const std::vector<double>& grad) {
        std::vector<double> delta = grad;
        for (auto it = layers.rbegin(); it != layers.rend(); ++it)
            delta = it->backward(delta);
    }
};

// Replay memory class
class ReplayMemory {
public:
    struct Experience {
        std::vector<double> state;
        int action;
        double reward;
        std::vector<double> next_state;
        bool done;
    };

    std::vector<Experience> memory;
    size_t capacity;
    size_t position;

    ReplayMemory(size_t capacity) : capacity(capacity), position(0) {
		memory.resize(capacity);
	}

    void add(const Experience& experience) {
        memory[position] = experience;
        position = (position + 1) % capacity;
    }

    std::vector<Experience> sample(size_t batch_size) {
        std::vector<Experience> batch;
        std::uniform_int_distribution<> dist(0, memory.size() - 1);
        for (size_t i = 0; i < batch_size; ++i)
            batch.push_back(memory[dist(gen)]);
        return batch;
    }

    bool is_full() const {
        return memory.size() == capacity;
    }

	void popFront() {
    	assert(!memory.empty());
    	memory.erase(memory.begin());
	}
};

// DQN class
class DQN {
public:
    NeuralNetwork policy_net;
    NeuralNetwork target_net;
    ReplayMemory replay_memory;
    double gamma;

    DQN(int input_size, int output_size, size_t memory_capacity, double gamma)
        : replay_memory(memory_capacity), gamma(gamma) {
        // Initialize policy network
        policy_net.add_layer(input_size, 64);
        policy_net.add_layer(128, 64);
        policy_net.add_layer(64, output_size);

        // Initialize target network with the same architecture
        target_net = policy_net;
    }

    void update_target_net() {
        target_net = policy_net;
    }

    void train(int batch_size) {
        if (replay_memory.capacity < batch_size)
            return;

        auto batch = replay_memory.sample(batch_size);

        for (const auto& exp : batch) {
            auto q_values = policy_net.forward(exp.state);
            auto next_q_values = target_net.forward(exp.next_state);

            double q_update = exp.reward;
            if (!exp.done) {
                q_update += gamma * *std::max_element(next_q_values.begin(), next_q_values.end());
            }

            std::vector<double> target = q_values;
            target[exp.action] = q_update;

            std::vector<double> grad(target.size());
            for (size_t i = 0; i < target.size(); ++i) {
                grad[i] = q_values[i] - target[i];
            }

            policy_net.backward(grad);
        }
    }
};

double GetReward(const Snake &snake, const Food &food) {
	if (Vector2Equals(snake.body[0], food.position)) return 100.0; // Food eaten
	if (ElementInDeque(snake.body[0], snake.body)) return -10.0; // Collision with self.
	if (snake.body[0].x < 0 || snake.body[0].x >= cellCount || snake.body[0].y < 0 || snake.body[0].y >= cellCount) return -10.0; // Collision with walls
	return -0.1; // Time penalty
}


// void outputProgramState (std::ofstream& outFile, int episode, int action, std::vector<double> state, double reward, int replay_memory_size, std::deque<Experience> replay_memory) {
// 	outFile << "======================================================" << std::endl;
// 	outFile << "episode: " << episode << std::endl;
// 	outFile << "action: " << action << std::endl;
// 	outFile << "--------------------------------------------------" << std::endl;
// 	for (int i = 0; i < state.size(0); ++i) {
// 		outFile << "state var: " << state[i].item<float>() << std::endl;
// 	}
// 	outFile << "--------------------------------------------------" << std::endl;
// 	outFile << "reward: " << reward << std::endl;
// 	outFile << "replay_memory_size : " << replay_memory_size << std::endl;
// 	outFile << "--------------------------------------------------" << std::endl;
// 	for (int i = 0; i < replay_memory.size(); ++i) {
// 		outFile << "replay memory: " << std::get<0>(replay_memory[i]) << std::endl;
// 		outFile << "replay memory: " << std::get<1>(replay_memory[i]) << std::endl;
// 		outFile << "replay memory: " << std::get<2>(replay_memory[i]) << std::endl;
// 		outFile << "replay memory: " << std::get<3>(replay_memory[i]) << std::endl;
// 		outFile << "replay memory: " << std::get<4>(replay_memory[i]) << std::endl;
// 	}

// }

int main() {

	// Initialise game, create game window and set FPS.
	InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
	SetTargetFPS(60);
	std::ofstream outFile;
	outFile.open("output.txt"); 
	Game game = Game();
	DQN dqn(4, 2, 10000, 0.99);

	int episode = 0;

	// Game loop:
	// Check if the esc key pressed to close the window
	while(WindowShouldClose() == false) {
		
		BeginDrawing();

		if (eventTriggered(0.2)) {
			episode++;
			std::vector state = GetState(game.snake, game.food);
			int action = selectAction(state);

			// std::cout << "state: " << state << std::endl;
			// std::cout << "Chosen action: " << action << std::endl;

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

			game.Update();

			std::vector next_state = GetState(game.snake, game.food);
			double reward = GetReward(game.snake, game.food);
			
			storeExperience(state, action, reward, next_state, done); // If replay memory smaller than the batch size return, do not train the model.

			if (episode % 50 == 0) {
				dqn.update_target_net();
			}

			dqn.train(32);
			dqn.update_target_net();

		// outputProgramState(outFile, episode, action, state, reward, replay_memory.size(), replay_memory);

		};

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
		
		std::cout << "episode: " << episode << std::endl;

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
