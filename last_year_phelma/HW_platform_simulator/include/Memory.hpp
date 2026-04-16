#ifndef MEMORY_HPP
#define MEMORY_HPP


#include <vector>
#include "LoadUtils.hpp"
#include "IReadable.hpp"
#include "Component.hpp"
#include "DataValue.hpp"

using namespace std;


class Memory : public Component, public IReadable {
private:
    int size=0;
    int accessTime=0;
    int stepCounter = 0;

    IReadable* source = nullptr;
    string sourceLabel;

    vector<double> buffer;
    int head = 0, tail = 0;
    bool full = false;
    //============ EXTRA MEMORY ROM & RWM ==========
    bool MemTypeRom = false;
    //============ END EXTRA ======================

public:
    Memory() = default;
    Memory(string type , string label, int s, int acc)
        : Component(type , label), size(s), accessTime(acc), buffer(s) {}

    // SOURCE
    void setSourceLabel(const string& label) { sourceLabel = label; }
    string getSourceLabel() const { return sourceLabel; }
    void bindSource(IReadable* source_t) { 
        source = source_t; 
    }

    // GETTER
    int getStepCounter() const{ return stepCounter;}

    vector<double> getBuffer() const {
        vector<double> buf(size, 0.0);
        
        for (int i = 0; i < size; ++i) {
            buf[i] = buffer[i]; 
        }
        return buf;
    }

    int getSize() const { return size; }
    int getAccessTime() const { return accessTime; }
    
    string getLabel() const { return label; }
    string getType() const { return type; }

    void simulate() override;
    DataValue read() override;

    // LOAD
    void LoadFileMem(const string& file);
    void BufferInit( const string& initValue);


    //============ EXTRA MEMORY ROM & RWM==========
     bool getMemTypeRom() const { return MemTypeRom; }
    void setMemTypeRom(bool value) { MemTypeRom = value; }
    //============ END EXTRA ======================
};

#endif