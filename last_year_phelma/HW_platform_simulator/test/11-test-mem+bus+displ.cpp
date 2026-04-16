#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Memory.hpp"
#include "Bus.hpp"
#include "Display.hpp"

using namespace std;

// --------------------------------------------------
// FakeSource: produces incremental values 1..maxVal
// --------------------------------------------------
class FakeSource : public IReadable {
private:
    int current;
    int maxVal;
public:
    FakeSource(int start, int end) : current(start), maxVal(end) {}

    DataValue read() override {
        if (current > maxVal)
            return DataValue(0.0, false);
        return DataValue(current++, true);
    }
};

// --------------------------------------------------
// Helper for formatted debug output
// --------------------------------------------------
void printBuffer(const vector<double>& buf) {
    cout << "[ ";
    for (double v : buf) cout << fixed << setprecision(1) << v << " ";
    cout << "]";
}

// --------------------------------------------------
// TEST CHAIN: FakeSource → Memory → Bus → Display
// --------------------------------------------------
void test_display_bus_memory_chain() {
    cout << "\n\033[1;36m══════════ TEST DISPLAY ← BUS ← MEMORY ← SOURCE ══════════\033[0m\n";

    FakeSource src(1, 10);
    Memory mem( "RAM" ,"RAM1", 10, 1);     // 3 slots, writes every 1 step
    Bus bus("BUS" , "BUS1", 2);           // width 2 = max 2 data in flight
    Display disp( "DISPLAY" ,"SCREEN1", 2);   // refresh every 2 steps

    // Bind components
    mem.bindSource(&src);
    bus.bindSource(&mem);
    disp.bindSource(&bus);

    // 1️. Memory simulate
    cout << "\033[0;36m[Memory]\033[0m Simulating... ";
    mem.simulate();
    cout << "Buffer after simulate: ";
    printBuffer(mem.getBuffer());
    cout << "\n";

    // Simulate 10 cycles
    for (int step = 1; step <= 10; ++step) {
        cout << "\n\033[1;33m--- CYCLE " << step << " ---\033[0m\n";

        // 2️. Bus simulate
        cout << "\033[0;35m[Bus]\033[0m Simulating... ";
        bus.simulate();
        cout << "(check pending/ready)\n";

        // Read current bus state
        bus.printPending();
        bus.printReady();

        // 3️. Display simulate
        cout << "\033[0;32m[Display]\033[0m Simulating...\n ";
        disp.simulate();
        cout << "(refresh every 2 cycles)\n";
        
    }

    cout << "\n\033[1;32m✅ Chain simulation complete.\033[0m\n";
    cout << "Final memory buffer content: ";
    printBuffer(mem.getBuffer());
    cout << "\n";
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main() {
    cout << "\033[1;35m║ TEST SUITE: Display + Bus + Memory ║\033[0m\n";
    test_display_bus_memory_chain();
    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";
    cout << "Expected behavior:\n";
    cout << " - Memory stores new values each cycle (size=3)\n";
    cout << " - Bus moves data from memory to display gradually\n";
    cout << " - Display shows data only every 2 steps (refresh rate)\n";
    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";
    return 0;
}
