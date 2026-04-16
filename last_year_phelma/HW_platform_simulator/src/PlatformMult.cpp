#include "PlatformMult.hpp"


void PlatformMult::LoadFilePlatformMult(const string& configFile) {
    ifstream file(configFile);
    if (!file.is_open()) {
        cerr << "Erreur : impossible d'ouvrir " << configFile << endl;
        return;
    }

    string line;
    size_t lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        size_t pos = line.find(':');
        if (pos == string::npos) {
            cerr << "Erreur de format (':' attendu) à la ligne " << lineNumber
                 << ": " << line << endl;
            continue;
        }

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        // Nettoyage des espaces
        key.erase(0, LoadVar::find_first_not_escape(key));
        key.erase(LoadVar::find_last_not_escape(key) + 1);
        value.erase(0, LoadVar::find_first_not_escape(value));
        value.erase(LoadVar::find_last_not_escape(value) + 1);

        if (key == "TYPE") {
            if (value != "PLATFORM") {
                cerr << "Erreur : TYPE doit être 'PLATFORM' à la ligne "
                     << lineNumber << endl;
            } else {
                type = value;
            }
        } 
        else if (key == "LABEL") {
            label = value;
        } 
        else if (key == "COMPONENT") {
            if (value.empty()) {
                cerr << "Erreur : chemin de fichier manquant à la ligne "
                     << lineNumber << endl;
            } else {
                Platform* p = new Platform();
                p->loadComponents(value);
                subPlatforms.push_back(p);
            }
        } 
        else {
            cerr << "Clé inconnue à la ligne " << lineNumber << ": " << key << endl;
        }
    }

    file.close();
}

// ----------------------------
// Simulation of each platform
// -----------------------------
void PlatformMult::simulateVerbose(int steps, bool verbose) {
    cout << "\n\033[1;36m=== Simulating Platform of Platforms: \033[0m" << label << " ===" << endl;
    int i =0;
    for (auto* p : subPlatforms) {
        i++;
        if (verbose){
            p->setVerbose(verbose);
        }
        cout << "\n\033[1;32m--> ║ SIMULATING SUB-PLATFORM "  << i << "  ║  \033[0m"<< "  :  " << p->getLabel() << endl;
        p->simulate(steps);
    }
    cout << "\n=== Simulation complete for " << label << " ✅ ===" << endl;
}
