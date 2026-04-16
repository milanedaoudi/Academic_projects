#include <iostream>
#include <vector>
#include <cmath>
#include "Program.hpp"

using namespace std;

/**
 * Test classique avec 2 opérandes
 */
void test_program_colored(const string& configFile) {
    Program prog;

    // Lecture du fichier d'instructions
    prog.loadFromFile(configFile);
    prog.InitPointer();

    // Résultats attendus
    vector<double> expected = {5.0, 3.3, 15.006};
    vector<double> results;

    cout << "\033[1;36m║ TEST SUITE: Program & Instructions ║\033[0m\n";

    // --- Exécution ---
    while (!prog.finished()) {
        DataValue val = prog.compute();
        cout << "\033[1;32m[DEBUG] Computed: " << val.getValue() << "\033[0m\n";
        results.push_back(val.getValue());
    }

    // --- Vérification ---
    bool all_ok = true;

    if (results.size() != expected.size()) {
        all_ok = false;
        cout << "\033[1;31m❌ Mismatch number of results: got " << results.size()
             << ", expected " << expected.size() << "\033[0m\n";
    } else {
        for (size_t i = 0; i < results.size(); ++i) {
            if (fabs(results[i] - expected[i]) > 1e-5) {
                all_ok = false;
                cout << "\033[1;31m❌ Mismatch at instruction " << i
                     << ": got " << results[i]
                     << ", expected " << expected[i] << "\033[0m\n";
            } else {
                cout << "\033[1;32m✅ Instruction " << i
                     << " correct: " << results[i] << "\033[0m\n";
            }
        }
    }

    cout << "\n\033[1;32mProgram finished? \033[0m" 
         << (prog.finished() ? "yes" : "no") << endl;

    cout << "\033[1;33m────────────────────────────────────────────\033[0m\n";
    if (all_ok)
        cout << "\033[1;32m✅ ALL TESTS PASSED (" << results.size() << " results)\033[0m\n";
    else
        cout << "\033[1;31m⚠️ SOME TESTS FAILED\033[0m\n";
    cout << "\033[1;33m────────────────────────────────────────────\033[0m\n";
}

/**
 * Test pour instructions avec N opérandes (variadic)
 * Exemple fichier .txt:
 * ADD 2 3 1 1
 * SUB 4.5 1.2 1.3
 * MUL 5.002 3 0.008 0.99
 */
void test_program_multipleOp(const string& configFile) {
    Program prog;

    prog.loadFromFile(configFile);
    prog.InitPointer();

    // Résultats attendus calculés à la main
    vector<double> expected = {7.0, 2.0, 0.11884752}; // ADD:2+3+1+1, SUB:4.5-1.2-1.3, MUL:5.002*3*0.008*0.99
    vector<double> results;

    cout << "\033[1;36m║ TEST SUITE: Variadic Instructions ║\033[0m\n";

    while (!prog.finished()) {
        DataValue val = prog.compute();
        cout << "\033[1;32m[DEBUG] Computed: " << val.getValue() << "\033[0m\n";
        results.push_back(val.getValue());
    }

    // Vérification
    bool all_ok = true;
    if (results.size() != expected.size()) {
        all_ok = false;
        cout << "\033[1;31m❌ Mismatch number of results: got " << results.size()
             << ", expected " << expected.size() << "\033[0m\n";
    } else {
        for (size_t i = 0; i < results.size(); ++i) {
            if (fabs(results[i] - expected[i]) > 1e-6) {
                all_ok = false;
                cout << "\033[1;31m❌ Mismatch at instruction " << i
                     << ": got " << results[i]
                     << ", expected " << expected[i] << "\033[0m\n";
            } else {
                cout << "\033[1;32m✅ Instruction " << i
                     << " correct: " << results[i] << "\033[0m\n";
            }
        }
    }


    cout << "\033[1;33m────────────────────────────────────────────\033[0m\n";
    if (all_ok)
        cout << "\033[1;32m✅ ALL TESTS PASSED (" << results.size() << " results)\033[0m\n";
    else
        cout << "\033[1;31m⚠️ SOME TESTS FAILED\033[0m\n";
    cout << "\033[1;33m────────────────────────────────────────────\033[0m\n";
}


int main() {
    test_program_colored("../data/instSimple.txt");
    test_program_multipleOp("../data/instSimple2.txt");
    return 0;
}
