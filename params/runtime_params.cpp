#include "runtime_params.hpp"

// функция для удаления пробелов из строки + преобразование в нижний регистр
std::string convert_type(const std::string& input) {
    std::string output;
    int str_size = input.size();

    for (int i = 0; i < str_size; ++i) {
        if (input[i] == ' ') continue;
        output.push_back(std::tolower(input[i]));
    }

    if (output == "float") {
        output = "float(32,0)";
    }
    else if (output == "double") {
         output = "float(64,0)";
    }

    return output;
}

// Спарсить параметры запуска в структуру
Params get_runtime_params(int argc, char* argv[]) {
    // словарь для хранения параметров
    std::unordered_map<std::string, std::string> args;

    // поиск по аргументам командной строки
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.rfind("--", 0) != 0) {
            continue;
        }

        // разделим аргумент на ключ:значение
        size_t eq_pos = arg.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = arg.substr(2, eq_pos - 2);
            std::string value = arg.substr(eq_pos + 1);
            args[key] = value;
        }
    }

    // заполняем структуру
    Params p;
    if (args.find("p-type") != args.end()) {
        p.p_type = convert_type(args["p-type"]);
    } else {
        p.p_type = DEFAULT_TYPE;
    }

    if (args.find("v-type") != args.end()) {
        p.v_type = convert_type(args["v-type"]);
    } else {
        p.v_type = DEFAULT_TYPE;
    }

    if (args.find("v-flow-type") != args.end()) {
        p.v_flow_type = convert_type(args["v-flow-type"]);
    } else {
        p.v_flow_type = DEFAULT_TYPE;
    }

    if (args.find("save-tick") != args.end()) {
        p.save_tick = std::stoi(args["save-tick"]);
    } else {
        p.save_tick = 0;
    }

    if (args.find("load-tick") != args.end()) {
        p.load_tick = std::stoi(args["load-tick"]);
    } else {
        p.load_tick = 0;
    }
    
    return p;
}

// Спарсить значения {type_name, N, K} из значения формата "type_name(N,K)"
std::pair<int, int> extract_params(const std::string& params_string, std::string& type_name) {
    std::size_t open_pos = params_string.find('(');
    std::size_t close_pos = params_string.find(')');
    std::size_t comma_pos = params_string.find(',');

    if (open_pos == std::string::npos || close_pos == std::string::npos || comma_pos == std::string::npos) {
        throw std::invalid_argument("Invalid parameter string format: missing '(' or ')' or ','");
    }

    std::string numbers_substr = params_string.substr(open_pos + 1, close_pos - open_pos - 1);
    std::size_t local_comma_pos = numbers_substr.find(',');

    int n = std::stoi(numbers_substr.substr(0, local_comma_pos));
    int k = std::stoi(numbers_substr.substr(local_comma_pos + 1));
    type_name = params_string.substr(0, open_pos);

    return {n, k};
}