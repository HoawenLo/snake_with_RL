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
torch::Tensor GetState(const Snake &snake, const Food &food) {
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

	auto state = torch::tensor({
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


// Q Learning Algorithm

// Neural network

// Deep Q Network, inherits from the base neural network class.
struct DQN : torch::nn::Module {
	// Setup three linear layers
	torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

	// Constructor to take inputs for the input size (number of parameters of a state)
	// and the output size (number of actions - 4)
	DQN(int state_size, int action_size) {
		// register_module comes from the custom neural network module torch::nn::Module
		// Allows registering of child modules within the current module. Inputs are
		// name and layer - module.
		fc1 = register_module("fc1", torch::nn::Linear(state_size, 128));
		fc2 = register_module("fc2", torch::nn::Linear(128, 64));
		fc3 = register_module("fc3", torch::nn::Linear(64, action_size));
	}

	// Forward pass method
	// Layers are inherited from base class, they are pointers, hence need to dereference
	// to access its methods.
	torch::Tensor forward(torch::Tensor x) {
		x = torch::nn::functional::relu(fc1->forward(x));
		x = torch::nn::functional::relu(fc2->forward(x));
		x = fc3->forward(x);
		return x;
	}
};

void loadstatedict(torch::nn::Module& model, torch::nn::Module& target_model) {
torch::autograd::GradMode::set_enabled(false);  // make parameters copying possible
auto new_params = target_model.named_parameters(); // implement this
auto params = model.named_parameters(true /*recurse*/);
auto buffers = model.named_buffers(true /*recurse*/);
for (auto& val : new_params) {
    auto name = val.key();
    auto* t = params.find(name);
    if (t != nullptr) {
        t->copy_(val.value());
    } else {
        t = buffers.find(name);
        if (t != nullptr) {
            t->copy_(val.value());
            }
        }
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
using Experience = std::tuple<torch::Tensor, int, double, torch::Tensor, int>;

// deque to store memory
std::deque<Experience> replay_memory;
const int MEMORY_CAPACITY = 10000;

void storeExperience(const torch::Tensor state, int action, double reward, const torch::Tensor& next_state, int done) {
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

DQN policy_net(10, 4); // Neural network used for selecting actions
DQN target_net(10, 4); // Neural network copy used for stable Q-value updates

std::random_device rd; // Create random device to produce random numbers
std::mt19937 gen(rd()); // Use Mersenne Twister generator with seed provided by rd
std::uniform_real_distribution<> dis(0.0, 1.0); // Defines a uniform real distribution that produce random floating point number in range 0.0 to 1.0.

int steps_done = 0; // Count the number of steps done to decay epsilon

// Action selection function
int selectAction(const torch::Tensor& state) {
	double epsilon = EPSILON_END + (EPSILON_START - EPSILON_END) * exp(-1.0 * steps_done / EPSILON_DECAY); // Decay function
	steps_done++; // Increment steps done
	if (dis(gen) > epsilon) { // If generated value above epsilon threshold choose best action.
		std::cout << "Finally using forward." << std::endl;
		auto q_values = policy_net.forward(state); // Forward pass state through policy_net 
		return q_values.argmax(1).item<int>(); // First find the index of the maximum element of the tensor, returns a tensor corresponding to that index, then convert this index to an integer.
	} else {
		std::uniform_int_distribution<> action_dis(0, 3); // Select random action
		return action_dis(gen);
	}
}

torch::Tensor calculateLoss() {
	std::vector<Experience> batch(BATCH_SIZE); // Initialise a vector called batch, of size BATCH_SIZE, with default valued Experience tuples.
	std::sample(replay_memory.begin(), replay_memory.end(), batch.begin(), BATCH_SIZE, gen); 
	// Sample experience tuples from the replay memory, storing them in batch, storing BATCH_SIZE number of experiences, using generator gen.

	// Set up vectors to store state variables
	std::vector<torch::Tensor> state_batch, next_state_batch;
	std::vector<int> action_batch;
	std::vector<double> reward_batch;
	std::vector<int> done_batch;

	for (const auto& experience: batch) { // Iterate over the batch vector. Syntax (loop variable : container to be looped over)
		state_batch.push_back(std::get<0>(experience)); // For each batch vector get the relevant element from the experience tuple and push it into the batch vector.
		action_batch.push_back(std::get<1>(experience));
		reward_batch.push_back(std::get<2>(experience));
		next_state_batch.push_back(std::get<3>(experience));
		done_batch.push_back(std::get<4>(experience));
	}

	auto state_tensor = torch::stack(state_batch); // Stack the state_batch vector which contains state_batch as torch::tensors 
	auto next_state_tensor = torch::stack(next_state_batch); // Do the same for next_state_batch

	auto action_options = torch::TensorOptions().dtype(torch::kInt64);
	auto reward_options = torch::TensorOptions().dtype(torch::kFloat64);
	auto done_options = torch::TensorOptions().dtype(torch::kInt64);

	auto action_tensor = torch::from_blob(action_batch.data(), {static_cast<int>(action_batch.size())}, action_options); // Convert the action batch vector (a vector of integers) to a torch::tensor, stating the datatype.
	auto reward_tensor = torch::from_blob(reward_batch.data(), {static_cast<long>(reward_batch.size())}, reward_options); // Convert the reward batch vector (a vector of doubles) to a torch tensor.
	auto done_tensor = torch::from_blob(done_batch.data(), {static_cast<int>(done_batch.size())}, done_options); // Convert the done batch vector (a vector of Booleans) to a torch tensor with datatype torch::kBool.

	auto q_values = policy_net.forward(state_tensor).gather(1, action_tensor.unsqueeze(1)).squeeze(1); // From the action tensor, select the corresponding elements.
	
	// Calculate the maximum Q value for the next state q values. A tuple is returned, hence use .values() to extract the values.
	auto next_q_values = target_net.forward(next_state_tensor);
	auto max_next_q_values = std::get<0>(torch::max({next_q_values}, 0));

	auto expected_q_values = reward_tensor + (GAMMA * next_q_values * done_tensor); // Calculate the expected q values with Bellman equation. If the state has terminated, the done tensor will be true, 
	// hence inverting the done tensor will set it to false, meaning the calculation will yield only the reward tensor.

	// torch::Tensor loss = torch::mse_loss(q_values, expected_q_values); // Calculate the loss]

	// Perform ack propagation and update network parameters

	return torch::mse_loss(q_values, expected_q_values);
}

double GetReward(const Snake &snake, const Food &food) {
	if (Vector2Equals(snake.body[0], food.position)) return 100.0; // Food eaten
	if (ElementInDeque(snake.body[0], snake.body)) return -10.0; // Collision with self.
	if (snake.body[0].x < 0 || snake.body[0].x >= cellCount || snake.body[0].y < 0 || snake.body[0].y >= cellCount) return -10.0; // Collision with walls
	return -0.1; // Time penalty
}




void outputProgramState (std::ofstream& outFile, int episode, int action, torch::Tensor state, double reward, int replay_memory_size, std::deque<Experience> replay_memory) {
	outFile << "======================================================" << std::endl;
	outFile << "episode: " << episode << std::endl;
	outFile << "action: " << action << std::endl;
	outFile << "--------------------------------------------------" << std::endl;
	for (int i = 0; i < state.size(0); ++i) {
		outFile << "state var: " << state[i].item<float>() << std::endl;
	}
	outFile << "--------------------------------------------------" << std::endl;
	outFile << "reward: " << reward << std::endl;
	outFile << "replay_memory_size : " << replay_memory_size << std::endl;
	outFile << "--------------------------------------------------" << std::endl;
	for (int i = 0; i < replay_memory.size(); ++i) {
		outFile << "replay memory: " << std::get<0>(replay_memory[i]) << std::endl;
		outFile << "replay memory: " << std::get<1>(replay_memory[i]) << std::endl;
		outFile << "replay memory: " << std::get<2>(replay_memory[i]) << std::endl;
		outFile << "replay memory: " << std::get<3>(replay_memory[i]) << std::endl;
		outFile << "replay memory: " << std::get<4>(replay_memory[i]) << std::endl;
	}

}

int main() {

	// Initialise game, create game window and set FPS.
	InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");
	SetTargetFPS(60);
	std::ofstream outFile;
	outFile.open("output.txt"); 
	Game game = Game();
	int done;

	torch::optim::Adam optimizer(policy_net.parameters(), torch::optim::AdamOptions(0.001f)); // Setup optimiser

	loadstatedict(policy_net, target_net);
	target_net.eval(); // Set to evaluation mode
	int episode = 0;

	// Game loop:
	// Check if the esc key pressed to close the window
	while(WindowShouldClose() == false) {
		
		BeginDrawing();

		if (eventTriggered(0.2)) {
			episode++;
			torch::Tensor state = GetState(game.snake, game.food);
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

			torch::Tensor next_state = GetState(game.snake, game.food);
			double reward = GetReward(game.snake, game.food);
			
			if (!game.gameRunning) {
				done = 0;
			} else {
				done = 1;
			}
			
			storeExperience(state, action, reward, next_state, done); // If replay memory smaller than the batch size return, do not train the model.

			if (episode % 50 == 0) {
				loadstatedict(policy_net, target_net);
			}

			if (replay_memory.size() > BATCH_SIZE) {
				torch::Tensor loss = calculateLoss();
				optimizer.zero_grad(); // Clear gradients of all optimised parameters
				loss.backward(); // Compute gradients of loss function wrt to parameters, back propagation
				optimizer.step(); // Update parameters
			}

		outputProgramState(outFile, episode, action, state, reward, replay_memory.size(), replay_memory);

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
