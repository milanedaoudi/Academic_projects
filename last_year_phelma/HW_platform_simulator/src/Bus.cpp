#include <iostream>
#include "Bus.hpp"

// -----------------------------
// simulate()
// -----------------------------
// Each simulation step, the Bus:
//  1. Moves all pending values to ready
//  2. Clears pending
//  3. Reads up to 'width' values from the source
//     (stopping if an invalid DataValue is obtained)
// -----------------------------
void Bus::simulate() {
    // Step 1: Move pending → ready
    for (auto& val : pending)
        ready.push_back(val);
        
    pending.clear();

    // Step 2: Read from the source up to 'width' times
    if (!source) {
        std::cerr << "Warning: Bus '" << label << "' has no bound source.\n";
        return;
    }

    for (int i = 0; i < width; ++i) {
        DataValue val = source->read();
        if (!val.isValid())
            break; // stop when invalid value obtained
        pending.push_back(val);
    }
}


// -----------------------------
// read()
// -----------------------------
// Returns the oldest DataValue from the ready queue.
// If none are available, returns an invalid DataValue.
// Increments read counter each time.
// -----------------------------
DataValue Bus::read() {
    if (ready.empty())
        return DataValue(0.0, false);

    DataValue val = ready.front();
    ready.pop_front();
    ++readCount;
    return val;
}


void Bus::printPending() const {
    cout << "  [Bus] Pending queue (" << pending.size() << "): [ ";
    for (const auto& val : pending) {
        if (val.isValid()) cout << fixed << val.getValue() << " ";
    }
    cout << "]\n";
}

void Bus::printReady() const {
    cout << "  [Bus] Ready queue (" << ready.size() << "): [ ";
    for (const auto& val : ready) {
        if (val.isValid()) cout << fixed << val.getValue() << " ";
    }
    cout << "]\n";
}




/*-----------------------------------------------------------------
LoadFileMEMORY : loads MEMORY configuration from a .txt file
Required fields : TYPE, LABEL, WIDTH, SOURCE
Optional fields : PRIORITY
-----------------------------------------------------------------*/
void Bus::LoadFileBus(const string& file) {
    try {
        auto bus = LoadVar::parseFile(file);

        // ==== Required fields ====
        if (!bus.count("TYPE"))
            throw runtime_error("Missing key 'TYPE' in " + file);
        if (!bus.count("LABEL"))
            throw runtime_error("Missing key 'LABEL' in " + file);
        if (!bus.count("WIDTH"))
            throw runtime_error("Missing key 'WIDTH' in " + file);
        if (!bus.count("SOURCE"))
            throw runtime_error("Missing key 'SOURCE' in " + file);

        type = bus.at("TYPE");
        label = bus.at("LABEL");
        sourceLabel = bus.at("SOURCE");


        try {
            width = stoi(bus.at("WIDTH"));
        } catch (const invalid_argument&) {
            throw runtime_error("Non-numeric value for 'WIDTH' in " + file);
        }

        // ==== Optional fields ====
        if (bus.count("PRIORITY")) {
            try {
                setPriority(stoi(bus.at("PRIORITY")));
            } catch (const invalid_argument&) {
                cerr << "Warning: invalid 'PRIORITY' value, ignored in " << file << endl;
            }
        }

    } catch (const runtime_error& e) {
            cerr << "Error while loading BUS (" << file << "): " << e.what() << endl;
    } catch (const exception& e) {
            cerr << "Unexpected error while loading BUS (" << file << "): " << e.what() << endl;
    }
}
