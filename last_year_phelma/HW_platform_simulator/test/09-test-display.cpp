#include <iostream>
#include <cassert>
#include <stdexcept>
#include "Display.hpp"

// -----------------------------------------------------
// Fake source: produit une séquence simple
// -----------------------------------------------------
class FakeSource : public IReadable {
    private:
        int counter = 0;
        int value = 1;
    public:
        DataValue read() override {
            counter++;
            if (counter == 3) { // toutes les 3 lectures → invalide
                counter = 0;
                return DataValue(0.0, false);
            }
            return DataValue(static_cast<double>(value++), true);
        }
};

// -----------------------------------------------------
// Test principal
// -----------------------------------------------------
void test_display() {
cout << "\033[1;36m║ TEST: Display Simulation ║\033[0m\n";

// === Test 1 : Display simulé avec source factice ===
FakeSource src;
Display disp("MEMORY", "Display1", 2);
disp.bindSource(&src);

for (int step = 1; step <= 4; ++step) {
    cout << "\n--- Step " << step << " ---\n";
    disp.simulate();
}

cout << "\n=== Test 1 terminé ✅ ===\n\n";


// === Test 2 : Chargement à partir d’un fichier ===
cout << "\033[1;36m║ TEST: Display depuis fichier ║\033[0m\n";
Display screen;
try {
    screen.LoadFileDisplay("../data/display.txt");  // ton fichier de config
    cout << "Type : " << screen.getType() << endl;
    cout << "Label : " << screen.getLabel() << endl;
    cout << "Refresh : " << screen.getRefresh() << endl;
    cout << "Source : " << screen.getSourceLabel() << endl;
}
catch (const exception& e) {
    cerr << "Erreur pendant le chargement du Display : " << e.what() << endl;
}

cout << "\n=== Test 2 terminé ✅ ===\n";

}

// -----------------------------------------------------
// Main avec gestion d’erreurs
// -----------------------------------------------------
int main() {
try {
test_display();
return 0;
}
catch (const bad_alloc& e) {
cerr << "Erreur mémoire: " << e.what() << endl;
return 1;
}
catch (const runtime_error& e) {
cerr << "Erreur d’exécution: " << e.what() << endl;
return 2;
}
catch (const exception& e) {
cerr << "Exception standard attrapée: " << e.what() << endl;
return 3;
}
catch (...) {
cerr << "Erreur inconnue détectée !" << endl;
return 99;
}
}
