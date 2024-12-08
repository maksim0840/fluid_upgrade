#include "../types/fixed_types.cpp"
#pragma once

class FluidEmulator {
private:
    static constexpr size_t T = 1'000'000; // количество тиков работы программы
    static constexpr std::array<std::pair<int, int>, 4> deltas{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}}; // направления жидкости (влево, впрво, вниз, вверх)
    static constexpr size_t N = 36; // ширина поля
    static constexpr size_t M = 84; // высота поля

    // Доступные поля:
    static inline char field[36][84 + 1] = {
        "####################################################################################",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                       .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............################                     #                 #",
        "#...........................#....................................#                 #",
        "#...........................#....................................#                 #",
        "#...........................#....................................#                 #",
        "##################################################################                 #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "####################################################################################",
    };
    static inline char field2[14][5 + 1] = {
        "#####",
        "#.  #",
        "#.# #",
        "#.# #",
        "#.# #",
        "#.# #",
        "#.# #",
        "#.# #",
        "#...#",
        "#####",
        "#   #",
        "#   #",
        "#   #",
        "#####",
    };

    // Структура для свапа параметров
    struct ParticleParams {
        char type;
        Fixed cur_p;
        std::array<Fixed, deltas.size()> v;

        void swap_with(int x, int y) {
            std::swap(field[x][y], type);
            std::swap(p[x][y], cur_p);
            std::swap(velocity.v[x][y], v);
        }
    };

    struct VectorField {
        std::array<Fixed, deltas.size()> v[N][M];
        Fixed &add(int x, int y, int dx, int dy, Fixed dv) {
            return get(x, y, dx, dy) += dv;
        }

        Fixed &get(int x, int y, int dx, int dy) {
            size_t i = std::ranges::find(deltas, std::pair(dx, dy)) - deltas.begin();
            assert(i < deltas.size());
            return v[x][y][i];
        }
    };
    
    // генератор случайных чисел типа FIXED
    std::mt19937 rnd{1337};
    Fixed random01() {
        return Fixed::from_raw((rnd() & ((1 << 16) - 1)));
    }

    static inline VectorField velocity{}, velocity_flow{};
    int last_use[N][M]{};
    int UT = 0;
    Fixed rho[256]; // плотность веществ (сиволов) на поле field
    static inline Fixed p[N][M]{};
    Fixed old_p[N][M];
    int dirs[N][M]{};

    // Функции для работы эмулятора
    std::tuple<Fixed, bool, std::pair<int, int>> propagate_flow(int x, int y, Fixed lim);
    void propagate_stop(int x, int y, bool force = false);
    Fixed move_prob(int x, int y);
    bool propagate_move(int x, int y, bool is_first);


public:
    void emulate();

};