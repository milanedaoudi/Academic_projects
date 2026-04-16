#include <iostream>
#include <string>
#include <cassert> 
#include <vector>
#include <cmath>
#include "Memory.hpp"
#include "IReadable.hpp"
#include "DataValue.hpp"


using namespace std;
// -------------------------------
// Fake source: produces one fixed value then stops
// -------------------------------
class FakeSource : public IReadable {
private:
    double value;
    bool hasSent = false;

public:
    explicit FakeSource(double v) : value(v) {}

    DataValue read() override {
        if (hasSent)
            return DataValue(0.0, false); // no more valid data
        hasSent = true;
        return DataValue(value, true);
    }
};

// -------------------------------
// Test writing to RWM (RAM) from multiple sources
// -------------------------------
void test_write_rwm() {
    cout << "\n\033[1;36m══════════ TEST WRITE RWM (CIRCULAR BUFFER) ══════════\033[0m\n";

    // Create 5 fake sources producing one unique value each
    vector<FakeSource*> sources = {
        new FakeSource(1),
        new FakeSource(2),
        new FakeSource(3),
        new FakeSource(4),
        new FakeSource(5)
    };


    // Memory capacity = 3, access time = 1. Utiliser "MEMORY" ou "RWM".
    Memory mem( "RWM" , "RAM_MULTI_SRC", 3, 1);

    
    // Feed data from each fake source one by one
    for (size_t i = 0; i < sources.size(); ++i) {
        mem.bindSource(sources[i]);
        cout << "\n\033[1;33m[Cycle " << (i + 1) << "] Source #" << (i + 1)
             << " -> value " << (i + 1) << "\033[0m\n";
        mem.simulate();

        // Debug: show content of buffer
        vector<double> buffer = mem.getBuffer(); 
        cout << "  [DEBUG] Buffer after write: ";
        for (double v : buffer) cout << v << " ";
        cout << "\n";
    }

    cout << "\n\033[1;32m✅ All sources written sequentially.\033[0m\n";
    cout << "Attendue: Le buffer de taille 3 doit contenir les 3 dernières valeurs (3, 4, 5).\n";

    // Cleanup
    for (auto* s : sources) delete s;
}

// -------------------------------
// Test reading RWM (RAM) contents (Destructive Read)
// -------------------------------
void test_read_rwm() {
    cout << "\n\033[1;36m══════════ TEST READ RWM (DESTRUCTIVE READ) ══════════\033[0m\n";

    // Create fake sources again
    vector<FakeSource*> sources = {
        new FakeSource(1),
        new FakeSource(2),
        new FakeSource(3),
        new FakeSource(4),
        new FakeSource(5)
    };

    Memory mem( "RWM", "RAM_READ_MULTI", 3, 1);

    // Fill the memory. Buffer ends up with [3, 4, 5]
    for (auto* s : sources) {
        mem.bindSource(s);
        mem.simulate();
    }

    cout << "\n\033[1;33m--- Reading stored values ---\033[0m\n";
    while (true) {
        DataValue val = mem.read();
        if (!val.isValid())
            break;
        cout << "  🔹 Value read from memory: " << val.getValue() << "\n";
    }
   
    cout << "\n\033[1;32m✅ Reading complete.\033[0m\n";
    cout << "Attendue: Les valeurs lues doivent être (3, 4, 5) \n";

    for (auto* s : sources) delete s;
}

// -------------------------------
// Test reading ROM contents (Cyclic Read)
// -------------------------------
void test_read_rom() {
    cout << "\n\033[1;36m══════════ TEST READ ROM (CYCLIC READ) ══════════\033[0m\n";

    // ROM de taille 3, initialisée avec [1, 2, 3]
    string value = "1.0 2.0 3.5";
    
    // Utiliser "ROM" pour le type.
    Memory mem( "ROM", "ROM_READ_TEST", 3, 1);
    mem.BufferInit(value);
    mem.setMemTypeRom(true);
    cout << "\n\033[1;33m--- Tentative de lecture cyclique (5 fois) ---\033[0m\n";
    for(int i = 0; i < 5; ++i) {
        DataValue val = mem.read();
        string status = val.isValid() ? "VALID" : "INVALID";
        cout << "  Value read (Cycle " << i + 1 << "): ";
        cout << val.getValue() << " (" << status << ")" << "\n";
    }


    
    

    cout << "\n\033[1;32m✅ Reading complete.\033[0m\n";
    cout << "Attendue: (1, 2, 3.5,(0.0, false), 1, 2, 3.5) si MemTypeRom est actif.\n";
}

