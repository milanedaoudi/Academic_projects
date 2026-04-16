#ifndef UNIT_TEST

#include <iostream>
#include <string>
#include <stdexcept>   // pour std::exception
#include "Platform.hpp"

using namespace std;

/*
  make main_exe
  ./main_exe ../data/platform.txt steps -v     (-v optional)
*/

int main(int argc, char* argv[]) {
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";
    cout << "\033[1;35m║          SYSTEM-ON-CHIP PLATFORM TEST        ║\033[0m\n";
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";

    //  Default value
    string platformFile = "../data/platform.txt";  
    int totalSteps = 4;
    bool verbose = false;

    //  Read of arguments
    if (argc > 1)
        platformFile = string(argv[1]);  // argument 1 = file of platform

    if (argc > 2) {
        try {
            totalSteps = stoi(argv[2]);   // argument 2 = number of steps
        } catch (const exception& e) {
            cerr << "❌ Erreur: '" << argv[2] << "' is not a valid number for totalSteps.\n";
            return 1;
        }
    }

    if (argc > 3) {
        string arg3 = argv[3];
        verbose = (arg3 == "true" || arg3 == "1" || arg3 == "-v");
    }

    //  Résumé des paramètres
    cout << "\033[1;35m  Platform file : \033[0m" << platformFile << endl;
    cout << "\033[1;35m  Steps : \033[0m" << totalSteps << endl;
    cout << "\033[1;35m  Verbose : \033[0m" << (verbose ? "enabled" : "disabled") << endl;

    //  Simulation
    Platform platform;
    platform.setVerbose(verbose);

    platform.loadComponents(platformFile);
    platform.bindComponents();

    cout << "\nSimulating " << totalSteps << " steps...\n";
    platform.simulate(totalSteps);

    cout << "\n\033[1;32m✅ Platform simulation complete.\033[0m\n";
    cout << "Executed " << totalSteps << " cycles across all components.\n";

    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";
    cout << "Expected behavior:\n";
    cout << " - CPU executes its program instructions each cycle\n";
    cout << " - Memory stores values received from CPU\n";
    cout << " - Bus transmits values from memory to display\n";
    cout << " - Display shows results periodically (based on refresh rate)\n";
    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";

    return 0;
}

#endif
