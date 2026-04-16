#ifndef BUS_HPP
#define BUS_HPP


#include <deque>
#include "Program.hpp"
#include "LoadUtils.hpp"
#include "Register.hpp"
#include "IReadable.hpp"
#include "Component.hpp"
#include "DataValue.hpp"


using namespace std;

class Bus : public Component, public IReadable {
private:
    int width;
    deque<DataValue> pending;
    deque<DataValue> ready;
    int readCount = 0;

    string sourceLabel;      // the label name of the source (from file)
    IReadable* source = nullptr;  // actual source object (bound later)
    
public:
    Bus() = default;
    Bus(string type , string lbl, int w) 
        : Component( type , lbl), width(w) {}

    // SOURCE 
    void setSourceLabel(const std::string& lbl) { sourceLabel = lbl; }
    string getSourceLabel() const { return sourceLabel; }
    void bindSource(IReadable* src) { 
        source = src;
    }
    
    // GETTER
    string getLabel() const { return label; }
    string getType() const { return type; }
    int getWidth() const { return width; }
    int getAccessCount() const { return readCount; }

    // DEBUG
    DataValue read() override;
    void printPending() const;
    void printReady() const;

    // LOAD
    void LoadFileBus(const string& configFile);
    
    void simulate() override;
   
};
#endif
