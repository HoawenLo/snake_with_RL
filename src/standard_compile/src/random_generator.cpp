#include "../include/random_generator.h"

/*
    Struct: randomGenerator
    
    Component: Method
    
    Name: initialiseRandomGenerator

    Description: Setup the random generator when the a random generator
        object is created. 

    Arguments:
        None
    
    Returns:
        None

    Code explanation:

    Code:

    std::random_device rd;

    Explanation:

    Create a random number, which serves as seed for the generator.

    Code:

    std::mt19937 gen(rd());

    Explanation:

    Pass the seed into the random generator.

    Code:

    std::uniform_real_distribution<> dis(-0.1, 0.1);

    Explanation:

    Create a distribution range in which numbers will be drawn from.
*/ 

void randomGenerator::initialiseRandomGenerator() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.1, 0.1);
}


/*
    Struct: randomGenerator
    
    Component: Constructor
    
    Name: randomGenerator

    Description: The constructor called when the randomGenerator object is created.
        Runs the initialiseRandomGenerator method.

    Arguments:
        None
    
    Returns:
        None
*/ 
randomGenerator::randomGenerator() {
    initialiseRandomGenerator();
}