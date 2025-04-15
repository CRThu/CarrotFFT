#include <iostream>
#include <fstream>
#include <format>

#include "load_data.hpp"

std::vector<double> load_data(const std::string& dir, const std::string& obj)
{
    std::string path = std::format("{}/{}.txt", dir, obj);

    std::vector<double> data;
    double value;

    std::ifstream file(path);
    if (!file) {
        std::cerr << "cannot open file: " << path << std::endl;
        return data;
    }


    while (file >> value)
    {
        data.push_back(value);
    }

    return data;
}