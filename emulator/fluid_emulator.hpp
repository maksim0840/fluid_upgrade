#include "../types/fixed_operations.hpp"
#include "../start_values/start_values.hpp"
#include <fstream>

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






    /* Функции для работы эмулятора */





    std::tuple<vfFixType<vfN, vfK>, bool, std::pair<int, int>> propagate_flow(int x, int y, vfFixType<vfN, vfK> lim) {
        last_use[x][y] = UT - 1;
        pFixType<pN, pK> ret = 0;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT) {
                auto cap = velocity.get(x, y, dx, dy);
                auto flow = velocity_flow.get(x, y, dx, dy);
                if (flow == cap) {
                    continue;
                }
                // assert(v >= velocity_flow.get(x, y, dx, dy));
                auto vp = std::min(lim, vfFixType<vfN, vfK>(cap - flow));
                if (last_use[nx][ny] == UT - 1) {
                    velocity_flow.add(x, y, dx, dy, vp);
                    last_use[x][y] = UT;
                    // cerr << x << " " << y << " -> " << nx << " " << ny << " " << vp << " / " << lim << "\n";
                    return {vp, 1, {nx, ny}};
                }
                auto [t, prop, end] = propagate_flow(nx, ny, vp);
                ret += t;
                if (prop) {
                    velocity_flow.add(x, y, dx, dy, t);
                    last_use[x][y] = UT;
                    // cerr << x << " " << y << " -> " << nx << " " << ny << " " << t << " / " << lim << "\n";
                    return {t, prop && end != std::pair(x, y), end};
                }
            }
        }
        last_use[x][y] = UT;
        return {ret, 0, {0, 0}};
    }
    
    
    void propagate_stop(int x, int y, bool force = false) {
        if (!force) {
            bool stop = true;
            for (auto [dx, dy] : deltas) {
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > zero) {
                    stop = false;
                    break;
                }
            }
            if (!stop) {
                return;
            }
        }
        last_use[x][y] = UT;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > zero) {
                continue;
            }
            propagate_stop(nx, ny);
        }
    }


    pFixType<pN, pK> move_prob(int x, int y) {
        pFixType<pN, pK> sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            if (v < zero) {
                continue;
            }
            sum += v;
        }
        return sum;
    }


    bool propagate_move(int x, int y, bool is_first) {
        last_use[x][y] = UT - is_first;
        bool ret = false;
        int nx = -1, ny = -1;
        do {
            std::array<pFixType<pN, pK>, deltas.size()> tres;
            pFixType<pN, pK> sum = 0;
            for (size_t i = 0; i < deltas.size(); ++i) {
                auto [dx, dy] = deltas[i];
                int nx = x + dx, ny = y + dy;
                if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                    tres[i] = sum;
                    continue;
                }
                auto v = velocity.get(x, y, dx, dy);
                if (v < zero) {
                    tres[i] = sum;
                    continue;
                }
                sum += v;
                tres[i] = sum;
            }

            if (sum == zero) {
                break;
            }

            pFixType<pN, pK> p = sum * random01();
            size_t d = std::ranges::upper_bound(tres, p) - tres.begin();

            auto [dx, dy] = deltas[d];
            nx = x + dx;
            ny = y + dy;
            assert(velocity.get(x, y, dx, dy) > zero && field[nx][ny] != '#' && last_use[nx][ny] < UT);

            ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
        } while (!ret);
        last_use[x][y] = UT;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < zero) {
                propagate_stop(nx, ny);
            }
        }
        if (ret) {
            if (!is_first) {
                ParticleParams pp{};
                pp.swap_with(x, y);
                pp.swap_with(nx, ny);
                pp.swap_with(x, y);
            }
        }
        return ret;
    }


    void save_tick_params(int tick, const std::string& filename) {
        // Открываем файл в режиме добавления в конец
        std::ofstream out(filename, std::ios::binary | std::ios::app);
        if (!out.is_open()) {
            throw std::ios_base::failure("Cant open binary table file\n");
        }

        size_t param_size;

        // Сохраняем tick
        param_size = sizeof(tick);
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        out.write(reinterpret_cast<char*>(&tick), param_size);
        
        // Сохраняем field
        size_t field_size = field.size(); // кол-во строк в векторе
        size_t row_size = (field[0]).size();  // размер строки
        param_size = field_size * (row_size + sizeof(row_size));  // размер всех строк + размер указанныех размеров строк
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (const auto& row : field) {
            out.write(reinterpret_cast<char*>(&row_size), sizeof(row_size));  // записываем длину строки
            out.write(row.c_str(), row_size);  // записываем содержимое строки
        }
        // Сохраняем p
        param_size = sizeof(p);
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                out.write(reinterpret_cast<char*>(&p[x][y]), sizeof(pFixType<pN, pK>));
            }
        }
        // Сохраняем old_p
        param_size = sizeof(old_p);
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                out.write(reinterpret_cast<char*>(&old_p[x][y]), sizeof(pFixType<pN, pK>));
            }
        }
        // Сохраняем velocity
        param_size = sizeof(velocity);
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                for (size_t i = 0; i < deltas.size(); ++i) {
                    out.write(reinterpret_cast<char*>(&velocity.v[x][y][i]), sizeof(vFixType<vN, vK>));
                }
            }
        }
        // Сохраняем velocity_flow
        param_size = sizeof(velocity_flow);
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                for (size_t i = 0; i < deltas.size(); ++i) {
                    out.write(reinterpret_cast<char*>(&velocity_flow.v[x][y][i]), sizeof(vfFixType<vfN, vfK>));
                }
            }
        }
        // Сохраняем last_use
        param_size = sizeof(last_use);
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                out.write(reinterpret_cast<char*>(&last_use[x][y]), sizeof(int));
            }
        }
        // Сохраняем UT
        param_size = sizeof(UT);
        out.write(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        out.write(reinterpret_cast<char*>(&UT), param_size);
        
        // Закрываем файл
        out.close();
    }


    void load_tick_params(int& tick, const std::string& filename) {
        // Открываем файл в бинарном режиме для чтения
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) {
            throw std::ios_base::failure("Can't open saved_ticks file for reading.");
        }

        size_t param_size;
        int current_tick = 0;

        // Ищем первый тик, который >= заданного tick
        while (1) {

            // Завершаем чтение если достигли конца
            if(!in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size))) {
                tick = 0;
                in.close();
                std::cout << "___CANNOT FIND TICK, START WITH 0___\n";
                return;
            }

            // Читаем номер текущего тика
            in.read(reinterpret_cast<char*>(&current_tick), param_size);

            // Переходим к сохранению параметров
            if (current_tick >= tick) {
                tick = current_tick;
                break;
            }

            // Скипаем 7 других параметрво помимо tick
            for (int i = 0; i < 7; ++i) {
                in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
                in.ignore(param_size);
            }
        }

        // Загружаем field
        in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t i = 0; i < N; ++i) {
            in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
            in.read(&field[i][0], param_size);
        }
        // Загружаем p
        in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                in.read(reinterpret_cast<char*>(&p[x][y]), sizeof(pFixType<pN, pK>));
            }
        }
        // Загружаем old_p
        in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                in.read(reinterpret_cast<char*>(&old_p[x][y]), sizeof(pFixType<pN, pK>));
            }
        }
        // Загружаем velocity
        in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                for (size_t i = 0; i < deltas.size(); ++i) {
                    in.read(reinterpret_cast<char*>(&velocity.v[x][y][i]), sizeof(vFixType<vN, vK>));
                }
            }
        }
        // Загружаем velocity_flow
        in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                for (size_t i = 0; i < deltas.size(); ++i) {
                    in.read(reinterpret_cast<char*>(&velocity_flow.v[x][y][i]), sizeof(vfFixType<vfN, vfK>));
                }
            }
        }
        // Загружаем last_use
        in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                in.read(reinterpret_cast<char*>(&last_use[x][y]), sizeof(int));
            }
        }
        // Загружаем UT
        in.read(reinterpret_cast<char*>(&param_size), sizeof(param_size));
        in.read(reinterpret_cast<char*>(&UT), param_size);

        // Закрываем файл
        in.close();
    }
    

