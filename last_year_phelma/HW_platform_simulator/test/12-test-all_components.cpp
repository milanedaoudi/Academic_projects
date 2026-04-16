#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Cpu.hpp"
#include "Program.hpp"
#include "Memory.hpp"
#include "Bus.hpp"
#include "Display.hpp"

using namespace std;

// --------------------------------------------------
// Helper to print Memory buffer content
// --------------------------------------------------
void printBuffer(const vector<double>& buf) {
    cout << "[ ";
    for (double v : buf) cout << fixed << setprecision(2) << v << " ";
    cout << "]";
}

// --------------------------------------------------
// TEST CHAIN: CPU → Memory → Bus → Display
// --------------------------------------------------
void test_cpu_memory_bus_display_chain() {
    cout << "\n\033[1;36m══════════ TEST DISPLAY ← BUS ← MEMORY ← CPU ══════════\033[0m\n";

    // 1. Load CPU program
    Program prog;
    prog.loadFromFile("../data/test_all_components.txt");
    prog.InitPointer();

    // 2. Instantiate components
    CPU cpu( "CPU" , "Main CPU", 3, 1, prog);
    Memory mem("MEMORY" , "RAM1", 5, 1);     // buffer size 5, access time 1
    Bus bus( "BUS" , "BUS1", 3);           // width 3
    Display disp( "DISPLAY", "SCREEN1", 2);   // refresh every 2 cycles

    // 3. Bind components (CPU → Memory → Bus → Display)
    mem.bindSource(&cpu);
    bus.bindSource(&mem);
    disp.bindSource(&bus);

    cout << "\n\033[1;33m--- Simulation chain ready ---\033[0m\n";

    // 4. Run simulation
    int totalCycles = 12;
    for (int step = 1; step <= totalCycles; ++step) {
        cout << "\n\033[1;33m=== CYCLE " << step << " ===\033[0m\n";

        // --- CPU ---
        cout << "\033[1;36m[CPU]\033[0m Simulating...\n";
        cpu.simulate();

        // Try reading values produced by CPU
        // DataValue val;
        // while ((val = cpu.read()).isValid()) {
        //     cout << "  → CPU produced value: " << val.getValue() << endl;
        // }

        // --- Memory ---
        cout << "\033[0;36m[Memory]\033[0m Simulating...\n";
        mem.simulate();
        cout << "  Buffer content: ";
        printBuffer(mem.getBuffer());
        cout << "\n";

        // --- Bus ---
        cout << "\033[0;35m[Bus]\033[0m Simulating...\n";
        bus.simulate();
        bus.printPending();
        bus.printReady();

        // --- Display ---
        cout << "\033[0;32m[Display]\033[0m Simulating (refresh every "
             << 2 << " cycles)...\n";
        disp.simulate();
    }

    cout << "\n\033[1;32m✅ Full chain simulation complete.\033[0m\n";
    cout << "Final memory buffer: ";
    printBuffer(mem.getBuffer());
    cout << "\n";
}

// --------------------------------------------------
// MAIN
// --------------------------------------------------
int main() {
    cout << "\033[1;35m║ TEST SUITE: CPU + Memory + Bus + Display ║\033[0m\n";
    test_cpu_memory_bus_display_chain();
    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";
    cout << "Expected:\n";
    cout << " • CPU executes program, produces DataValues\n";
    cout << " • Memory stores and outputs to Bus\n";
    cout << " • Bus transmits values gradually to Display\n";
    cout << " • Display shows updates every 2 cycles\n";
    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";
    return 0;
}
