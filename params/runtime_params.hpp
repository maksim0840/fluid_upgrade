#include <string>
#include <unordered_map>
#include <cstdlib>
#include <stdexcept>

#pragma once

#define DEFAULT_TYPE "float(64,0)"

// Структура для хранения параметров
struct Params {
    std::string p_type; // тип для p
    std::string v_type; // тип для velocity
    std::string v_flow_type; // тип для velocity_flow
    int save_tick; // сохраняет каждый n-ый тик (если текущий номер тика кратен save_tick)
    int load_tick; // минимальный номер тика, с которого начать (программа начнёт с минимального тика, который >= load_tick)
};


// Функция для удаления пробелов из строки + преобразование в нижний регистр
std::string convert_type(const std::string& input);

// Спарсить параметры запуска в структуру
Params get_runtime_params(int argc, char* argv[]);

// Спарсить значения {type_name, N, K} из значения формата "type_name(N,K)"
std::pair<int, int> extract_params(const std::string& params_string, std::string& type_name);