public:
    void emulate(int save_tick, int load_tick, const std::string& start_file, const std::string& ticks_file) {

        // Получаем значения из файла
        StartValues start_values = get_start_values(start_file);

        rho[' '] = Fixed<32, 0>(start_values.rho_space);
        rho['.'] = Fixed<32, 0>(start_values.rho_dot);
        vFixType<vN, vK> g(start_values.g);
        if (!(start_values.N == N && start_values.M == M)) {
            throw std::runtime_error("unequal emulator and input field sizes");
        }
        field = start_values.field;

        // Загружаемся с нужного тика
        if (load_tick != 0) {
            load_tick_params(load_tick, ticks_file);
        }

        // Определяем режим сохранения значений
        if (save_tick != 0) {
            // перезапись файла
            std::ofstream out(ticks_file, std::ios::binary | std::ios::trunc);
            if (!out.is_open()) {
                throw std::ios_base::failure("сannot open saved_ticks file for clearing.");
            }
            out.close();
        }

        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas) {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }
        for (size_t i = 0; i < T; ++i) {  
            pFixType<pN, pK> total_delta_p = 0;
            // Apply external forces
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    if (field[x + 1][y] != '#')
                        velocity.add(x, y, 1, 0, g);
                }
            }
            // Apply forces from p
            memcpy(old_p, p, sizeof(p));
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas) {
                        int nx = x + dx, ny = y + dy;
                        if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y]) {
                            auto delta_p = old_p[x][y] - old_p[nx][ny];
                            auto force = delta_p;
                            auto &contr = velocity.get(nx, ny, -dx, -dy);
                            if (contr * rho[(int) field[nx][ny]] >= force) {
                                contr -= force / rho[(int) field[nx][ny]];
                                continue;
                            }
                            force -= contr * rho[(int) field[nx][ny]];
                            contr = 0;
                            velocity.add(x, y, dx, dy, force / rho[(int) field[x][y]]);
                            p[x][y] -= force / pFixType<pN, pK>(dirs[x][y]);
                            total_delta_p -= force / pFixType<pN, pK>(dirs[x][y]);
                        }
                    }
                }
            }
            // Make flow from velocities
            velocity_flow = {};
            bool prop = false;
            do {
                UT += 2;
                prop = 0;
                for (size_t x = 0; x < N; ++x) {
                    for (size_t y = 0; y < M; ++y) {
                        if (field[x][y] != '#' && last_use[x][y] != UT) {
                            auto [t, local_prop, _] = propagate_flow(x, y, 1);
                            if (t > zero) {
                                prop = 1;
                            }
                        }
                    }
                }
            } while (prop);

            // Recalculate p with kinetic energy
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] == '#')
                        continue;
                    for (auto [dx, dy] : deltas) {
                        auto old_v = velocity.get(x, y, dx, dy);
                        auto new_v = velocity_flow.get(x, y, dx, dy);
                        if (old_v > zero) {
                            assert(new_v <= old_v);
                            velocity.get(x, y, dx, dy) = new_v;
                            auto force = (old_v - new_v) * rho[(int) field[x][y]];
                            if (field[x][y] == '.')
                                force *= vFixType<vN, vK>(0.8);
                            if (field[x + dx][y + dy] == '#') {
                                p[x][y] += force / pFixType<pN, pK>(dirs[x][y]);
                                total_delta_p += force / pFixType<pN, pK>(dirs[x][y]);
                            } else {
                                p[x + dx][y + dy] += force / pFixType<pN, pK>(dirs[x + dx][y + dy]);
                                total_delta_p += force / pFixType<pN, pK>(dirs[x + dx][y + dy]);
                            }
                        }
                    }
                }
            }

            UT += 2;
            prop = false;
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] != '#' && last_use[x][y] != UT) {
                        if (random01() < move_prob(x, y)) {
                            prop = true;
                            propagate_move(x, y, true);
                        } else {
                            propagate_stop(x, y, true);
                        }
                    }
                }
            }

            if (prop) {
                int cur_tick = (load_tick != 0) ? (load_tick + i) : (i);

                // Сохраняем тик
                if ((save_tick != 0) && (cur_tick % save_tick == 0)) {
                    save_tick_params(cur_tick, ticks_file);
                    std::cout << "\n\n TICK WAS SAVED \n\n";
                }
        
                std::cout << "Tick " << cur_tick << ":\n";
                for (size_t x = 0; x < N; ++x) {
                    std::cout << field[x] << "\n";
                }
            }
        }
    }
};