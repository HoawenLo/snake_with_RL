#include <random>

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
        (float) A reward. See the training_params.h header file for the values.
            Positive rewards will encourage snake to win, whilst negative rewards,
            will discourage snake from behaviours that will cause it to lose.

    Code explanation:

    Code:

    GameParams game_params; 
    networkParams training_params;

    float food_reward = training_params.food_reward;
    float self_collision_penalty = training_params.self_collision_penalty;
    float edge_collision_penalty = training_params.edge_collision_penalty;
    float general_time_penalty = training_params.general_time_penalty;
    float cell_count = game_params.cell_count

    Explanation:

    Initialise parameters. Retrieve relevant reward parameters.

    Code:

    if (Vector2Equals(snake.body[0], food.position)) return food_reward;

    Explanation:

    If snake head is same position as good, hence eats a food, return the
    food reward.

    Code:

    if (elementInDeque(snake.body[0], snake.body)) return self_collision_penalty;

    Explanation:

    If snake head is same position as part of its body, hence it has collided, 
    with itself return self collision penalty.

    Code:

    if (snake.body[0].x < 0 || snake.body[0].x >= cell_count || snake.body[0].y < 0 || snake.body[0].y >= cell_count) return edge_collision_penalty;

    Explanation:

    If snake head is past one of the borders of the game, vertical or horizontal, 
    return edge collision penalty.

    Code:

    return general_time_penalty;

    Explanation:

    Return general time penalty to encourage snake to find food in fast as time
    possible.

