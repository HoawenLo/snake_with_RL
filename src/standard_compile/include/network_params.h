#ifndef NETWORK_PARAMS_H
#define NETWORK_PARAMS_H


// Based to DQN class.
struct NetworkParams {
    // Determines the memory capacity.
    int MEMORY_CAPACITY = 10000;
    
    // Determines the selection of action.
    // Determines the balance between exploration and exploitation.
    const float EPSILON_START = 1.0; 
    const float EPSILON_END = 0.1;
    const float EPSILON_DECAY = 1000;
    
    int steps_done = 0;
    int MINIMUM_EXPLORATION_THRESHOLD = 100; // The minimum amount of episodes until exploitation can be used. 
                                              // That is replay memory needs to be full enough. This value needs 
                                              // to be higher than sampling threshold. The reason there are too 
                                              // seperate values for this is perhaps you can continue exploration
                                              // but start training. Hence once you have both enough training and
                                              // replay memory you can start using

    // Neural network parameters.
    float LEARNING_RATE = 0.01; // Neural network parameter step update.
    int SAMPLING_THRESHOLD = 100; // The point in which you start training once the memory capacity is full enough.
    int BATCH_SIZE = 32;

    // Q Learning parameters
    float gamma = 0.99; // Balance between focus on immediate and future rewards.
    const int TARGET_UPDATE = 50;; // number of training iterations of policy network until target network can be updated.

    // Reward parameters
    float food_reward = 100.0;
    float self_collision_penalty = -10.0;
    float edge_collision_penalty = -10.0;
    float general_time_penalty = 0.1;
};

#endif