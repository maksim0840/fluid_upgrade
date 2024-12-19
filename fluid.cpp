
#include "macro_params.hpp"
#define START_VALUES_FILE "../start_values/start_file"
#define SAVE_TICKS_FILE "../emulator/saved_ticks"

// Рекурсивный constexpr цикл для перебора i, j, k
template <typename Func, int I = 0, int J = 0, int K = 0>
constexpr void constexpr_triple_loop(Func&& func) {
    if constexpr (I < DTYPES_SIZE) {
        if constexpr (J < DTYPES_SIZE) {
            if constexpr (K < DTYPES_SIZE) {
                func(std::integral_constant<int, I>{},
                     std::integral_constant<int, J>{},
                     std::integral_constant<int, K>{});
                constexpr_triple_loop<Func, I, J, K + 1>(std::forward<Func>(func));
            } else {
                constexpr_triple_loop<Func, I, J + 1, 0>(std::forward<Func>(func));
            }
        } else {
            constexpr_triple_loop<Func, I + 1, 0, 0>(std::forward<Func>(func));
        }
    }
}

// Запустить эмулятор
void start_fluid(const Params& params) {

    std::string p, v, vf;
    std::pair<int, int> p_NK = extract_params(params.p_type, p);
    std::pair<int, int> v_NK = extract_params(params.v_type, v);
    std::pair<int, int> vf_NK = extract_params(params.v_flow_type, vf);

    constexpr auto allowed_types = get_allowed_types();
    constexpr auto allowed_N = get_allowed_N();
    constexpr auto allowed_K = get_allowed_K();

    constexpr_triple_loop([&](auto i, auto j, auto k) {
        constexpr int i_val = decltype(i)::value;
        constexpr int j_val = decltype(j)::value;
        constexpr int k_val = decltype(k)::value;
        IF_CREATE_EMULATOR(p, v, vf, i_val, j_val, k_val, allowed_types, allowed_N, allowed_K, p_NK, v_NK, vf_NK, params.save_tick, params.load_tick, START_VALUES_FILE, SAVE_TICKS_FILE);
    });

    throw std::runtime_error("Incorrect type for emulator");
}


int main(int argc, char* argv[]) {
    Params params = get_runtime_params(argc, argv);
    start_fluid(params);
    return 0;
}
