#include "emulator/fluid_emulator.cpp"


int main() {
    FluidEmulator<Fixed, 32, 16, Fixed, 32, 16, Fixed, 32, 16> emulator;
    emulator.emulate();

    return 0;
}