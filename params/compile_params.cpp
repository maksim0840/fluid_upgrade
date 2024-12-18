#include <array>
#include <tuple>
#include <string_view>
#include <iostream>

#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)

// макросы для типов
#define FAST_FIXED(N, K) fastfixed(N, K)
#define FIXED(N, K) fixed(N, K)
#define FLOAT float(32, 0)
#define DOUBLE float(64, 0)

// получение TYPES в зависимости от наличия -DTYPES 
#ifdef TYPES
constexpr std::string_view DTYPES = EXPAND_AND_STRINGIFY(TYPES);
#else
constexpr std::string_view DTYPES = "float(64,0)";
#endif

// удаление начальных и конечных скобок
constexpr std::string_view trim_brackets(std::string_view input) {
    if (input.front() == '(' && input.back() == ')') {
        return input.substr(1, input.size() - 2);
    }
    return input;
}

// constexpr функция для подсчета количества типов в DTYPES
constexpr int calculate_dtypes_size(std::string_view input) {
    int count = 0;
    size_t pos = 0;

    while (pos < input.size()) {
        // найти открывающую скобку '('
        size_t open_bracket = input.find('(', pos);
        if (open_bracket == std::string_view::npos) break;

        // найти закрывающую скобку ')'
        size_t close_bracket = input.find(')', open_bracket);
        if (close_bracket == std::string_view::npos) break;

        // переместить указатель дальше
        pos = close_bracket + 1;
        ++count;
    }
    return count;
}

// определение DTYPES_SIZE (кол-во типов)
constexpr int DTYPES_SIZE = calculate_dtypes_size(trim_brackets(DTYPES));

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

// функция для парсинга строки DTYPES в массив std::tuple
constexpr auto parse_dtypes(std::string_view input) {
    input = trim_brackets(input);
    std::array<std::tuple<std::string_view, int, int>, DTYPES_SIZE> result{};

    size_t pos = 0;
    size_t idx = 0;

    while (pos < input.size() && idx < DTYPES_SIZE) {
        // находим начало типа
        size_t start = input.find_first_not_of(" ,", pos);
        size_t open_bracket = input.find('(', start);
        size_t close_bracket = input.find(')', open_bracket);

        if (start == std::string_view::npos || open_bracket == std::string_view::npos) break;

        std::string_view type_name = input.substr(start, open_bracket - start);
        std::string_view params = input.substr(open_bracket + 1, close_bracket - open_bracket - 1);

        // извлекаем параметры N и K
        size_t comma_pos = params.find(',');
        int n = parse_int(params.substr(0, comma_pos));
        int k = parse_int(params.substr(comma_pos + 1));

        // заполняем массив
        result[idx++] = {type_name, n, k};

        // переходим к следующему аргументу
        pos = close_bracket + 1;
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