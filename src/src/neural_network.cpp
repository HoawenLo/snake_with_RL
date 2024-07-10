#include "../include/neural_network.h"

/*
    Class: NeuralNetwork

    Component: Constructor

    Name: NeuralNetwork

    Description: Set the neural network parameters.

    Arguments:
        (double) learning_rate: Neural network learning rate.
    
    Returns:
        None
*/ 
NeuralNetwork::NeuralNetwork(float learning_rate) : learning_rate(learning_rate) {};

/*
    Class: NeuralNetwork

    Component: Method

    Name: add_layer

    Description: Add a layer to the neural network.

    Arguments:
        (int) input_size: The input size of the layer.
        (int) output_size: The output size of the layer.
        (double) learning_rate: The learning rate of the neural network.
    
    Returns:
        None

    Code Explanation:

        Code:

        layers.emplace_back(input_size, output_size);

        Explanation:

        Create a layer object and place into layers vector without needing to perform copy
        or move operation.
*/ 
void NeuralNetwork::add_layer(int input_size, int output_size) {
    layers.emplace_back(input_size, output_size, learning_rate);
}

/*
    Class: NeuralNetwork

    Component: Method

    Name: forward

    Description: Perform forward propagation.

    Arguments:
        (std::vector<double>) input: Input data to perform forward propagation on.
    
    Returns:
        (std::vector<double>) A vector with the shape of the output of the neural
            network.

    Code Explanation:

        Code:

        std::vector<double> output = input;

        Explanation:

        Create a copy of the output vector to pass through the neural network.
*/ 
std::vector<float> NeuralNetwork::forward(const std::vector<float>& input) {
    std::vector<float> output = input;
    for (auto& layer : layers) {
        output = layer.forward(output);
    } return output;
}

/*
    Class: NeuralNetwork

    Component: Method

    Name: backward

    Description: Perform back propagation to update all weights and biases.

    Arguments:
        (std::vector<double>) grad: The input gradients which is the derivative of
            the loss value with respect to the predicted values.
    
    Returns:
        None

    Code Explanation:

        Code:

        for (auto it = layers.rbegin(); it != layers.rend(); ++it)
            delta = it->backward(delta);

        Explanation:

        Iterate through layers in reverse. Apply back propagation to each layer.
        
*/ 
void NeuralNetwork::backward(const std::vector<float>& grad) {
    std::vector<float> delta = grad;
    for (auto layer = layers.rbegin(); layer != layers.rend(); ++layer)
        delta = layer->backward(delta);
}

void NeuralNetwork::load_in_network_params(std::vector<std::vector<std::vector<float>>>& loaded_weights, std::vector<std::vector<float>>& loaded_biases) {


    if (loaded_weights.size() != layers.size() || loaded_biases.size() != layers.size()) {
        std::cout << loaded_weights.size() << std::endl;
        std::cout << loaded_biases.size() << std::endl;
        std::cout << layers.size() << std::endl;
        throw std::runtime_error("Mismatch in number of layers and loaded parameters");
    }

    for (int i = 0; i < layers.size(); i++) {
        layers[i].load_in_params(loaded_weights[i], loaded_biases[i]);
    }
}


