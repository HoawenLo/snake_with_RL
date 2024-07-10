#include <algorithm>
#include <iostream>
#include <random>

#include "../include/layer.h"

/*
    Function: relu

    Description: Relu activation function for forward propagation.

    Arguments:
        (float) x: Input value.
    
    Returns:
        (float) Returns input value if positive.
*/ 
float relu(float x) {
    return std::max(0.0f, x);
}

/*
    Function: relu_derivative

    Description: Relu derivative activation function for back propagation.

    Arguments:
        (float) x: Input value.
    
    Returns:
        (float) Returns one if input is positive, else return zero.
*/ 
float relu_derivative(float x) {
    return x > 0 ? 1.0 : 0.0;
}

/*
    Class: Layer

    Component: Constructor

    Name: Layer

    Description: Constructor to initialise random values for weights and biases for each layer.

    Arguments:
        (int) input_size: The input size of the layer.
        (int) output_size: The output size of the layer.
        (float) learning_rate: Learning rate of the neural network.

    
    Returns:
        None

    Code Explanation:

        Code:

        weights.resize(output_size, std::vector<float>(input_size));

        Explanation:

        
                

        Code:

        biases.resize(output_size);
        
        Explanation:

        The biases variable is a std::vector. Resize it to the output size.

        Code:

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-0.1, 0.1);

        Explanation:

        Setup random number generator.

        Code:

        for (auto& row : weights) {
                for (auto& val : row) {
                    val = rng.dis(rng.gen);
                }
            }

        for (auto& val : biases) {
                val = rng.dis(rng.gen);
            }

        Explanation:

        For each weight and bias, set them to a random value.

*/ 
Layer::Layer(int input_size, int output_size, float learning_rate) : learning_rate(learning_rate) {
    weights.resize(output_size, std::vector<float>(input_size));
    biases.resize(output_size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 0.1);

    for (auto& row : weights) {
        for (auto& val : row) {
            val = dis(gen);
        }
    }
    // initialise biases to avoid dead neurons > trying to figure out cause of q values convergine to zero
    for (auto& val : biases) {
        val = 0.01;
    }
}

/*
    Class: Layer

    Component: Method

    Name: forward

    Description: Perform forward propagation for the layer.

    Arguments:
        (std::vector<float>) input: A vector containing the tensor input to be passed
            through the layer. Tensor is of size input_size.
    
    Returns:
        (std::vector<float>) Returns a vector of size output_size.

    Code Explanation:

        Code:

        inputs = input;

        Explanation:

        Copy the input to be used later in back propagation.

        Code:

        outputs.resize(biases.size());

        Explanation:

        Resize the outputs vector to be the same size of the biases. This will store
        the outputs.

        Code:

        for (size_t i = 0; i < biases.size(); i++) {
            outputs[i] = biases[i];
            for (size_t j = 0; j < input.size(); j++) {
                outputs[i] += weights[i][j] * input[j];
            } outputs[i] = relu(outputs[i]);
        }

        Explanation:
        
        We will perform forward pass which is 
        
        y = activation_function(sum(w_n * x_n) + bias).

        So we iterate over each output value in the vector, add the bias.
        Then calculate sum(w_n * x_n) and add that to each output value.
        Finally for each output value put them through the activation 
        function.
*/ 
std::vector<float> Layer::forward(const std::vector<float>& input) {

    inputs = input;
    outputs.resize(biases.size());
    for (size_t i = 0; i < biases.size(); i++) {
        outputs[i] = biases[i];
        for (size_t j = 0; j < input.size(); j++) {
            outputs[i] += weights[i][j] * input[j];
        } outputs[i] = relu(outputs[i]);
    }
    return outputs;
}

/*
    Class: Layer

    Component: Method

    Name: backward

    Description: Perform back propagation for the layer.

    Arguments:
        (std::vector<float>) grad: A vector of size output_size containing the input gradients 
            for back propagation calculation.
    
    Returns:
        (std::vector<float>) Returns a vector of size input_size containing the gradients for
            the next layer.

    Code Explanation:

        Code:

        deltas.resize(inputs.size());

        Explanation:

        Resize the gradients vector that will be passed onto next layer. Because we are moving
        backwards it has a size input_size.

        Code:

        std::fill(deltas.begin(), deltas.end(), 0.0)

        Explanation:

        Fill the deltas vector with 0.0. We are initialising the deltas vector.

        Code:

        for (size_t i = 0; i < outputs.size(); ++i) {
            float delta = grad[i] * relu_derivative(outputs[i]);
            ...

        Explanation:

        Iterate over each gradient and multiply with activation function gradient to
        get a gradient to calculate the loss contributions of each weight and bias.
        Hence iteration size will be size outputs.size().

        Code:

        for (size_t j = 0; j < inputs.size(); j++) {
            deltas[j] += delta * weights[i][j];

        Explanation:

        Calculate the gradients for next layer, hence iteration will be size inputs.size().
        Calculation of gradients to be passed onto next layer is

        dLoss / dLayerInputs = Weights transposed . grads_prev_layer
        dLoss / dLayerInputs = sum(grads_prev_layer * weights)

        Code:

        for (size_t j = 0; j < inputs.size(); ++j)
            weights[i][j] -= learning_rate * delta * inputs[j];

        Explanation:

        Calculate the update value for each weight. This is calculated with

        dLoss / dWeights = grads_prev_layer . inputs for current layer

        The learning rate controls the step size updates. The smaller the learning
        rate the slower but smoother the convergence.

        Code:

        biases[i] -= learning_rate * delta;

        Explanation:

        Calculate the update values for bias, which is the following

        dLoss / dBiases = grads_prev_layer
*/ 
std::vector<float> Layer::backward(const std::vector<float>& grad) {
    deltas.resize(inputs.size());
    std::fill(deltas.begin(), deltas.end(), 0.0);

    for (size_t i = 0; i < outputs.size(); i++) {
        float delta = grad[i] * relu_derivative(outputs[i]);
        for (size_t j = 0; j < inputs.size(); j++) {
            deltas[j] += delta * weights[i][j];
        } for (size_t j = 0; j < inputs.size(); j++) {
            weights[i][j] -= learning_rate * delta * inputs[j];
        } biases[i] -= learning_rate * delta;
    } return deltas;
}


void save_weights() {
    
}

