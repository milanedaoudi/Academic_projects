#ifndef UNIT_TEST

#include <iostream>
#include <string>
#include <stdexcept>   // pour std::exception
#include "PlatformMult.hpp"

using namespace std;
/*
  make main_exe2
  ./main_exe2 ../data/platformMult.txt steps -v     (-v optional)

*/
int main(int argc, char* argv[]) {
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";
    cout << "\033[1;35m║          TEST PLATFORM OF PLATFORM           ║\033[0m\n";
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";

    // Default value
    string platformFile = "../data/platformMult.txt";  
    int totalSteps = 4;
    bool verbose = false;

    //  Lecture des arguments
    if (argc > 1)
        platformFile = string(argv[1]); 

    if (argc > 2) {
        try {
            totalSteps = stoi(argv[2]);   
        } catch (const exception& e) {
            cerr << "❌ Erreur: '" << argv[2] << "' n'est pas un nombre valide pour totalSteps.\n";
            return 1;
        }
    }

    if (argc > 3) {
        string arg3 = argv[3];
        verbose = (arg3 == "true" || arg3 == "1" || arg3 == "-v");
    }

    
    cout << "\033[1;35m  Platform file : \033[0m" << platformFile << endl;
    cout << "\033[1;35m  Steps : \033[0m" << totalSteps << endl;
    cout << "\033[1;35m  Verbose : \033[0m" << (verbose ? "enabled" : "disabled") << endl;

    //  Simulation
    PlatformMult platformMult;
    platformMult.LoadFilePlatformMult(platformFile);

    // binding sub platform
    for (auto* sub : platformMult.getSubPlatforms()) {
        sub->bindComponents();
    }

    cout << "\nSimulating " << totalSteps << " steps...\n";
    platformMult.simulateVerbose(totalSteps, verbose);

   
    cout << "\n\033[1;32m✅ PlatformMult simulation complete.\033[0m\n";
    cout << "Executed " << totalSteps << " cycles across all sub-platforms.\n";

    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";
    cout << "Expected behavior:\n";
    cout << " - Each sub-platform simulates its own SoC system\n";
    cout << " - Data flows between sub-platforms as defined in the config file\n";
    cout << " - Verbose mode prints component activity in detail\n";
    cout << "\033[1;33m──────────────────────────────────────────────\033[0m\n";

    return 0;
}

#endif
