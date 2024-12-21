#include <array>
#include <tuple>
#include <string_view>
#include <iostream>

#pragma once

// макросы для преобразования
#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

// получение TYPES в зависимости от наличия -DTYPES 
#ifdef TYPES
constexpr std::string_view DTYPES = EXPAND_AND_STRINGIFY(TYPES);
#else
constexpr std::string_view DTYPES = "float(64,0)";
#endif

// удаление лишних символов
constexpr std::string_view string_preprocessing(std::string_view input) {
    if (input.front() == '"' && input.back() == '"') {
        input = input.substr(1, input.size() - 2);
    }
    if (input.front() == '(' && input.back() == ')') {
        input = input.substr(1, input.size() - 2);
    }
    if (input.front() == '\'' && input.back() == '\'') {
        input = input.substr(1, input.size() - 2);
    }
    return input;
}

// constexpr функция для подсчета количества типов в DTYPES
constexpr int calculate_dtypes_size(std::string_view input) {
    int count = 1;
    size_t pos = 0;

    while (pos < input.size()) {
        size_t sep = input.find(' ', pos);
        if (sep == std::string_view::npos) break;
        pos = sep + 1;
        ++count;
    }
    return count;
}

constexpr int DTYPES_SIZE = calculate_dtypes_size(DTYPES); // определение DTYPES_SIZE (кол-во типов)

// constexpr функция для парсинга целого числа из строки
constexpr int parse_int(std::string_view str) {
    int result = 0;
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            result = result * 10 + (c - '0');
        }
    }
    return result;
}

// Преобразование имени типа
constexpr std::string_view transform_type_name(std::string_view type_name) {
    if (type_name == "FIXED") return "fixed";
    if (type_name == "FAST_FIXED") return "fast_fixed";
    if (type_name == "FLOAT") return "float";
    if (type_name == "DOUBLE") return "float";
    return type_name; // Неизвестный тип
}

// функция для парсинга строки DTYPES в массив std::tuple
constexpr auto parse_dtypes(std::string_view input) {
    input = string_preprocessing(input);
    std::array<std::tuple<std::string_view, int, int>, DTYPES_SIZE> result{};

    size_t pos = 0;
    size_t idx = 0;

    while (pos < input.size() && idx < DTYPES_SIZE) {
        size_t start = input.find_first_not_of(", ", pos);
        if (start == std::string_view::npos) break;

        size_t open_bracket = input.find('(', start);
        size_t close_bracket = input.find(')', open_bracket);

        if (open_bracket != std::string_view::npos && close_bracket != std::string_view::npos) {
            // Тип с параметрами в скобках
            std::string_view type_name = input.substr(start, open_bracket - start);
            std::string_view params = input.substr(open_bracket + 1, close_bracket - open_bracket - 1);

            size_t comma_pos = params.find(',');
            int n = parse_int(params.substr(0, comma_pos));
            int k = parse_int(params.substr(comma_pos + 1));

            result[idx++] = {transform_type_name(type_name), n, k};
            pos = close_bracket + 1;
        } else {
            // Тип без параметров в скобках
            size_t end = input.find(',', start);
            if (end == std::string_view::npos) end = input.size();

            std::string_view type_name = input.substr(start, end - start);
            
            int n, k;
            if (type_name == "DOUBLE") {
                n = 64; k = 0; 
            } else if (type_name == "FLOAT") {
                n = 32; k = 0;
            }

            result[idx++] = {transform_type_name(type_name) , n, k};
            pos = end + 1;
        }
    }

    return result;
}   

constexpr auto parsed_dtypes = parse_dtypes(DTYPES); // кэширование результатов парсинга (чтобы не вызвать много раз)

// constexpr функции для получения массивов всех возможных type, N, K
constexpr auto get_allowed_types() {
    std::array<std::string_view, DTYPES_SIZE> types{};
    for (size_t i = 0; i < DTYPES_SIZE; ++i) {
        types[i] = std::get<0>(parsed_dtypes[i]);
    }
    return types;
}

constexpr auto get_allowed_N() {
    std::array<int, DTYPES_SIZE> N{};
    for (size_t i = 0; i < DTYPES_SIZE; ++i) {
        N[i] = std::get<1>(parsed_dtypes[i]);
    }
    return N;
}

constexpr auto get_allowed_K() {
    std::array<int, DTYPES_SIZE> K{};
    for (size_t i = 0; i < DTYPES_SIZE; ++i) {
        K[i] = std::get<2>(parsed_dtypes[i]);
    }
    return K;
}
