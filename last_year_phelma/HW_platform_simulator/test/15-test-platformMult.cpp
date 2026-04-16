#include <iostream>
#include "PlatformMult.hpp"  // doit contenir Platform, CPU, Memory, Bus

using namespace std;

void test_platformMult(const string& platformFile) {
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";
   cout << "\033[1;35m║          TEST PLATFORM OF PLATFORM        ║\033[0m\n";
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";

    PlatformMult platformMult;
    int totalSteps = 3;

    cout << "\033[1;36mLOADING TOP PLATFORM:  \033[0m" << platformFile << endl; 
    
    platformMult.LoadFilePlatformMult(platformFile);
  
    for (auto* sub : platformMult.getSubPlatforms()) {
         sub->bindComponents();
    }

    platformMult.simulateVerbose(totalSteps, false); 
    cout << "\n\033[1;32m✅ Simulation finished successfully.\n\033[0m";
}

int main() {
    test_platformMult("../data/platformMult.txt");
    return 0;
}