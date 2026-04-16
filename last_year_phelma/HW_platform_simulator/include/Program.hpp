#ifndef PROGRAM_HPP   
#define PROGRAM_HPP

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>   
#include "Instruction.hpp"
#include "DataValue.hpp"

using namespace std;

class Program {
private:
    vector<Instruction> instructions;
    Instruction* InstPointer = nullptr; 
    size_t currentIndex = 0;

public:
    Program() = default;
    ~Program() = default;
    void loadFromFile(const string& programFile);
    void InitPointer();
    DataValue compute();

    Instruction* currentInstruction() const{ return InstPointer; }
    bool finished() const;

    // Use of copy constructor is mandatory to manage InstPointer's memory 
    // when the program is copied from a temporary loader object.
    // Without it, deleting the Loader creates a dangling pointer (shadow pointer)
    // in the main CPU object.
    Program(const Program& other);
};

#endif 