#include "../include/dqn.h"
#include "../include/game.h"
#include "../include/game_params.h"
#include "../include/network_params.h"

/*
    Function: getReward

    Description: Get the reward.

    Arguments:
        (Snake) snake: The snake object.
        (Food) food: The food object.
     
    Returns:
        (double) A reward. See the training_params.h header file for the values.
            Positive rewards will encourage snake to win, whilst negative rewards,
            will discourage snake from behaviours that will cause it to lose.

    Code explanation:

    Code:

    gameParams game_params; 
    networkParams training_params;

    double food_reward = training_params.food_reward;
    double self_collision_penalty = training_params.self_collision_penalty;
    double edge_collision_penalty = training_params.edge_collision_penalty;
    double general_time_penalty = training_params.general_time_penalty;
    double cellCount = game_params.cellCount

    Explanation:

    Initialise parameters. Retrieve relevant reward parameters.

    Code:

    if (Vector2Equals(snake.body[0], food.position)) return food_reward;

    Explanation:

    If snake head is same position as good, hence eats a food, return the
    food reward.

    Code:

    if (ElementInDeque(snake.body[0], snake.body)) return self_collision_penalty;

    Explanation:

    If snake head is same position as part of its body, hence it has collided, 
    with itself return self collision penalty.

    Code:

    if (snake.body[0].x < 0 || snake.body[0].x >= cellCount || snake.body[0].y < 0 || snake.body[0].y >= cellCount) return edge_collision_penalty;

    Explanation:

    If snake head is past one of the borders of the game, vertical or horizontal, 
    return edge collision penalty.

    Code:

    return general_time_penalty;

    Explanation:

    Return general time penalty to encourage snake to find food in fast as time
    possible.

*/ 
double getReward(const Snake &snake, const Food &food) {
    gameParams game_params; 
    networkParams training_params;

    double food_reward = training_params.food_reward;
    double self_collision_penalty = training_params.self_collision_penalty;
    double edge_collision_penalty = training_params.edge_collision_penalty;
    double general_time_penalty = training_params.general_time_penalty;
    double cellCount = game_params.cellCount;

    if (Vector2Equals(snake.body[0], food.position)) return food_reward;
	if (ElementInDeque(snake.body[0], snake.body)) return self_collision_penalty;
	if (snake.body[0].x < 0 || snake.body[0].x >= cellCount || snake.body[0].y < 0 || snake.body[0].y >= cellCount) return edge_collision_penalty;
	return general_time_penalty;
}

/*
    Function: getState

    Description: Get the state to be stored in replay memory. State consists of
        the positions of the food and snake head, directions the snake is facing
        and whether there are any obstacles.

    Arguments:
        (Snake) snake: The snake object.
        (Food) food: The food object.
     
    Returns:
        (std::vector<float>) The state as a vector.

    Code explanation:

    Code:

    Vector2 head = snake.body[0];
	Vector2 foodPos = food.position;
	Vector2 direction = snake.direction;
    gameParams params;

	bool obstacleUp = false, obstacleDown = false, obstacleLeft = false, obstacleRight = false;

    Explanation:

    Initialise key variables, the snake head position, the food position, the snake's current
    direction, the game parameters, whether there are any obstacles.

    Code:

    Explanation:


    Code:

    Explanation:
    

    Code:

    Explanation:

*/ 
std::vector<float> getState(const Snake &snake, const Food &food) {
	Vector2 head = snake.body[0];
	Vector2 foodPos = food.position;
	Vector2 direction = snake.direction;
    gameParams params;

	bool obstacleUp = false, obstacleDown = false, obstacleLeft = false, obstacleRight = false;

	// Check for obstacles in each direction.
	// ElementInDeque iterates over all elements in the deque and it it finds
	// the input element in the deque it returns true.
	// In this case iterate over elements in the snake body and see if the snake
	// is pointing towards its own body (ElementInDeque) or (||) it is pointing
	// to outside the game boundary.
	if (ElementInDeque(Vector2{head.x, head.y - 1}, snake.body) || head.y - 1 < 0) obstacleUp = true;
	if (ElementInDeque(Vector2{head.x, head.y + 1}, snake.body) || head.y + 1 >= params.cellCount) obstacleDown = false;
	if (ElementInDeque(Vector2{head.x - 1, head.y}, snake.body) || head.x - 1 < 0) obstacleLeft = true;
	if (ElementInDeque(Vector2{head.x + 1, head.y}, snake.body) || head.x + 1 >= params.cellCount) obstacleRight = true;

	float normalisedHeadX = head.x / (float)params.cellCount;
	float normalisedHeadY = head.y / (float)params.cellCount;
	float normalisedFoodX = foodPos.x / (float)params.cellCount;
	float normalisedFoodY = foodPos.y / (float)params.cellCount;

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

