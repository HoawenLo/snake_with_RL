#ifndef LAYER_H
#define LAYER_H

#include <vector>

float relu(float x);
float relu_derivative(float x);

class Layer {
public:
    std::vector<std::vector<float>> weights;
    std::vector<float> biases;
    std::vector<float> outputs;
    std::vector<float> inputs;
    std::vector<float> deltas;
    float learning_rate;

    Layer(int input_size, int output_size, randomGenerator& rng, float learning_rate);

    std::vector<float> forward(const std::vector<float>& input);
    std::vector<float> backward(const std::vector<float>& grad);
};

#endif