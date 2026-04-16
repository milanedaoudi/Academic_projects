#include "LoadUtils.hpp"
#include "Serial.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>

using namespace std;


void test_serial(const string& cpuConfig, const string& serialTarget) {
    cout << "\n\033[1;36m║ TEST SERIAL COMPONENT ║\033[0m\n";

    int total = 0, passed = 0;
    auto printAction  = [](const string& msg ){ cout << "\033[1;34m[Action]\033[0m " << msg << endl; };
    auto printInfo    = [](const string& msg ){ cout << "\033[1;36m[Info]\033[0m " << msg << endl; };
    auto printSuccess = [&](const string& msg ){ cout << "\033[1;32m✅ " << msg << "\033[0m\n"; passed++; total++; };
    auto printError   = [&](const string& msg){ cout << "\033[1;31m❌ " << msg << "\033[0m\n"; total++; };

    ofstream file1(serialTarget, ios::trunc);
    file1.close();
    
    

    try {
        printAction("Loading debuggable CPU from: " + cpuConfig );
        CPU cpu;
        cpu.LoadFileCpu(cpuConfig);
        DataValue val;

        printAction("Binding Serial to CPU and target file: " + serialTarget);
        Serial serial;
        serial.bind(&cpu);
        serial.setTarget(serialTarget);

        printInfo("Simulating CPU for debugging...");
        while(true){
            cpu.simulate();
            val = cpu.read();
            if(!val.isValid()) break;
        }

        printInfo("Writing debug buffer to file...");
        serial.simulate();

        ifstream file(serialTarget);
        if(!file.is_open()) {
            printError("Could not open target file: " + serialTarget);
            return;
        }
        

        vector<string> lines;
        string line;
        while(getline(file, line)) {
            if(!line.empty()) lines.push_back(line);
        }
        file.close();

        if(!lines.empty()) {
            printSuccess("Serial wrote " + to_string(lines.size()) + " instruction(s) to file");
            cout << "\033[1;36m────────── FILE CONTENT ──────────\033[0m\n";
            for(const auto& l : lines) cout << "  " << l << "\n";
            cout << "\033[1;36m──────────────────────────────────\033[0m\n";
        } else {
            printError("Serial output file is empty");
        }

    } catch (const exception& e) {
        cerr << "\033[1;31m❌ Exception caught: " << e.what() << "\033[0m\n";
    } catch (...) {
        cerr << "\033[1;31m❌ Unknown exception caught\033[0m\n";
    }

    cout << "\n\033[1;33m────────────────────────────\033[0m\n";
    if(passed == total) 
        printSuccess("ALL SERIAL TESTS PASSED (" + to_string(passed) + "/" + to_string(total) + ")");
    else 
        printError("SOME SERIAL TESTS FAILED (" + to_string(passed) + "/" + to_string(total) + ")");
    cout << "\033[1;33m────────────────────────────\033[0m\n";
}

int main() {
    try {
        test_serial("../data/cpu2.txt", "../data/target.txt");
    } catch (const exception& e) {
        cerr << "\033[1;31m❌ Exception caught in main: " << e.what() << "\033[0m\n";
        return 1;
    } catch (...) {
        cerr << "\033[1;31m❌ Unknown exception caught in main\033[0m\n";
        return 1;
    }
    return 0;
}
