#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <iostream>
#include <vector>

#include "../include/layer.h"
#include "../include/random_generator.h"

class NeuralNetwork {
public:
    std::vector<Layer> layers;
    float learning_rate;

    NeuralNetwork(float learning_rate);

    void add_layer(int input_size, int output_size);
    std::vector<float> forward(const std::vector<float>& input);
    void backward(const std::vector<float>&grad);
};

#endif