// -------------------------------
// Test writing ROM (Write Protection)
// -------------------------------
void test_file_write_validation(const string& configFile) {
    cout << "\n\033[1;36m══════════ TEST ÉCRITURE AVEC FICHIER : " << configFile << " ══════════\033[0m\n";

    Memory mem;
    mem.LoadFileMem(configFile);
    
    // Le temps d'accès est défini par le fichier
    int required_steps = mem.getAccessTime();

    // La mémoire doit avoir été initialisée avec des données via INIT dans mem2.txt
    vector<double> initial_buffer = mem.getBuffer();
    cout << "\033[1;33m [DEBUG] Buffer initial (après LoadFileMem): \033[0m";
    for (double w : initial_buffer) cout << w << " ";
    cout << "\n";
    
    //assert(mem.getMemTypeRom() == true)
    
    vector<double> final_buffer;
    // Tentative d'écrire une valeur claire pour la validation
    FakeSource source(1);
    mem.bindSource(&source);
    
    cout << "\033[1;33m [DEBUG] Simulation avec " << required_steps << " pas l'écriture.\033[0m\n";
    
    // BOUCLE POUR ATTENDRE LE TEMPS D'ACCÈS
    for (int step = 0; step < required_steps; ++step) {
        cout << "=============CYCLE " << step << "===============" << endl;
        mem.simulate();
        final_buffer = mem.getBuffer();
        cout << "\033[1;33m [DEBUG] Buffer après tentative d'écriture (1): \033[0m";
        for (double w : final_buffer) cout << w << " ";
        cout << "\n";
        }
    if (mem.getMemTypeRom()) {
        // ROM: La première valeur (index 0) ne doit pas avoir changé
        assert(fabs(final_buffer[0] - initial_buffer[0]) < 1e-6);
        cout << "\033[1;32m✅ ASSERT PASSED: ROM - La valeur initiale (" << initial_buffer[0] << ") est inchangée (Protection en écriture).\033[0m\n";
    } else {
        // RWM: L'écriture doit avoir eu lieu. On vérifie l'index 0 (l'emplacement exact peut varier)
        // Note: Dans le cas de mem1.txt, le buffer est vide après chargement. L'écriture se fait à l'index 0.
        assert(fabs(final_buffer[0] - 1) < (1e-6)); 
        cout << "\033[1;32m✅ ASSERT PASSED: RWM - L'écriture (" << final_buffer[0] << ") a réussi à l'index 0.\033[0m\n";
    }

    cout << "\033[1;36m══════════ FIN TEST FICHIER " << configFile << " ══════════\033[0m\n";
}

void test_Mem( const string& file) {
    Memory mem;
    // Charge le fichier, définit TYPE et SIZE, et positionne correctement MemTypeRom
    mem.LoadFileMem(file); 

    bool all_ok = true;
    FakeSource* Source = new FakeSource(10);
    mem.bindSource(Source);

    // Tentative d'écriture (sera ignorée si TYPE:ROM dans le fichier)
    mem.simulate(); 

    if (mem.getSize() <= 0) { 
    cout << " taille :" << mem.getSize() << endl;    
    }

    // Vérification TYPE
     if (mem.getType().empty()) {
        all_ok = false;
        cout << " \033[1;31m❌ TYPE: empty\033[0m\n";
    }
    string TYPE = mem.getType();

    if ( TYPE == "MEMORY" ) {
        cout << " \033[1;32m✅ TYPE: MEMORY" << "\033[0m\n";
    }
    if ( TYPE == "RWM") {
        cout << " \033[1;32m✅ TYPE: RWM" << "\033[0m\n";
    }
     else if(  TYPE == "ROM"  ){
        cout << " \033[1;32m✅ TYPE: ROM " << "\033[0m\n";
    } else { 
        all_ok = false;
        cout << " \033[1;31m❌ TYPE: is not ROM, RWM, or MEMORY \033[0m\n";
    } 
    // Vérification LABEL
    if (mem.getLabel().empty()) {
        all_ok = false;
        cout << " \033[1;31m❌ LABEL: empty\033[0m\n";
    } else {
        cout << " \033[1;32m✅ LABEL: " << mem.getLabel() << "\033[0m\n";
    }
    if(  TYPE == "ROM"  ){    
        if(!mem.getMemTypeRom()){
            cout << "\n\033[1;31m❌   ROM Flag is false.\033[0m\n";
        } else {
            cout << "\n\033[1;32m✅  ROM Flag is true.\033[0m\n";
        };
    }
    if (mem.getSize() <= 0 ) {
        all_ok = false;
        cout << " \033[1;31m❌ SIZE: invalid\033[0m\n";
    } else {
        cout << " \033[1;32m✅ SIZE: " << mem.getSize() << "\033[0m\n";
    }
     if (mem.getAccessTime() <= 0 ) {
        all_ok = false;
        cout << " \033[1;31m❌ ACCESS: invalid\033[0m\n";
    } else {
        cout << " \033[1;32m✅ ACCESS: " << mem.getAccessTime() << "\033[0m\n";
    }

    vector<double> buffer1 = mem.getBuffer();
        cout << "  [DEBUG] Buffer after load and one simulation step: ";
        for (double v : buffer1) cout << v << " ";
        cout << "\n";
        
    

    cout << "────────────────────────────\n";
    if (all_ok) cout << "✅ ALL TESTS PASSED\n";
    else cout << "⚠️ SOME TESTS FAILED\n";
    cout << "────────────────────────────\n";
}

// -------------------------------
// Main test runner
// -------------------------------
int main() {
    cout << "\033[1;36m║ TEST SUITE: Memory Logic Correction ║\033[0m\n";
    
    cout << "\n\033[1;36m══════════ TESTS DE CHARGEMENT DE FICHIER (RWM et ROM) ══════════\033[0m\n";
    test_Mem("../data/mem2.txt");
  
    test_Mem("../data/mem1.txt");

    cout << "\n\033[1;36m══════════ TESTS DE COMPORTEMENT RWM/RAM (Constructor) ══════════\033[0m\n";
    test_write_rwm();
    test_read_rwm();
    
    cout << "\n\033[1;36m══════════ TESTS DE COMPORTEMENT ROM (Constructor) ══════════\033[0m\n";
    test_read_rom();
    test_file_write_validation("../data/mem1.txt");

    test_file_write_validation("../data/mem2.txt");

    return 0;
}
