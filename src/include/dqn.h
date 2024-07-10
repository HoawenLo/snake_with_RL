#ifndef DQN_H
#define DQN_H

#include <iostream>
#include <random>
#include <vector>

#include "../include/game.h"
#include "../include/neural_network.h"
#include "../include/network_params.h"

float getReward(const Snake &snake, const Food &food, const Vector2 &previous_head_position);
std::vector<float> getState(const Snake &snake, const Food &food);

class ReplayMemory {
public:
    struct Experience {
        std::vector<float> state;
        int action;
        float reward;
        std::vector<float> next_state;
        bool done;
    };

    std::vector<Experience> memory;
    size_t capacity;
    size_t position;

    ReplayMemory(size_t capacity);

    void storeExperience(const Experience& experience);
    std::vector<Experience> sample(size_t batch_size);
};

class DQN {
public:
    NeuralNetwork policy_net;
    NeuralNetwork target_net;
    ReplayMemory replay_memory;
    NetworkParams params;
    int steps_done;

    DQN(int input_size, int output_size, size_t memory_capacity, const NetworkParams& params);

    void updateTargetNet();
    void train(int batch_size);
    int argmax(std::vector<float> q_values);
    int selectActionTrain(const std::vector<float>& state, NeuralNetwork policy_net, int episode_number);
    int selectActionTest(const std::vector<float>& state, NeuralNetwork policy_net);
};

#endif