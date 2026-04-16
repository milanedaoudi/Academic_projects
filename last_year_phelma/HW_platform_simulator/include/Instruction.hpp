#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cmath> 
#include "LoadUtils.hpp"

using namespace std;

enum class OpCode { NOP, ADD, SUB, MUL, DIV, ERROR };

class Instruction {

private:
  OpCode opC;        
  //double operand1, operand2;            case of 2 operands
  vector<double> operand;      

public:

    // Instruction(OpCode o, double a, double b)        // compute directely operand1,2
    //        : opC(o), operand1(a), operand2(b) {}
            
    /*
    *  Constructeur variadique acceptant un nombre variable d’opérandes.
    *  Args Types des opérandes (ici double)
    */

    //============ EXTRA VARIADIC OPERANDS======================
    template<typename... Args>
    Instruction(OpCode o, Args... args)
    : opC(o), operand{static_cast<double>(args)...} {}

    double compute() const {
        switch (opC) {
            case OpCode::ADD: return sum();
            case OpCode::SUB: return sub();
            case OpCode::MUL: return mult();
            case OpCode::DIV: return divide();
            case OpCode::NOP: return 0.0;
            default: return std::nan("");
        }
    }

    OpCode getOpCode() const { return opC; }

    void addOperand( stringstream& ss) {
        double op;
        while( ss >> op){
            operand.push_back(op);
            
        }
    }

    const vector<double>& getOperands() const {
        return operand;
    }
    
    //============ END EXTRA ======================

//For multiple operands
private:

    double sum() const {
    double result = 0.0;
    for (auto v : operand) result += v;
        return result;
    }

    double sub() const {
        if (operand.empty()) return 0.0;
        double result = operand[0];
        for (size_t i = 1; i < operand.size(); ++i)
            result -= operand[i];
        return result;
    }

    double mult() const {
        if (operand.empty()) return 0.0;
        double result = 1.0;
        for (auto v : operand) result *= v;
        return result;
    }

    double divide() const {
        if (operand.empty()) return std::nan("");
        double result = operand[0];
        for (size_t i = 1; i < operand.size(); ++i) {
            if (operand[i] == 0) return std::nan("");
            result /= operand[i];
        }
        return result;
    }
    
};

#endif
