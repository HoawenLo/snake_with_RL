#include "../include/random_generator.h"

void randomGenerator::initialiseRandomGenerator() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1, 0.1);
}

randomGenerator::randomGenerator() {
    initialiseRandomGenerator();
}