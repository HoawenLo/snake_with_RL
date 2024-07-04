
// Based to DQN class.
struct networkParams {
    // Determines the memory capacity.
    int MEMORY_CAPACITY = 10000;
    
    // Determines the selection of action.
    // Determines the balance between exploration and exploitation.
    const double GAMMA = 0.99;
    const double EPSILON_START = 1.0;
    const double EPSILON_END = 0.1;
    const double EPSILON_DECAY = 1000;
    const int TARGET_UPDATE = 10;
    int steps_done = 0;

    // Neural network parameters.
    double learning_rate = 0.01;
    double sampling_threshold = 1000; // The point in which you start training once the memory capacity is full enough.

    // Q Learning parameters
    double gamma = 0.99;
    int update_threshold = 10; // number of training iterations of policy network until target network can be updated.

    // Reward parameters
    double food_reward = 100.0;
    double self_collision_penalty = -10.0;
    double edge_collision_penalty = -10.0;
    double general_time_penalty = 0.1;
};

