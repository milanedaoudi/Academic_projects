#include <iostream>
#include <cassert>
#include "Register.hpp"
#include "LoadUtils.hpp"
using namespace std;

void test_register() {
    cout << "\033[1;36m║ TEST SUITE: Register (FIFO) ║\033[0m\n";
    Register reg;
    int total = 0, passed = 0;
    auto printAction  = [](const string& msg ){ cout << "\033[1;34m[Action]\033[0m " << msg << endl; };
   

    // ---------------- Test 1 ----------------
    total++;
    try {
        assert(reg.isEmpty());
        cout << " \033[1;32m✅ Test 1 passed:\033[0m Register starts empty\n";
        passed++;
    } catch (...) {
        cout << " \033[1;31m❌ Test 1 failed:\033[0m Register not empty\n";
    }

    // ---------------- Test 2 ----------------
    total++;
    try {
        printAction("Writing 42.0");
        reg.write(42.0);

        DataValue d1 = reg.read();
        printAction("Read value: " + to_string(d1.getValue()) + " (valid=" + (d1.isValid() ? "true" : "false") + ")" );
        
        assert(d1.isValid() && d1.getValue() == 42.0);
        assert(reg.isEmpty());
        cout << " \033[1;32m✅ Test 2 passed:\033[0m Write/read single value\n";
        passed++;
    } catch (...) {
        cout << " \033[1;31m❌ Test 2 failed:\033[0m Write/read single value\n";
    }

    // ---------------- Test 3 ----------------
    total++;
    try {
        printAction("Writing 10.0, 20.0, 30.0" );
        reg.write(10.0);
        reg.write(20.0);
        reg.write(30.0);

        vector<double> expected = {10.0, 20.0, 30.0};
        for (double val : expected) {
            val=val;
            DataValue dv = reg.read();
            printAction("Read value: " + to_string(dv.getValue()) + " (valid=" + (dv.isValid() ? "true" : "false") + ")");
        }

        assert(reg.isEmpty());
        cout << " \033[1;32m✅ Test 3 passed:\033[0m FIFO order preserved\n";
        passed++;
    } catch (...) {
        cout << " \033[1;31m❌ Test 3 failed:\033[0m FIFO order\n";
    }

    // ---------------- Test 4 ----------------
    total++;
    try {
        printAction("Reading from empty register" );
        DataValue dvEmpty = reg.read();
        printAction("Read value: " + to_string(dvEmpty.getValue()) + " (valid=" + (dvEmpty.isValid() ? "true" : "false") + ")" );
        assert(!dvEmpty.isValid());
        cout << " \033[1;32m✅ Test 4 passed:\033[0m Invalid DataValue when empty\n";
        passed++;
    } catch (...) {
        cout << " \033[1;31m❌ Test 4 failed:\033[0m Reading empty register\n";
    }

    // ---------------- Résumé ----------------
    cout << "\n\033[1;33m─────────────────────────────────────────────\033[0m\n";
    if (passed == total)
        cout << "\033[1;32m✅ ALL REGISTER TESTS PASSED (" << passed << "/" << total << ")\033[0m\n";
    else
        cout << "\033[1;31m⚠️  SOME REGISTER TESTS FAILED (" << passed << "/" << total << ")\033[0m\n";
    cout << "\033[1;33m─────────────────────────────────────────────\033[0m\n";
}

int main() {
    test_register();
    return 0;
}
