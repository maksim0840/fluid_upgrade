# fluid_upgrade

------------------
# Проверенные параметры запуска (точно работают)
------------------

1.
```
g++-11 -std=c++20 -DTYPES="(FIXED(32,16))" fluid.cpp -o fluid
./fluid --p-type="FIXED(32, 16)" --v-type="FIXED(32, 16)" --v-flow-type="FIXED(32, 16)"
```

2.
```
g++-11 -std=c++20 fluid.cpp -o fluid
./fluid
```
------------------
# Компиляция
------------------
Предупреждение: компиляция долгая ~ 2-3 мин

```
g++-11 -std=c++20 -DTYPES="(FAST_FIXED(32,5),FIXED(32,16),DOUBLE)" fluid.cpp -o fluid
```

Аргументы DTYPES И DSIZES передаются, дополнительно оборачиваясь в "(...)"

- -DTYPES - доступные типы для эмулятора (по умолчанию DOUBLE)

------------------
# Запуск
------------------


```
./fluid --p-type="FIXED(32, 16)" --v-type="FIXED(32, 16)" --v-flow-type="FIXED(32, 16)"
```

Аргументы p_type, v_type и v_flow_type передаются в кавычках "..."

- --p-type - тип для p (по умолчанию DOUBLE)
- --v-type - тип для velocity (по умолчанию DOUBLE)
- --v-flow-type - ип для velocity_flow (по умолчанию DOUBLE)
- --save-tick - сохраняет каждый n-ый тик (если текущий номер тика кратен save_tick) (очищает предыдущие сохранения)
- --load-tick - минимальный номер тика, с которого начать (программа начнёт с минимального тика, который >= load_tick) (по умолчанию 0)

------------------
# Структура файлов
------------------
- types - папка с реализацией шаблонных типов
    - base.hpp - файл, содержащий базовые объявления, требуемые во всех реализуемых типах
    - fixed.hpp - реализация типа Fixed(N, K)
    - fast_fixed.hpp - реализация типа FastFixed(N, K)
    - float.hpp - реализация типа Float(N, 0) ~ обёртка double и float для передачи и использования в эмуляторе
    - fixed_operations.hpp - общие операции над типами
- emulator - папка с реализацией эмулятора жидкости
- params - чтение параметров при компиляции и при запуске программы
- start_values - чтение начальных значений для эмулятора из файла

-macro_params.h - макрос, нужный для передачи типов и их размеров в эмулятор
-fluid.h - запускает программу


------------------
# Проверка сохранения тиков
------------------

```
./fluid --p-type="FIXED(32, 16)" --v-type="FIXED(32, 16)" --v-flow-type="FIXED(32, 16)" --save-tick=10
```

```
./fluid --p-type="FIXED(32, 16)" --v-type="FIXED(32, 16)" --v-flow-type="FIXED(32, 16)" --load-tick=50
```