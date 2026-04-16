#ifndef LOADUTILS_HPP
#define LOADUTILS_HPP

#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;

class LoadVar {
 public:
    virtual ~LoadVar() = default;               

    static int find_first_not_escape(const string& s) {
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
                return static_cast<int>(i);
        }
        return -1;
    }

    static int find_last_not_escape(const string& s) {
        if (s.empty()) return -1;
        for (int i = static_cast<int>(s.size()) - 1; i >= 0; --i) {
            if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
                return i;
        }
        return -1;
    }


     // Parsing générique d’un fichier key:value
    static map<string,string> parseFile(const string& filename) {
        map<string,string> component;

        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Impossible d'ouvrir le fichier " + filename);
        }

        string line;
        size_t lineNumber = 0;

        while (getline(file, line)) {
            lineNumber++;
            if (line.empty()) continue;

            size_t pos = line.find(':');
            if (pos == string::npos) {
                throw runtime_error("Format invalide à la ligne " + to_string(lineNumber) + " dans " + filename);
            }

            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            // Nettoyage des espaces
            key.erase(0, LoadVar::find_first_not_escape(key));
            key.erase(LoadVar::find_last_not_escape(key) + 1);
            value.erase(0, LoadVar::find_first_not_escape(value));
            value.erase(LoadVar::find_last_not_escape(value) + 1);

            component[key] = value;
        }

        file.close();
        return component;
    }
};
#endif