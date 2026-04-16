#include <iostream>
#include <cassert>
#include "DataValue.hpp"

using namespace std;

int test_data_value() {
    cout << "\033[1;36m║ TEST SUITE: DataValue Class ║\033[0m\n";
    
    int passed = 0, total = 0;

    // ---------------- Test 1 ----------------
    total++;
    try {
        DataValue d1;
        assert(!d1.isValid());
        assert(d1.getValue() == 0.0);
        cout << " \033[1;32m✅ Test 1 passed:\033[0m Default invalid DataValue\n";
        passed++;
    } catch (const exception& e) {
        cout << " \033[1;31m❌ Test 1 failed:\033[0m Exception: " << e.what() << "\n";
    } catch (...) {
        cout << " \033[1;31m❌ Test 1 failed:\033[0m Unknown error\n";
    }

    // ---------------- Test 2 ----------------
    total++;
    try {
        DataValue d2(42.5, true);
        assert(d2.isValid());
        assert(d2.getValue() == 42.5);
        cout << " \033[1;32m✅ Test 2 passed:\033[0m Valid DataValue with 42.5\n";
        passed++;
    } catch (const exception& e) {
        cout << " \033[1;31m❌ Test 2 failed:\033[0m Exception: " << e.what() << "\n";
    } catch (...) {
        cout << " \033[1;31m❌ Test 2 failed:\033[0m Unknown error\n";
    }

    // ---------------- Test 3 ----------------
    total++;
    try {
        DataValue d3(99.9, false);
        assert(!d3.isValid());
        assert(d3.getValue() == 99.9);
        cout << " \033[1;32m✅ Test 3 passed:\033[0m Custom invalid DataValue with 99.9\n";
        passed++;
    } catch (const exception& e) {
        cout << " \033[1;31m❌ Test 3 failed:\033[0m Exception: " << e.what() << "\n";
    } catch (...) {
        cout << " \033[1;31m❌ Test 3 failed:\033[0m Unknown error\n";
    }

    // ---------------- Résumé ----------------
    cout << "\n\033[1;33m─────────────────────────────────────────────\033[0m\n";
    if (passed == total)
        cout << "\033[1;32m✅ ALL TESTS PASSED (" << passed << "/" << total << ")\033[0m\n";
    else
        cout << "\033[1;31m⚠️  SOME TESTS FAILED (" << passed << "/" << total << ")\033[0m\n";
    cout << "\033[1;33m─────────────────────────────────────────────\033[0m\n";

    return (passed == total) ? 0 : 1;
}

int main() {
    return test_data_value();
}