*/ 
float getReward(const Snake &snake, const Food &food) {
    GameParams game_params; 
    networkParams training_params;

    float food_reward = training_params.food_reward;
    float self_collision_penalty = training_params.self_collision_penalty;
    float edge_collision_penalty = training_params.edge_collision_penalty;
    float general_time_penalty = training_params.general_time_penalty;
    float cell_count = game_params.cell_count;

    if (Vector2Equals(snake.body[0], food.position)) return food_reward;
	if (elementInDeque(snake.body[0], snake.body)) return self_collision_penalty;
	if (snake.body[0].x < 0 || snake.body[0].x >= cell_count || snake.body[0].y < 0 || snake.body[0].y >= cell_count) return edge_collision_penalty;
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

    if (elementInDeque(Vector2{head.x, head.y - 1}, snake.body) || head.y - 1 < 0) obstacleUp = true;
	if (elementInDeque(Vector2{head.x, head.y + 1}, snake.body) || head.y + 1 >= params.cell_count) obstacleDown = false;
	if (elementInDeque(Vector2{head.x - 1, head.y}, snake.body) || head.x - 1 < 0) obstacleLeft = true;
	if (elementInDeque(Vector2{head.x + 1, head.y}, snake.body) || head.x + 1 >= params.cell_count) obstacleRight = true;

    Explanation:

	Check for obstacles in each direction. elementInDeque iterates over all elements
    in the deque and it it finds the input element in the deque it returns true. In 
    this case iterate over elements in the snake body and see if the snake is pointing 
    towards its own body (elementInDeque) or (||) it is pointing to outside the game 
    boundary.

    Code:

    float normalisedHeadX = head.x / (float)params.cell_count;
	float normalisedHeadY = head.y / (float)params.cell_count;
	float normalisedFoodX = foodPos.x / (float)params.cell_count;
	float normalisedFoodY = foodPos.y / (float)params.cell_count;

    Explanation:
    
    Normalise position values for food and snake head.

    Code:

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

    Explanation:

    Combine all state values into a vector.
*/ 
std::vector<float> getState(const Snake &snake, const Food &food) {
	Vector2 head = snake.body[0];
	Vector2 foodPos = food.position;
	Vector2 direction = snake.direction;
    GameParams params;

	bool obstacleUp = false, obstacleDown = false, obstacleLeft = false, obstacleRight = false;

	if (elementInDeque(Vector2{head.x, head.y - 1}, snake.body) || head.y - 1 < 0) obstacleUp = true;
	if (elementInDeque(Vector2{head.x, head.y + 1}, snake.body) || head.y + 1 >= params.cell_count) obstacleDown = false;
	if (elementInDeque(Vector2{head.x - 1, head.y}, snake.body) || head.x - 1 < 0) obstacleLeft = true;
	if (elementInDeque(Vector2{head.x + 1, head.y}, snake.body) || head.x + 1 >= params.cell_count) obstacleRight = true;

	float normalisedHeadX = head.x / (float)params.cell_count;
	float normalisedHeadY = head.y / (float)params.cell_count;
	float normalisedFoodX = foodPos.x / (float)params.cell_count;
	float normalisedFoodY = foodPos.y / (float)params.cell_count;

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

/*
    Class: ReplayMemory

    Component: Constructor
    
    Name: ReplayMemory

    Description: Constructor takes inputs for memory capacity.

    Arguments:
        (size_t) Unsigned integer value, representing the memory capacity.
     
    Returns:
        None

    Code Explanation:

    Code:

    ReplayMemory::ReplayMemory(size_t capacity) : capacity(capacity), position(0)

    Explanation:

    Assign the capacity attribute to size capacity, then initialise starting position 
    to zero.

    Code:

    memory.resize(capacity);

    Explanation:

    Creates the memory to appropriate size to be filled with experiences.
*/

ReplayMemory::ReplayMemory(size_t capacity) : capacity(capacity), position(0) {
    memory.resize(capacity);
}

/*
    Class: ReplayMemory

    Component: Method
    
    Name: add

    Description: Add a new experience to replay memory.

    Arguments:
        (const Experience) The experience struct containing values to be added to
            replay memory. See the bottom for notes on experience struct.
     
    Returns:
        None

    Code Explanation:
    
    Code:

    memory[position] = experience;
    position = (position + 1) & capacity;

    Explanation:

    Fill the memory for current position. If position reaches the end of capacity 
    the remainder operator brings the value back to 0.
*/

void ReplayMemory::add(const Experience& experience) {
    memory[position] = experience;
    position = (position + 1) & capacity;
}

/*
    Class: ReplayMemory

    Component: Method
    
    Name: sample

    Description: Sample a vector of experiences from replay memory of size batch size. 

    Arguments:
        (size_t) batch_size: The size of the vector - the batch.
     
    Returns:
        (std::vector) A vector of size batch size, containing experiences from the replay
            memory.

    Code Explanation:
    
    Code:

    randomGenerator generator;
    std::vector<Experience> batch;
    std::vector<int> seen;
    std::uniform_int_distribution<> dist(0, memory.size() - 1);

    Explanation:

    Initialise key variables. Initialise random generator to randomly select experiences.
    Also initialise batch vector to hold experiences. Initialise seen vector to keep track
    of the positions of experiences added to batch to ensure that no same experience is 
    reused. Finally initialise the range to sample the positions values of the experiences
    in the replay memory vector.

    Code:

    int i = 0;
    while (i < batch_size) {
        int memory_pos = dist(generator.gen);

    Explanation:

    Use while loop to iterate until batch vector is filled. Generate a position value to
    pull an experience from the replay memory.

    Code:

    if (std::find(seen.begin(), seen.end(), memory_pos) == seen.end()) {
        batch.push_back(memory[memory_pos]);
        i++;
    }

    Explanation:

    Iterate through to check if the generated value for the position of the
    experience to add to the batch has already been seen. If it has not add
    it to the batch and increment i.
*/

std::vector<ReplayMemory::Experience> ReplayMemory::sample(size_t batch_size) {
    randomGenerator generator;
    std::vector<Experience> batch;
    std::vector<int> seen;
    std::uniform_int_distribution<> dist(0, memory.size() - 1);
    
    int i = 0;
    while (i < batch_size) {
        int memory_pos = dist(generator.gen);
        if (std::find(seen.begin(), seen.end(), memory_pos) == seen.end()) {
            batch.push_back(memory[memory_pos]);
            i++;
        }
    } return batch;
}

/*
    Class: DQN

    Component: Constructor
    
    Name: DQN

    Description: Constructor for deep Q network. Create the policy network and
        target network.

    Arguments:
        (int input_size) The input data size. This will be the size of the experience.
        (int output_size) The output values size. This is the output Q values which
            correspond to the snake's possible actions. This should be four.
        (size_t memory_capcacity) An unsigned integer representing the size of the
            memory capacity. This is passed onto the replay memory object.
        (const networkParams) params: The network parameters from the network_params.h
            file. These are hyperparameters.
     
    Returns:
        None

    Code Explanation:
    
    Code:

    DQN::DQN(int input_size, int output_size, size_t memory_capacity, const networkParams& params)
    : replay_memory(memory_capacity), 
      params(params),
      policy_net(params.learning_rate),
      target_net(params.learning_rate)
    {
        policy_net.add_layer(input_size, 64);
        policy_net.add_layer(128, 64);
        policy_net.add_layer(64, output_size);
        target_net = policy_net;
    }

    Explanation:

    Takes input arguments for constructor, passes the memory capacity to replay_memory attribute.
    In this case replay_memory() has an argument hence memory_capacity is passed to it. Whilst with
    gamma(), it has no arguments hence the input gamma passes as the attribute to the gamma attribute.
    This is also done with passing the learning rate parameter to the policy net and target net.

to add
// require seperate params file for network structure - to implement.
// error checker if sampling theshold is greater than memory capacity
*/
DQN::DQN(int input_size, int output_size, size_t memory_capacity, const networkParams& params)
    : replay_memory(memory_capacity), 
      params(params),
      policy_net(params.learning_rate),
      target_net(params.learning_rate)
    {
        policy_net.add_layer(input_size, 64);
        policy_net.add_layer(128, 64);
        policy_net.add_layer(64, output_size);
        target_net = policy_net;
    }

/*
    Class: DQN

    Component: Method
    
    Name: updateTargetNet

    Description: Update the parameters of the target network to the parameters of
        the policy network after a number of training iterations of the policy
        network.

    Arguments:
        None
     
    Returns:
        None

    Code Explanation:
    
    Code:

    target_net = policy_net;

    Explanation:

    Set the target_net to policy_net.
*/
void DQN::updateTargetNet() {
    target_net = policy_net;
}

/*
    Class: DQN

    Component: Method
    
    Name: train

    Description: Train the neural network model.

    Arguments:
        (int) batch_size: The training batch size.
     
    Returns:
        None

    Code Explanation:

    Code:

    if (replay_memory.capacity < params.sampling_threshold) {
        return;
    }

    Explanation:

    Ensure that the memory capacity has passed the sampling threshold to enable
    training to begin. Sampling threshold is set in network_params.h.

    Code:

    auto batch = replay_memory.sample(batch_size);

    Explanation:

    Sample a batch of experiences from the replay memory.

    Code:

    for (const auto& exp : batch) {
        auto q_values = policy_net.forward(exp.state);
        

    Explanation:

    Iterate through each experience in the batch. 

    Perform forward pass with the current state on the policy neural network.
    This outputs the predicted Q values. 

    Code:

    auto next_q_values = target_net.forward(exp.next_state);

    Explanation:

    Perform forward pass with the next state on the target neural network. 
    This outputs Q values for the next state, and will be used to calculate
    the target Q value (can be seen as a ground truth).

    Code:

    float q_update = exp.reward;

    Explanation:

    Retrieve the actual reward value from the action performed from being in 
    the current state. This will be used with the Q values output from the 
    target neural network with the next state inputted for the forward pass.

    Code:

    if (!exp.done) {
        q_update += gamma * *std::max_element(next_q_values.begin(), next_q_values.end());
    }

    Explanation:

    If not at terminal state, add the max Q value outputted from the target neural network.
    Here we calculate the temporal difference target estimate. This represents the actual
    output.

    Code:

    std::vector<float> target = q_values;

    Explanation:

    Set the target vector as the predicted Q values. At there are 4 outputs, for back
    propagation we need to match this. However since the agent (snake) only performs
    one action, we only have the actual reward for one action. Hence what we will do
    is just take the same Q values predicted by the policy neural network with the current
    weights and for the particular action performed, replace the actual Q value. The 
    idea is that the other Q values will zero out and provide no contribution, whilst 
    the actual Q value can be used with the predicted Q value to calculate a loss value.

    Code:

    std::vector<float> grad(target.size());

    Explanation:

    Setup the vector which holds the input gradients.

    Code:

    grad[i] = q_values[i] - target[i];

    Explanation:

    Calculate the input gradients for mean squared loss.

    MSE = 1/2 (Pred - Actual)^2
    dMSE = Pred - Actual
*/
void DQN::train(int batch_size) {
    if (replay_memory.capacity < params.sampling_threshold) {
        return;
    }

    auto batch = replay_memory.sample(batch_size);

    for (const auto& exp : batch) {
        auto q_values = policy_net.forward(exp.state); // Q_old

        auto next_q_values = target_net.forward(exp.next_state); // Q_target
        float q_update = exp.reward; // r_current

        if (!exp.done) {
            q_update += params.gamma * *std::max_element(next_q_values.begin(), next_q_values.end());
        } // TD target estimate - Q_actual

        // Only the action performed changed its Q value, hence predicted Q values will zero out
        // later when we calculate the loss.
        std::vector<float> target = q_values; 
        target[exp.action] = q_update; // Replace the actual Q value for the action performed.

        // The loss values are zero except for action performed as q_values[i] - target [i] = 0.
        std::vector<float> grad(target.size());
        for (size_t i = 0; i < target.size(); i++) {
            grad[i] = q_values[i] - target[i];
        }

        policy_net.backward(grad);
    }
}

/*
    Class: ReplayMemory

    Component: struct
    
    Name: Experience

    Description: The components of the experience struct. Experience struct holds
        the current state, the action taken, the reward, the next state and whether
        the current state has reached a terminal state. In this context it means that
        the snake has died. This is required as the q value can only take the immediate
        reward as there is no way of calculating the expected cumulative future reward
        as this is the final state. Hence the way expected cumulative future reward is
        calculated will be changed.

    Arguments:
        None
     
    Returns:
        None
*/