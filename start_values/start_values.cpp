#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

struct StartValues {
    double rho_space;
    double rho_dot;
    double g;
    int N;
    int M;
    std::vector<std::string> field;
};

StartValues get_start_values(void) {
    StartValues start_values;
    
    std::string filename = "start_values/start_file";
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw "cannot open the file";
    }
    
    std::string line;
    std::getline(file, line); // пропустить первую строку-пояснение

    while (std::getline(file, line)) {
        if (line.empty()) continue; // пропустить пустую строку

        // Находим опорные позиции для получения значений
        size_t equal_pos = line.find('=');
        size_t end_pos = line.find(';');

        if (equal_pos != std::string::npos && end_pos != std::string::npos) {
            std::string key = line.substr(0, equal_pos);
            std::string value = line.substr(equal_pos + 1, end_pos - equal_pos - 1);

            // Удаляем пробелы
            value.erase(std::remove_if(value.begin(), value.end(), isspace), value.end());

            // Считываем значения
            if (key == "rho[' ']")
                start_values.rho_space = std::stod(value);
            else if (key == "rho['.']")
                start_values.rho_dot = std::stod(value);
            else if (key == "g")
                start_values.g = std::stod(value);
            else if (key == "N")
                start_values.N = std::stoi(value);
            else if (key == "M")
                start_values.M = std::stoi(value);
        }
        else if (line[0] == '\"' && line[line.size() - 1] == '\"') {
            if (line.size() > 1 && line.front() == '"' && line.back() == '"') {
                line = line.substr(1, line.size() - 2); // Удаляем первый и последний символ "
            }
            start_values.field.push_back(line);
        }
    }

    file.close();
    return start_values;
}
