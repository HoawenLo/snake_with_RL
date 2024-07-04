#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>

struct randomGenerator {
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    void initialiseRandomGenerator();
    randomGenerator();
};

#endif