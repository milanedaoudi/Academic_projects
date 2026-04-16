#include <iostream>
#include <vector>
#include <cmath>
#include "Instruction.hpp"

using namespace std;

void test_inst() {

    cout << "\033[1;36m║ TEST INSTRUCTION WITH 2 AND MULTIPLE OPERANDS ║\033[0m\n";
    int passed = 0, total = 0;

    //[&] capture toutes les variables locales de la portée par référence
    auto runTest = [&](const string& name, const Instruction& inst, double expected) {
        total++;
        try {
            double result = inst.compute();
            cout << "  Operands: [ ";
            for (auto v : inst.getOperands()) cout << v << " ";
            cout << "]" << endl;
            if ((isnan(expected) && isnan(result)) || fabs(result - expected) < 1e-9) {
                cout << " \033[1;32m✅ [" << name << "] Passed. Result = " << result << "\033[0m\n";
                passed++;
            } else {
                cout << " \033[1;31m❌ [" << name << "] FAILED. Expected " << expected
                    << ", got " << result << "\033[0m\n";
            }
        } catch (const exception& e) {
            cout << " \033[1;31m❌ [" << name << "] EXCEPTION: " << e.what() << "\033[0m\n";
        } catch (...) {
            cout << " \033[1;31m❌ [" << name << "] UNKNOWN EXCEPTION\033[0m\n";
        }
    };


    // --- Tests 2 operands ---
    runTest("ADD", Instruction(OpCode::ADD, 3.0, 2.5), 5.5);
    runTest("SUB", Instruction(OpCode::SUB, 10.0, 4.0), 6.0);
    runTest("MUL", Instruction(OpCode::MUL, 4.0, 1.5), 6.0);
    runTest("DIV", Instruction(OpCode::DIV, 10.0, 2.0), 5.0);
    runTest("DIV Zero", Instruction(OpCode::DIV, 10.0, 0.0), nan(""));
    runTest("NOP", Instruction(OpCode::NOP, 100.0, 200.0), 0.0);

    // --- Tests variadique ---
    runTest("ADD*", Instruction(OpCode::ADD, 3.0, 2.5, 0.5, 1.0, 1.0, 2.0), 10.0);
    runTest("SUB*", Instruction(OpCode::SUB, 20.0, 3.0, 2.0, 5.0), 10.0);
    runTest("MUL*", Instruction(OpCode::MUL, 2.0, 3.0, 4.0), 24.0);
    runTest("DIV*", Instruction(OpCode::DIV, 100.0, 2.0, 5.0), 10.0);
    runTest("DIV* Zero", Instruction(OpCode::DIV, 10.0, 2.0, 0.0), nan(""));

    // --- Test buffer operands ---
    total++;
    try {
        Instruction inst_buf(OpCode::ADD, 1.0, 2.0, 3.0, 4.0);
        vector<double> expectedBuffer = {1.0, 2.0, 3.0, 4.0};
        vector<double> buffer = inst_buf.getOperands();
        cout << "  Buffer operands: [ ";
        for (auto v : buffer) cout << v << " ";
        cout << "]" << endl;
        if (buffer == expectedBuffer) {
            cout << " \033[1;32m✅ [BUFFER] getOperands correct\033[0m\n";
            passed++;
        } else {
            cout << " \033[1;31m❌ [BUFFER] FAILED. Expected [1 2 3 4], got [";
            for (auto v : buffer) cout << v << " ";
            cout << "]\033[0m\n";
        }
    } catch (...) {
        cout << " \033[1;31m❌ [BUFFER] EXCEPTION\033[0m\n";
    }

    cout << "\n\033[1;33m──────────────────────────────────────────────\033[0m\n";
    if (passed == total)
        cout << "\033[1;32m✅ ALL TESTS PASSED (" << passed << "/" << total << ")\033[0m\n";
    else
        cout << "\033[1;31m⚠️ SOME TESTS FAILED (" << passed << "/" << total << ")\033[0m\n";
    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";
}

int main() {
    test_inst();
    return 0;
}
