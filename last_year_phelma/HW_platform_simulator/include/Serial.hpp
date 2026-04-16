#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <vector>

#include "Cpu.hpp"
#include "Component.hpp"
#include "IDebuggable.hpp"


using namespace std;

//============ EXTRA SERIAL DEBUGGABLE ========================
class Serial : public Component , public IDebuggable {
    private:
        IDebuggable* source= nullptr;  // source "debuggable"
        string sourceLabel;

        string targetFile;

    public:

        Serial() = default;
        Serial(string type , string lbl )
            : Component(type , lbl ) {}

        // GETTER & SETTER
        void setTarget(const string& file) { targetFile = file; }
        string getTarget() { return targetFile; }

         // SOURCE
        void setSourceLabel(const string& lbl) { sourceLabel = lbl; }
        string getSourceLabel() const { return sourceLabel; }
        void bind(IDebuggable* dbg) { source = dbg; }

        void simulate();

        // DEBUG
        vector<string> getDebugBuffer() override { return {}; }
        void clearDebugBuffer() override {}

        // LOAD
        void LoadFileSerial(const string& configFile);
};

//========================== END EXTRA ==============================
#endif
