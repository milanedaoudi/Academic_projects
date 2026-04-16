#include <iostream>
#include <cassert>
#include "Bus.hpp"  
#include "DataValue.hpp"

using namespace std;


// -----------------------------------------------------
// A simple fake source that produces a known sequence of DataValues
// -----------------------------------------------------
class FakeSource : public IReadable {
private:
    int counter = 0;
    int maxCount;
public:
    FakeSource(int max) : maxCount(max) {}
    
    DataValue read() override {
        if (counter >= maxCount)
            return DataValue(0.0, false); // invalid (no more data)
        return DataValue(static_cast<double>(++counter), true);
    }
};

//  --------------Main test--------------

void test_bus() {
    cout << "\n\033[1;35m═══════════════════════════════════════════════════\033[0m\n";
    cout << "\033[1;35m║ TEST SUITE: Bus Simulation                      ║\033[0m\n";
    cout << "\033[1;35m═══════════════════════════════════════════════════\033[0m\n";

    int total = 0, passed = 0;
    auto printAction  = [](const string& msg ){ cout << "\033[1;34m[Action]\033[0m " << msg << endl; };
    auto printInfo    = [](const string& msg ){ cout << "\033[1;36m[Info]\033[0m " << msg << endl; };
    auto printSuccess = [&](const string& msg ){ cout << "\033[1;32m✅ " << msg << "\033[0m\n"; passed++; total++; };
    auto printError   = [&](const string& msg ){ cout << "\033[1;31m❌ " << msg << "\033[0m\n"; total++; };

    try {
        total++;
        printAction("Creating FakeSource(max=10)");
        FakeSource src(10);

        printAction("Creating Bus(label='B1', width=3)");
        Bus bus("BUS", "B1", 3);
        bus.bindSource(&src);
        printInfo("Initial simulate()");
        bus.simulate();

        // Test: first read should be invalid
        DataValue val = bus.read();
        if(!val.isValid()) {
            printSuccess("Step 1: first read is invalid as expected");
            passed++;
        } else {
            printError("Step 1: first read should be invalid");
        }

        // Step 2: simulate again, read ready values
        total++;
        printInfo("Step 2: simulate() and read ready values");
        bus.simulate();
        int expected[] = {1, 2, 3};
        bool step2_ok = true;
        for(int i=0;i<3;i++){
            val = bus.read();
            if(val.isValid() && val.getValue() == expected[i]){
                printSuccess("Read value: " + to_string(val.getValue()));
            } else {
                printError("Expected " + to_string(expected[i]) + ", got " + (val.isValid() ? to_string(val.getValue()) : "invalid"));
                step2_ok = false;
            }
        }
        // next read should be invalid
        val = bus.read();
        if(!val.isValid()) printSuccess("No more ready values as expected");
        else { printError("Expected no more ready values"); step2_ok=false; }
        if(step2_ok) passed++;

        // Step 3: simulate() → next batch
        total++;
        printInfo("Step 3: simulate() next batch");
        bus.simulate();
        val = bus.read();
        if(val.isValid() && val.getValue() == 4) { printSuccess("Read value: 4"); passed++; }
        else printError("Expected 4");

        // Step 4: simulate() → values 5..9
        total++;
        printInfo("Step 4: simulate() next batch of ready values");
        bus.simulate();
        int expected4[] = {5,6,7,8,9};
        bool step4_ok = true;
        for(int i=0;i<5;i++){
            val = bus.read();
            if(val.isValid() && val.getValue() == expected4[i]){
                printSuccess("Read value: " + to_string(val.getValue()));
            } else {
                printError("Expected " + to_string(expected4[i]) + ", got " + (val.isValid() ? to_string(val.getValue()) : "invalid"));
                step4_ok = false;
            }
        }
        if(step4_ok) passed++;

        // Step 5: simulate() → last value 10
        total++;
        printInfo("Step 5: simulate() final value");
        bus.simulate();
        val = bus.read();
        if(val.isValid() && val.getValue() == 10) printSuccess("Read value: 10"); else printError("Expected 10");
        passed++;

        // After all, next reads invalid
        val = bus.read();
        if(!val.isValid()) printSuccess("No more ready values: end of Bus test");
        else printError("Expected no more ready values at end");

    } catch(...) {
        printError("Exception occurred during Bus test");
    }

    cout << "\033[1;36m║ TEST: Bus depuis fichier ║\033[0m\n";
    Bus bus;
    try {
        bus.LoadFileBus("../data/bus1.txt");  
        cout << "\033[1;32m✅Type : \033[0m" << bus.getType() << endl;
        cout << "\033[1;32m✅Label : \033[0m" << bus.getLabel() << endl;
        cout << "\033[1;32m✅Source : \033[0m" << bus.getSourceLabel() << endl;
    }
    catch (const exception& e) {
        cerr << "Erreur pendant le chargement du Display : " << e.what() << endl;
    }

    cout << "\n=== Test 2 terminé ✅ ===\n";



    // Résumé
    cout << "\n\033[1;35m────────────────────────────────────────────\033[0m\n";
    if(passed == total)
        printSuccess("ALL BUS TESTS PASSED (" + to_string(passed) + "/" + to_string(total) + ")");
    else
        printError("SOME BUS TESTS FAILED (" + to_string(passed) + "/" + to_string(total) + ")");
    cout << "\033[1;35m────────────────────────────────────────────\033[0m\n";
}

int main() {
    test_bus();
    return 0;
}
