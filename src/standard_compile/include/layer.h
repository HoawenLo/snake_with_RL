#ifndef LAYER_H
#define LAYER_H

#include <vector>

double relu(double x);
double relu_derivative(double x);

class Layer {
public:
    std::vector<std::vector<double>> weights;
    std::vector<double> biases;
    std::vector<double> outputs;
    std::vector<double> inputs;
    std::vector<double> deltas;
    double learning_rate;

    Layer(int input_size, int output_size, randomGenerator& rng, double learning_rate);

    std::vector<double> forward(const std::vector<double>& input);
    std::vector<double> backward(const std::vector<double>& grad);
};

#endif