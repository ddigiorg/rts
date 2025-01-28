// File.hpp
# pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

std::string readFile(const std::string& filepath) {

    // open the file
    std::ifstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file: " + filepath);
    }

    // read the file contents into a stringstream
    std::ostringstream buffer;
    buffer << file.rdbuf();

    // return the file contents as a string
    return buffer.str();
}