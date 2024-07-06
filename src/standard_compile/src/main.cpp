#include <iostream>

#include "../include/dqn.h"
#include "../include/game.h"
#include "../include/game_params.h"


int main() {

	GameParams game_params;
	NetworkParams network_params;

	// Total window size includes two border regions (the offset) and game region (cell_size / cell_count).
	// See game_params.h for values.
	InitWindow(game_params.game_window_height, game_params.game_window_width, game_params.game_title);
	SetTargetFPS(60);

	// Create game object, with game active to false, score to zero, passing the game configuration
	// and the last update time to zero.
	Game game = Game(false, 0, game_params, 0);

	// Create dep q network object, with input size 10, being the state size, 4 the output size, representing
	// the 4 output q values representing the 4 actions, 10000 memory capacity and parameters of the deep q network.	
	DQN dqn = DQN(10, 4, network_params.MEMORY_CAPACITY, network_params);

	// Initialise epsiode number.
	int episode = 0;

	// Initialise actions
	enum Actions { UP, DOWN, LEFT, RIGHT };


	// Game loop:
	// Check is the esc key pressed to close the window.
	while (WindowShouldClose() == false) {

		BeginDrawing();

		// Set the game updates to specific interval.
		if (game.eventTriggered(0.2)) {

			// Increment episode
			episode++;

			// Get state and decide action.
			std::vector<float> state = getState(game.snake, game.food);

			int action = dqn.selectAction(state, dqn.policy_net, episode);

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

			// Once action has been taken update the graphics of the game.
			game.update();

			// Get the reward from action being done.
			float reward = getReward(game.snake, game.food);

			// Get next state.
			std::vector<float> next_state = getState(game.snake, game.food);

			// Decide if a terminal state has been reached. 
			// ! inversion operator for bools, that is if the game is running, then
			// game is done and vice versa.
			bool done = !game.game_running;

			// Bring state, 
			ReplayMemory::Experience experience = {state, action, reward, next_state, done};

			// Store experience
			dqn.replay_memory.storeExperience(experience);

			// If episode has reached update threshold, update the target neural network with 
			// policy network's current values.
			if (episode % network_params.TARGET_UPDATE == 0) {
				dqn.updateTargetNet();
			}

			// Train the neural network with batch size.
			dqn.train(network_params.BATCH_SIZE);

			std::cout << "episode: " << episode << std::endl;
			std::cout << "action: " << action << " ::: reward:" << reward << std::endl;
		}

		// if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
		// 	game.snake.direction = {0, -1};
		// 	game.game_running = true;
		// }
		// if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
		// 	game.snake.direction = {0, 1};
		// 	game.game_running = true;
		// }
		// if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
		// 	game.snake.direction = {-1, 0};
		// 	game.game_running = true;
		// }
		// if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
		// 	game.snake.direction = {1, 0};
		// 	game.game_running = true;
		// }

		// Output the current episode number
		

		// Drawing the background graphics
		ClearBackground(game_params.green);
		DrawRectangleLinesEx(Rectangle{(float)game_params.offset-5, (float)game_params.offset-5, (float)game_params.cell_size * game_params.cell_count + 10, (float)game_params.cell_size * game_params.cell_count + 10}, 5, game_params.dark_green);
		DrawText("Retro Snake", game_params.offset - 5, 20, 40, game_params.dark_green);
		DrawText(TextFormat("%i", game.score), game_params.offset - 5, game_params.offset + game_params.cell_size * game_params.cell_count + 10, 40, game_params.dark_green);
		game.draw();
		EndDrawing();
	}
	CloseWindow();
	return 0;
}