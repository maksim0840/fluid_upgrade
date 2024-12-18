#include "fluid_emulator.hpp"


template<template<int, int> class pFixType, int pN, int pK, 
    template<int, int> class vFixType, int vN, int vK,
    template<int, int> class vfFixType, int vfN, int vfK>
std::tuple<vfFixType<vfN, vfK>, bool, std::pair<int, int>> FluidEmulator<pFixType, pN, pK, vFixType, vN, vK, vfFixType, vfN, vfK>::propagate_flow(int x, int y, vfFixType<vfN, vfK> lim) {
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

template<template<int, int> class pFixType, int pN, int pK, 
    template<int, int> class vFixType, int vN, int vK,
    template<int, int> class vfFixType, int vfN, int vfK>
void FluidEmulator<pFixType, pN, pK, vFixType, vN, vK, vfFixType, vfN, vfK>::propagate_stop(int x, int y, bool force) {
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

template<template<int, int> class pFixType, int pN, int pK, 
    template<int, int> class vFixType, int vN, int vK,
    template<int, int> class vfFixType, int vfN, int vfK>
pFixType<pN, pK> FluidEmulator<pFixType, pN, pK, vFixType, vN, vK, vfFixType, vfN, vfK>::move_prob(int x, int y) {
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

template<template<int, int> class pFixType, int pN, int pK, 
    template<int, int> class vFixType, int vN, int vK,
    template<int, int> class vfFixType, int vfN, int vfK>
bool FluidEmulator<pFixType, pN, pK, vFixType, vN, vK, vfFixType, vfN, vfK>::propagate_move(int x, int y, bool is_first) {
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

template<template<int, int> class pFixType, int pN, int pK, 
    template<int, int> class vFixType, int vN, int vK,
    template<int, int> class vfFixType, int vfN, int vfK>
void FluidEmulator<pFixType, pN, pK, vFixType, vN, vK, vfFixType, vfN, vfK>::emulate(const std::string& save_ticks, const int load_tick) {
    StartValues start_values = get_start_values();

    rho[' '] = Fixed<32, 0>(start_values.rho_space);
    rho['.'] = Fixed<32, 0>(start_values.rho_dot);
    vFixType<vN, vK> g(start_values.g);

    bool save = (save_ticks != "");

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
            std::cout << "Tick " << i << ":\n";
            for (size_t x = 0; x < N; ++x) {
                std::cout << field[x] << "\n";
            }
        }
    }
}
