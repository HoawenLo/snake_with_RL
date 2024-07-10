#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/dqn.h"
#include "../include/game.h"
#include "../include/game_params.h"

std::vector<std::vector<std::vector<float>>> read_in_weights (std::string filepath) {
    std::ifstream file(filepath);
    std::string line;
    std::vector<std::vector<std::vector<float>>> weights;

    int layer_number = -1;

    while (std::getline(file, line)) {
        if (line == "layer") {
            weights.push_back(std::vector<std::vector<float>>());
            layer_number++;
            continue;
        }

        std::istringstream iss(line);
        std::string row_marker;
        iss >> row_marker;

        if (row_marker == "row") {
            weights[layer_number].push_back(std::vector<float>());
            float weight;
            while (iss >> weight) {
                weights[layer_number].back().push_back(weight);
            }
        }
    }

    return weights;
}

std::vector<std::vector<float>> read_in_biases(std::string filepath) {
    std::ifstream file(filepath);
    std::string line;
    std::vector<std::vector<float>> biases;

    while (std::getline(file, line)) {
        if (line == "bias") {
            biases.push_back(std::vector<float>());
            continue;
        }

        std::istringstream iss(line);
        float bias;
        while (iss >> bias) {
            biases.back().push_back(bias);
        }
    }
    return biases;
}

