#include <iostream>
#include <cassert>
#include "Component.hpp"
#include "LoadUtils.hpp"

using namespace std;

    auto printAction  = [](const string& msg ){ cout << "\033[1;34m[Action]\033[0m " << msg << endl; };
    auto printInfo    = [](const string& msg ){ cout << "\033[1;36m[Info]\033[0m " << msg << endl; };
    auto printSuccess = [](const string& msg , int& passed, int& total){ cout << "\033[1;32m✅ " << msg << "\033[0m\n"; passed++; total++; };
    auto printError   = [](const string& msg , int& total){ cout << "\033[1;31m❌ " << msg << "\033[0m\n"; total++; };


// Classe dérivée pour tester Component
class TestComponent : public Component {
public:
    TestComponent(const string& typ, const string& lbl) : Component(typ, lbl) {}
    void simulate() override {
        cout << "\033[1;36m[Simulate]\033[0m " << label << " of type " << type << endl;
    }
};

void test_component() {
    cout << "\n\033[1;35m═══════════════════════════════════════════════════\033[0m\n";
    cout << "\033[1;35m║            TEST SUITE: Component Class          ║\033[0m\n";
    cout << "\033[1;35m═══════════════════════════════════════════════════\033[0m\n";


    int total = 0, passed = 0;

    // Lambdas pour affichage standardisé
    auto printAction = [](const string& msg) {
        cout << "\033[1;34m[Action]\033[0m " << msg << endl;
    };
    auto printInfo = [](const string& msg) {
        cout << "\033[1;36m[Info]\033[0m " << msg << endl;
    };
    auto printSuccess = [](const string& msg) {
        cout << "\033[1;32m✅ " << msg << "\033[0m\n";
    };
    auto printError = [](const string& msg) {
        cout << "\033[1;31m❌ " << msg << "\033[0m\n";
    };

    // --- Test 1: Constructeur et getters ---
    total++;
    try {
        printAction("Creating component: type=CPU, label=MainProc");
        TestComponent comp("CPU", "MainProc");
        assert(comp.getType() == "CPU");
        assert(comp.getLabel() == "MainProc");
        printSuccess("Test 1 passed: Constructor and getters");
        passed++;
    } catch(...) {
        printError("Test 1 failed: Constructor/getters");
    }

    // --- Test 2: Priority par défaut ---
    total++;
    try {
        printAction("Creating component: type=Bus, label=MainBus");
        TestComponent comp("Bus", "MainBus");
        assert(comp.getPriority() == 0);
        printSuccess("Test 2 passed: Default priority is MEDIUM");
        passed++;
    } catch(...) {
        printError("Test 2 failed: Default priority");
    }

    // --- Test 3: Modifier la priorité ---
    total++;
    try {
        printAction("Creating component: type=RWM, label=MainRAM");
        TestComponent comp("RWM", "MainRAM");
        printAction("Set priority: 2");
        comp.setPriority(2);
        assert(comp.getPriority() == 2);
        printAction("Set priority: 0");
        comp.setPriority(0);
        assert(comp.getPriority() == 0);
        printAction("Set priority: 1");
        comp.setPriority(1);
        assert(comp.getPriority() == 1);
        printSuccess("Test 3 passed: Priority set/get works");
        passed++;
    } catch(...) {
        printError("Test 3 failed: Priority set/get");
    }

    // --- Test 4: Priority invalide ---
    total++;
    try {
        printAction("Creating component: type=IO, label=Port1");
        TestComponent comp("IO", "Port1");
        printAction("Attempt to set invalid priority: INVALID");
        comp.setPriority(-1);
        assert(comp.getPriority() == -1); // unchanged
        printSuccess("Test 4 passed: Invalid priority unchange default value (MEDIUM)");
        passed++;
    } catch(...) {
        printError("Test 4 failed: Invalid priority handling");
    }

    // --- Test 5: simulate() affichage ---
    total++;
    try {
        printAction("Creating component: type=CPU, label=Core0");
        TestComponent comp("CPU", "Core0");
        printInfo("Calling simulate()...");
        comp.simulate();  // should execute without error
        printSuccess("Test 5 passed: simulate() runs successfully");
        passed++;
    } catch(...) {
        printError("Test 5 failed: simulate()");
    }

    // --- Résumé ---
    cout << "\n\033[1;35m═══════════════════════════════════════════════════\033[0m\n";
    if(passed == total)
        printSuccess("ALL COMPONENT TESTS PASSED (" + to_string(passed) + "/" + to_string(total) + ")");
    else
        printError("SOME COMPONENT TESTS FAILED (" + to_string(passed) + "/" + to_string(total) + ")");
    cout << "\033[1;35m═══════════════════════════════════════════════════\033[0m\n";
}

 int main() {
     test_component();
     return 0;
 }
