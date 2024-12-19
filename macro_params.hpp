#include "emulator/fluid_emulator.hpp"
#include "params/compile_params.hpp"
#include "params/runtime_params.hpp"
#include <utility>

#pragma once

// макрос для создания if блоков, определяющих шаблонные параметры для FluidEmulator
// перебор 27 вариантов расположение 3-ёх элементов из набор {float, fixed, fast_fixed} для 3-ёх перменных p,v,vf

// кол-во блоков (вариантов возможного порядка определения типов для p, v, vf) было ограничено с 27 до 3
// добавление каждого нового блока сильно увеличивает время компиляции
#define IF_CREATE_EMULATOR(p, v, vf, i, j, k, allowed_types, allowed_N, allowed_K, p_NK, v_NK, vf_NK, save_ticks, load_tick, filename1, filename2) \
if ((p == allowed_types[i] && p_NK.first == allowed_N[i] && p_NK.second == allowed_K[i]) && \
    (v == allowed_types[j] && v_NK.first == allowed_N[j] && v_NK.second == allowed_K[j]) && \
    (vf == allowed_types[k] && vf_NK.first == allowed_N[k] && vf_NK.second == allowed_K[k])) { \
    if (p == "fixed" && v == "fixed" && vf == "fixed") { \
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; \
        emulator.emulate(save_ticks, load_tick, filename1, filename2); \
        return; \
    } else if (p == "float" && v == "float" && vf == "float") { \
        FluidEmulator<Float, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; \
        emulator.emulate(save_ticks, load_tick, filename1, filename2); \
        return; \
    } \
}


// Все блоки:
/*
    if (p == "float" && v == "float" && vf == "float") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "float" && vf == "fixed") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "float" && vf == "fast_fixed") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "fixed" && vf == "float") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "fixed" && vf == "fixed") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "fixed" && vf == "fast_fixed") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "fast_fixed" && vf == "float") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "fast_fixed" && vf == "fixed") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "float" && v == "fast_fixed" && vf == "fast_fixed") { 
        FluidEmulator<Float, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "float" && vf == "float") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "float" && vf == "fixed") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "float" && vf == "fast_fixed") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "fixed" && vf == "float") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "fixed" && vf == "fixed") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "fixed" && vf == "fast_fixed") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "fast_fixed" && vf == "float") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "fast_fixed" && vf == "fixed") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fixed" && v == "fast_fixed" && vf == "fast_fixed") { 
        FluidEmulator<Fixed, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "float" && vf == "float") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "float" && vf == "fixed") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "float" && vf == "fast_fixed") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], Float, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "fixed" && vf == "float") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "fixed" && vf == "fixed") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "fixed" && vf == "fast_fixed") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], Fixed, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "fast_fixed" && vf == "float") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], Float, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "fast_fixed" && vf == "fixed") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], Fixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } else if (p == "fast_fixed" && v == "fast_fixed" && vf == "fast_fixed") { 
        FluidEmulator<FastFixed, allowed_N[i], allowed_K[i], FastFixed, allowed_N[j], allowed_K[j], FastFixed, allowed_N[k], allowed_K[k]> emulator; 
        emulator.emulate(save_ticks, load_tick, filename1, filename2);
        return; 
    } 
*/