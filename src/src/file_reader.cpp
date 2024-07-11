#include "../include/file_reader.h"

std::vector<std::vector<std::vector<float>>> read_in_weights (std::string filepath) {
    std::ifstream file(filepath);
    std::string line;
    std::vector<std::vector<std::vector<float>>> weights;

    int layer_number = -1;

    while (std::getline(file, line)) {
        if (line == "layer") {
            weights.push_back(std::vector<std::vector<float>>());
            layer_number++;
            continue;
        }

        std::istringstream iss(line);
        std::string row_marker;
        iss >> row_marker;

        if (row_marker == "row") {
            weights[layer_number].push_back(std::vector<float>());
            float weight;
            while (iss >> weight) {
                weights[layer_number].back().push_back(weight);
            }
        }
    }

    return weights;
}

std::vector<std::vector<float>> read_in_biases(std::string filepath) {
    std::ifstream file(filepath);
    std::string line;
    std::vector<std::vector<float>> biases;

    while (std::getline(file, line)) {
        if (line == "bias") {
            biases.push_back(std::vector<float>());
            continue;
        }

        std::istringstream iss(line);
        float bias;
        while (iss >> bias) {
            biases.back().push_back(bias);
        }
    }
    return biases;
}