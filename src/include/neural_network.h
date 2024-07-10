#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <iostream>
#include <vector>

#include "../include/layer.h"

class NeuralNetwork {
public:
    struct AdamOptimiser {
        float lr;
        float beta1;
        float beta2;
        float epsilon;
        float m_weight;
        float m_bias;
        float v_weight;
        float v_bias;
        int t;

        AdamOptimiser (float learning_rate, float b1, float b2, float eps)
            : lr(learning_rate), beta1(b1), beta2(b2), epsilon(eps) {}

    };
    
    
    std::vector<Layer> layers;
    float learning_rate;

    NeuralNetwork(float learning_rate);

    void add_layer(int input_size, int output_size);
    std::vector<float> forward(const std::vector<float>& input);
    void backward(const std::vector<float>&grad);
    void load_in_network_params(std::vector<std::vector<std::vector<float>>>& loaded_weights, std::vector<std::vector<float>>& loaded_biases);
    
};

#endif