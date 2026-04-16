#include "Memory.hpp"
#include <iostream>

/*-----------------------------------------------------------------
* Simulates the Memory component by reading values from its bound source 
  Once every 'accessTime' steps.
* For non-ROM memory, values are stored in 
  a circular buffer, overwriting the oldest data when full. 
* ROM memory is read-only and does not modify the buffer.
-----------------------------------------------------------------*/
void Memory::simulate() {
    if (!source) {
        cerr << "Warning: Memory '" << label << "' has no bound source.\n";
        return;
    }

    // Increment step counter
    stepCounter++;

    // React only once every 'accessTime' steps
    if (stepCounter < accessTime) {
        return; // do nothing this step
    }

    // Reset the counter after reacting
    stepCounter = 0;

    // Read from the source until an invalid value is obtained
    if(!MemTypeRom){
        while (true) {
            DataValue val = source->read();
            if (!val.isValid()) break;

            // Store the valid value in the circular buffer
            buffer[tail] = val.getValue();
            tail = (tail + 1) % size;

            // If full, advance head as well (overwrite oldest)
            if (full) {
                head = (head + 1) % size;
            }

            // Mark as full if head catches tail
            if (tail == head) {
                full = true;
            }
        }
    }
}


/*-----------------------------------------------------------------
* Returns the next oldest DataValue from the buffer. 
* For ROM, values are read sequentially; (read in the order as they were loaded)
* For non-ROM, values are consumed in FIFO order. 
-----------------------------------------------------------------*/
DataValue Memory::read() {
    
    if( buffer.empty() ){
            return DataValue(0.0, false);
    }

    if(MemTypeRom){
        if( head >=  static_cast<int>(buffer.size()) ){
            head = 0;
            return DataValue(0.0, false);
        }
        double value = buffer[head];
        head++;
  
        return DataValue(value, true);
    } else {
        if (!full && head == tail) {
            return DataValue(0.0, false); 
        }

            // Get the oldest value
            double value = buffer[head];
            head = (head + 1) % size;

            // Once we consume a value, it's no longer full
            full = false;

            return DataValue(value, true);
        }
}




/*-----------------------------------------------------------------
LoadFileMEMORY : loads MEMORY configuration from a .txt file
Required fields : TYPE, LABEL, SIZE, ACCES, SOURCE
Optional fields : PRIORITY
-----------------------------------------------------------------*/
void Memory::LoadFileMem(const string& file) {
    
    try{
        auto mem = LoadVar::parseFile(file);

        if (!mem.count("TYPE")) 
            throw runtime_error("Missing key 'TYPE' in " + file); 
        if (!mem.count("LABEL")) 
            throw runtime_error("Missing key 'LABEL' in " + file);
        if (!mem.count("SIZE")) 
            throw runtime_error("Missing key 'SIZE' in " + file);
        if (!mem.count("ACCESS")) 
            throw runtime_error("Missing key 'ACCESS' in " + file); 
        if (!mem.count("SOURCE")) 
            throw runtime_error("Missing key 'SOURCE' in " + file);

        type = mem.at("TYPE");
        MemTypeRom = (type == "ROM");

        label = mem.at("LABEL");
        sourceLabel = mem.at("SOURCE");

        try{
            size = stoi(mem.at("SIZE") );   
            accessTime = stoi(mem.at("ACCESS") );
        } catch(const invalid_argument& e){
            throw runtime_error("Non-numeric value for 'SIZE' or 'ACCESS'' in " + file);
        }

         buffer.resize(size, 0.0);

        if (mem.count("PRIORITY")) {
            try { 
                setPriority(stoi(mem.at("PRIORITY"))); 
            } catch (const invalid_argument&) { 
                cerr << " Warning: invalid 'PRIORITY' value, ignored in " << file << endl; 
            } 
        }

        if(mem.count("INIT")) { BufferInit( mem.at("INIT") ); }  

    } catch(const runtime_error& e){
        cerr << "Error while loading MEMORY (" << file << "): " << e.what() << endl;
    } catch(const exception& e) {
        cerr << "Unexpected error while loading MEMORY (" << file << "): " << e.what() << endl;
    }
}



void Memory::BufferInit(const string& initValue) {
   
    this->buffer.clear(); // vider avant de remplir
    istringstream iss(initValue);
    double val;
    while (iss >> val) {
        this->buffer.push_back(val);
    }
    while (buffer.size() < static_cast<size_t>(size)) {
        buffer.push_back(0.0);
    }
}