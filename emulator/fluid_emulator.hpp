#include "../types/fixed_operations.hpp"
#include "../start_values/start_values.cpp"

#pragma once

template<template<int, int> class pFixType, int pN, int pK, 
    template<int, int> class vFixType, int vN, int vK,
    template<int, int> class vfFixType, int vfN, int vfK>
class FluidEmulator {
private:
    static constexpr size_t T = 1'000'000; // количество тиков работы программы
    static constexpr std::array<std::pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}}; // направления жидкости (влево, впрво, вниз, вверх)
    // Размеры поля
    static constexpr size_t N = 36;
    static constexpr size_t M = 84;
    // Поле
    static inline std::vector<std::string> field;

    // Структура для свапа параметров
    struct ParticleParams {
        char type;
        pFixType<pN, pK> cur_p;
        std::array<vFixType<vN, vK>, deltas.size()> v;

        void swap_with(int x, int y) {
            std::swap(field[x][y], type);
            std::swap(p[x][y], cur_p);
            std::swap(velocity.v[x][y], v);
        }
    };

    template<template<int, int> class FixType1, int N1, int K1>
    struct VectorField {
        std::array<FixType1<N1, K1>, deltas.size()> v[N][M];
        FixType1<N1, K1> &add(int x, int y, int dx, int dy, FixType1<N1, K1> dv) {
            FixType1<N1, K1>& ref = get(x, y, dx, dy);
            ref = ref + dv;
            return ref;
        }

        FixType1<N1, K1> &get(int x, int y, int dx, int dy) {
            size_t i = std::ranges::find(deltas, std::pair(dx, dy)) - deltas.begin();
            assert(i < deltas.size());
            return v[x][y][i];
        }
    };
    
    // генератор случайных чисел типа FIXED
    std::mt19937 rnd{1337};
    Fixed<64, 0> random01() {
        return  Fixed<64, 0> (static_cast<double>(rnd() & ((1 << pK) - 1)) / (1 << pK));
    }

    static inline VectorField<vFixType, vN, vK> velocity{};
    static inline VectorField<vfFixType, vfN, vfK> velocity_flow{};
    int last_use[N][M]{};
    int UT = 0;
    Fixed<32, 0> rho[256]; // плотность веществ (сиволов) на поле field
    static inline pFixType<pN, pK> p[N][M]{};
    pFixType<pN, pK> old_p[N][M];
    int dirs[N][M]{};

    Fixed<64, 0> zero = Fixed<64, 0>();

    // Функции для работы эмулятора
    std::tuple<vfFixType<vfN, vfK>, bool, std::pair<int, int>> propagate_flow(int x, int y, vfFixType<vfN, vfK> lim);
    void propagate_stop(int x, int y, bool force = false);
    pFixType<pN, pK> move_prob(int x, int y);
    bool propagate_move(int x, int y, bool is_first);

    void save_tick_params(int tick, const std::string& filename);
    void load_tick_params(int& tick, const std::string& filename);
    
public:
    void emulate(int save_tick, int load_tick);
    
};