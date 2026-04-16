#ifndef PLATEFORM_HPP
#define PLATEFORM_HPP


#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include <string>
#include <vector>
#include <map>
#include "Component.hpp"
#include "Cpu.hpp"
#include "Memory.hpp"
#include "Bus.hpp"
#include "Display.hpp"
#include "Serial.hpp"

using namespace std;

class Platform : public Component  {
private:
    string type;    
    vector<Component*> components;
    map<string, Component*> labelToComponent;

    bool verbose = false;

public:
    Platform() = default;
    ~Platform();

    void setVerbose(bool v) { verbose = v; }

    void loadComponents(const string& platformFile);
    void bindComponents();
    void simulate(int steps);
    
    Component* findByLabel(const string& lbl) const;

    // Redéfinition de la pure virtuelle pour ne pas être abstraite
    void simulate() override { simulate(1); }


};
#endif
