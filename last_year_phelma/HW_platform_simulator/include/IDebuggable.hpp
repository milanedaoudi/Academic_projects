#ifndef DEBUGGABLE_HPP
#define DEBUGGABLE_HPP

#include "Instruction.hpp"
#include "DataValue.hpp"
#include <vector>

using namespace std;

class IDebuggable {
public:
    virtual vector< string> getDebugBuffer() = 0; 
    virtual void clearDebugBuffer() = 0 ;
    virtual ~IDebuggable() = default;
};
#endif
