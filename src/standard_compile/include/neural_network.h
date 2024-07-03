#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <iostream>
#include <vector>

#include "../include/layer.h"
#include "../include/random_generator.h"

class NeuralNetwork {
public:
    std::vector<Layer> layers;
    double learning_rate;

    NeuralNetwork(double learning_rate);

    void add_layer(int input_size, int output_size);
    std::vector<double> forward(const std::vector<double>& input);
    void backward(const std::vector<double>&grad);
};

#endif