int main() {
	std::vector<std::vector<std::vector<float>>> loaded_weights = read_in_weights("best_weights.txt");
	std::vector<std::vector<float>> loaded_biases = read_in_biases("best_biases.txt");

	GameParams game_params;
	NetworkParams network_params;
	int action;

	// Total window size includes two border regions (the offset) and game region (cell_size / cell_count).
	// See game_params.h for values.
	InitWindow(game_params.game_window_height, game_params.game_window_width, game_params.game_title);
	SetTargetFPS(game_params.frame_rate);

	// Create game object, with game active to false, score to zero, passing the game configuration
	// and the last update time to zero.
	Game game = Game(false, 0, game_params, 0);

	// Create dep q network object, with input size 10, being the state size, 4 the output size, representing
	// the 4 output q values representing the 4 actions, 10000 memory capacity and parameters of the deep q network.	
	DQN dqn = DQN(10, 4, network_params.MEMORY_CAPACITY, network_params);
	dqn.policy_net.load_in_network_params(loaded_weights, loaded_biases);

	// Initialise epsiode number.
	// int episode = 0;

	// Initialise actions
	enum Actions { UP, DOWN, LEFT, RIGHT };

	// Open files.
	// std::ofstream outFile1("q_values.txt");
	// std::ofstream outFile2("weights.txt");
	// std::ofstream outFile3("biases.txt");

	// Game loop:
	// Check is the esc key pressed to close the window.
	while (WindowShouldClose() == false) {

		BeginDrawing();

		// Get state and decide action.
		std::vector<float> state = getState(game.snake, game.food);

		// Get previous snake head position to calculate if have moved towards
		// or away from food.
		Vector2 previous_snake_head_pos = game.snake.body[0];

		// if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
		// 	int action = 0;
		// 	game.snake.direction = {0, -1};
		// 	game.game_running = true;
		// }
		// if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
		// 	int action = 1;
		// 	game.snake.direction = {0, 1};
		// 	game.game_running = true;
		// }
		// if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
		// 	int action = 2;
		// 	game.snake.direction = {-1, 0};
		// 	game.game_running = true;
		// }
		// if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
		// 	int action = 3;
		// 	game.snake.direction = {1, 0};
		// 	game.game_running = true;
		// }

		// Select action
		// int action = dqn.selectActionTrain(state, dqn.policy_net, episode);
		int action = dqn.selectActionTest(state, dqn.policy_net);

		// Implement action from generated action value.
		switch(action) {
			case UP:
				if (game.snake.direction.y != 1) {
					game.snake.direction = {0, -1};
					game.game_running = true;
				}
				break;
			case DOWN:
				if (game.snake.direction.y != -1) {
					game.snake.direction = {0, 1};
					game.game_running = true;
				}
				break;
			case LEFT:
				if (game.snake.direction.x != 1) {
					game.snake.direction = {-1, 0};
					game.game_running = true;
				}
				break;
			case RIGHT:
				if (game.snake.direction.x != -1) {
					game.snake.direction = {1, 0};
					game.game_running = true;
				}
				break;
		}

		// Update snake position
		game.snake.update();
		
		// Get the reward from action being done.
		// float reward = getReward(game.snake, game.food, previous_snake_head_pos);

		// Check collisions
		game.checkCollisions();

		// Get next state.
		// std::vector<float> next_state = getState(game.snake, game.food);

		// Calculate q values to store.
		// std::vector<float> q_values = dqn.policy_net.forward(state);
		
		// Output data.
		// outFile1 << "-----------" << std::endl;
		// outFile1 << "snake head x " << game.snake.body[0].x << std::endl;
		// outFile1 << "snake head y " << game.snake.body[0].y << std::endl;
		// outFile1 << "food position x " << game.food.position.x << std::endl;
		// outFile1 << "food position y " << game.food.position.y << std::endl;
		// outFile1 << "action " << action << std::endl;
		// outFile1 << "reward " << reward << std::endl;
		// outFile1 << "Q values original" << std::endl;

		// for (const auto& value : q_values) {
		// 	outFile1 << value << std::endl;  // Write each value followed by a newline
		// }

		// // Decide if a terminal state has been reached. 
		// // ! inversion operator for bools, that is if the game is running, then
		// // game is done and vice versa.
		// bool done = !game.game_running;

		// // Bring state, 
		// ReplayMemory::Experience experience = {state, action, reward, next_state, done};

		// Store experience
		// dqn.replay_memory.storeExperience(experience);

		// If episode has reached update threshold, update the target neural network with 
		// policy network's current values.
		// if (episode % network_params.TARGET_UPDATE == 0) {
		// 	dqn.updateTargetNet();
		// }

		// Train the neural network with batch size.
		// dqn.train(network_params.BATCH_SIZE);

		// Check if Q values have been updated.
		// outFile1 << "Q values after training >> should be updated" << std::endl;
		// std::vector<float> new_q_values = dqn.policy_net.forward(state);

		// for (const auto& value : new_q_values) {
		// 	outFile1 << value << std::endl;  // Write each value followed by a newline
		// }

		// std::cout << "episode: " << episode << std::endl;
		// std::cout << "action: " << action << " ::: reward:" << reward << std::endl;

		// Drawing the background graphics
		ClearBackground(game_params.green);
		DrawRectangleLinesEx(Rectangle{(float)game_params.offset-5, (float)game_params.offset-5, (float)game_params.cell_size * game_params.cell_count + 10, (float)game_params.cell_size * game_params.cell_count + 10}, 5, game_params.dark_green);
		DrawText("Retro Snake", game_params.offset - 5, 20, 40, game_params.dark_green);
		DrawText(TextFormat("%i", game.score), game_params.offset - 5, game_params.offset + game_params.cell_size * game_params.cell_count + 10, 40, game_params.dark_green);
		game.draw();
		EndDrawing();

		// episode++;
	}

	// output the weights
	// for (auto& layer : dqn.policy_net.layers) {
	// 	outFile2 << "layer" << std::endl;
	// 	for (auto& row : layer.weights) {
	// 		outFile2 << "row " << std::endl;
	// 		for (auto& weight : row) {
	// 			outFile2 << weight << " ";
	// 		} outFile2 << std::endl;
	// 	}

	// 	outFile3 << "bias" << std::endl;
	// 	for (auto& bias : layer.biases) {
	// 		outFile3 << bias << " ";
	// 	} outFile3 << std::endl;
	// }

	// // Close the files
	// outFile2.close();
	// outFile3.close();
	// outFile1.close();
	CloseWindow();
	return 0;
}