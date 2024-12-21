#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

#pragma once

struct StartValues {
    double rho_space;
    double rho_dot;
    double g;
    int N;
    int M;
    std::vector<std::string> field;
};

StartValues get_start_values(const std::string& filename);


