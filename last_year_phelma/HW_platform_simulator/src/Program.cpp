#include "Program.hpp"


void Program::loadFromFile(const std::string& path) {
    instructions.clear();

    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Erreur : impossible d'ouvrir le fichier_ : " << path << endl;
        return;
    }

    string line;
    //lecture ligne par ligne
    while (getline(file, line)) {
        if (line.empty()) continue; // saut lignes vides

        stringstream ss(line);
        string opName;
        // Before multiple operande
        //double a, b;
        //ss >> opName >> a >> b;    // >> ignore les blancs : espaces, tab, \n.

        ss >> opName ;
       
        OpCode opcode;

        if (opName == "ADD") {
            opcode = OpCode::ADD;
        }else if (opName == "SUB"){
            opcode = OpCode::SUB;
        }
        else if (opName == "MUL"){
            opcode = OpCode::MUL;
        }
        else if (opName == "DIV"){
            opcode = OpCode::DIV;
        }else if(opName == "NOP"){
            opcode = OpCode::NOP;
        }
        else{
            opcode = OpCode::ERROR;
        }

        Instruction instr(opcode);
        instr.addOperand(ss);
        instructions.push_back(instr);
    }

    file.close();

    InitPointer(); 
    cout << "Load ended : " << instructions.size() << " instructions read." << endl;
}


//Init instruction Pointer to the first one
void Program::InitPointer() {
    if (!instructions.empty()) {
        InstPointer = &instructions[0];
    } else {
        InstPointer = nullptr;
    }
}

/*
 * Executes the instruction currently pointed to by InstPointer.
 * Advances the Program counter (currentIndex/InstPointer) to the next instruction.
 * Returns the result and its validity (based on OpCode) encapsulated in a DataValue.
 */
DataValue Program::compute() {
    
    if (finished()) { 
        return DataValue(0.0, false);
    }

    double currentResult = InstPointer->compute();

    // A result is valid if the OpCode is NOT a NOP (No Operation) or an ERROR.
    bool valid = (InstPointer->getOpCode() != OpCode::NOP && 
              InstPointer->getOpCode() != OpCode::ERROR && 
              !isnan(currentResult) );

    currentIndex++; 
    if (currentIndex < instructions.size()) {
        InstPointer = &instructions[currentIndex]; 
    } else {
        // If the index is out of bounds, set the pointer to null 
        // to signify the Program is fully executed.
        InstPointer = nullptr; 
    }
   return DataValue(currentResult, valid);
}

bool Program::finished() const {
    return (currentIndex >= instructions.size());
}

Program::Program(const Program& other) : instructions(other.instructions), 
                                         currentIndex(other.currentIndex){
    // 2. Recalcule la position du pointeur dans le NOUVEAU vecteur 'instructions'.
    // Ceci est crucial évite le pointeur pendu lors de la destruction de l'objet 'loader'.  
    if (other.InstPointer != nullptr && other.currentIndex < instructions.size()) {
        this->InstPointer = &this->instructions[this->currentIndex];
    } else {
        this->InstPointer = nullptr;
    }
}