#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "LoadUtils.hpp"
#include "IReadable.hpp"
#include "Component.hpp"

using namespace std; 

class Display : public Component {
private:
    int refreshRate;
    int stepCounter = 0;
    
    IReadable* source = nullptr;
    string sourceLabel;

public:
    Display() = default;
    Display(string type , string lbl, int r)
        : Component(type , lbl), refreshRate(r) {}

    // SOURCE
    void setSourceLabel(const string& lbl) { sourceLabel = lbl; }
    string getSourceLabel() const { return sourceLabel; }

    // Bind to a source object and store its label
    void bindSource(IReadable* src) { 
        source = src;  
    }

    // GETTER
    int getRefresh(){return refreshRate;}

    void simulate() override;

    // LOAD
    void LoadFileDisplay(const string& file);
};
#endif