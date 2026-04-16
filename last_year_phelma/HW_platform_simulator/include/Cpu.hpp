#ifndef CPU_HPP
#define CPU_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <queue>
#include "Program.hpp"
#include "LoadUtils.hpp"
#include "Register.hpp"
#include "IReadable.hpp"
#include "IDebuggable.hpp"
#include "Component.hpp"
#include "DataValue.hpp"
#include "Program.hpp"

using namespace std;

class CPU : public Component, public IReadable ,public IDebuggable  {
private:
    
    Register reg;
    int frequency;
    int numbCores;
    int activeCore;

    Program program;
    string progName;
    
    //============ EXTRA DEBUGGABLE==================
    vector<string>  DebugBuffer;  //store instruction when cpu compute
    //============= END EXTRA  ======================
public:
    CPU() = default;
    CPU(string type , string lbl, int freq, int cores, Program prog)
        : Component(type , lbl ), frequency(freq), numbCores(cores), activeCore(0), program(prog) {}

    void simulate() override;
    DataValue read() override;

    // GETTER
    int getFreq() const { return frequency; }
    int getCores() const { return numbCores; }
    int getActiveCore() const { return activeCore; }

    string getLabel() const { return label; }
    string getType() const { return type; }
    string getProgName() { return progName; }
    Program& getProgram() { return program; }

    // LOAD
    void LoadFileCpu(const string& file);

    //============ EXTRA DEBUGGABLE==================
    string toString(OpCode code);       // From ADD -> "ADD" for TargetFile in Serial component
    vector< string> getDebugBuffer(){ return DebugBuffer; }; 
    void clearDebugBuffer() { DebugBuffer.clear(); }
    //============= END EXTRA  ======================
    
};

#endif
