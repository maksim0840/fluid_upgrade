#include "emulator/fluid_emulator.cpp"


int main() {
    FluidEmulator<Fixed, 32, 0, Fixed, 32, 0, Fixed, 32, 0> emulator;
    emulator.emulate();

    return 0;
}