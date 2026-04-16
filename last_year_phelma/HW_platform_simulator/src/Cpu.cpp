#include <iostream>
#include "Cpu.hpp"



/**-----------------------------------------------------------------
 * Simulates the execution of instructions on the CPU.
 * Executes up to 'frequency' instructions per call for the active core.
 * Supports multi-core execution by switching to the next core when finished.
 *
 * Instructions are executed sequentially from the program, and results
 * are written to the CPU register. Optional debug output is generated
 * showing the operation and operands.
 -----------------------------------------------------------------*/
void CPU::simulate() {

    for (int i = 0; i < frequency; ++i) {
        if (program.finished()) {
            break;      // Stop if the program is done
        }
        
        Instruction* instr = program.currentInstruction();
  
        DataValue result = program.compute();


//============== EXTRA "DEBUGGABLE"===================================
       // Construct a debug string like: "ADD 2 3 = 5"
        ostringstream line;
        line << toString(instr->getOpCode());

        try {
            for (double Operand : instr->getOperands()){ 
                line << " " << Operand;
            }
        } catch (const exception& e) {
             cerr << "Erreur lors de l'accès aux opérandes: " << e.what() << endl;
            return;
        }
        line << " = " << result.getValue();
        
        // Output debug information and store in debug buffer
        //cout << "\033[1;33mstore in debug buffer:  \033[0m"<<line.str() << endl;
        
       DebugBuffer.push_back(line.str());
//================= END EXTRA =========================================

        // Write valid results to the CPU register
        if (result.isValid()) {
            reg.write(result.getValue());
        } 
    }

    // Move to the next core if the current program finished
    if (program.finished() && activeCore < numbCores - 1) {
        activeCore++;          
       program.InitPointer();
    }

}



/**-----------------------------------------------------------------
* Reads the current value from the CPU register.
 -----------------------------------------------------------------*/
DataValue CPU::read() {
    DataValue val = reg.read(); 
    return val; 
}



/*-----------------------------------------------------------------
LoadFileCpu : loads CPU configuration from a .txt file
Required fields : TYPE, LABEL, CORES, FREQUENCY, PROGRAM
Optional fields : PRIORITY
-----------------------------------------------------------------*/
void CPU::LoadFileCpu(const string& file) {
    try {
        auto cpu = LoadVar::parseFile(file);

        // ==== Required fields ==== 
        if (!cpu.count("TYPE")) 
            throw runtime_error("Missing key 'TYPE' in " + file); 
        if (!cpu.count("LABEL")) 
            throw runtime_error("Missing key 'LABEL' in " + file); 
        if (!cpu.count("CORES")) 
            throw runtime_error("Missing key 'CORES' in " + file); 
        if (!cpu.count("FREQUENCY")) 
            throw runtime_error("Missing key 'FREQUENCY' in " + file); 
        if (!cpu.count("PROGRAM")) 
            throw runtime_error("Missing key 'PROGRAM' in " + file);

        type = cpu.at("TYPE");
        label = cpu.at("LABEL");

        try { 
            numbCores = stoi(cpu.at("CORES"));
            frequency = stoi(cpu.at("FREQUENCY"));
        } catch (const invalid_argument&) { 
             throw runtime_error("Non-numeric value in 'CORES' or 'FREQUENCY' for " + file);
        }
     
        program.loadFromFile(cpu.at("PROGRAM"));
        progName = cpu.at("PROGRAM"); 

        if(cpu.count("PRIORITY")) { setPriority(stoi(cpu.at("PRIORITY")));}
    
        } catch (const runtime_error& e) {
        cerr << "Error while loading CPU (" << file << "):" << e.what() << endl;
        } catch (const exception& e) {
            cerr << "Unexpected error while loading CPU (" << file << "): " << e.what() <<endl;
        }
}


//============== EXTRA "DEBUGGABLE"===================================
string CPU::toString(OpCode code) {
    switch (code) {
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::NOP: return "NOP";
        default: return "ERROR";
    }
}
//================= END EXTRA =========================================