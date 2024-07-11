#ifndef FILE_READER_H
#define FILE_READER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::vector<std::vector<float>>> read_in_weights (std::string filepath);
std::vector<std::vector<float>> read_in_biases(std::string filepath);

#endif FILE_READER_H