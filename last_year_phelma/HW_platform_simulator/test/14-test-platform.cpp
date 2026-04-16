#include <iostream>
#include <string>
#include <cstdlib>     // for std::atoi
#include "Platform.hpp"

using namespace std;

void test_platform(const string& platformFile) {
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";
   cout << "\033[1;35m║          SYSTEM-ON-CHIP PLATFORM TEST       ║\033[0m\n";
    cout << "\033[1;35m═══════════════════════════════════════════════\033[0m\n";

    int totalSteps = 8;

    Platform platform;
    platform.setVerbose(false);

    // Charger tous les composants
    platform.loadComponents(platformFile);
    platform.bindComponents();

    // Simulation optionnelle
    platform.simulate(totalSteps);
}

int main() {
    //test_platform("../data/platform.txt");
    return 0;
}
