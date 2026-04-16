#include "Display.hpp"
#include <iostream>


/*-----------------------------------------------------------------
* Updates the Display component by reading all valid values from its source
* Once every 'refreshRate' steps. If no source is bound, a warning is printed.
-----------------------------------------------------------------*/
void Display::simulate() {
    if (!source) {
        cerr << "Warning: Display '" << label << "' has no bound source.\n";
        return;
    }

    // Increment step counter
    stepCounter++;

    // Only react once every refreshRate steps
    if (stepCounter < refreshRate) {
        return; 
    }

    // Reset counter
    stepCounter = 0;

    // Read and print all values from source until invalid
    while (true) {
        DataValue val = source->read();
        if (!val.isValid()) break;

        // Print the value
        cout << label << " reads: " << val.getValue() << endl;
    }
}


/*-----------------------------------------------------------------
LoadFileDisplay : loads Display configuration from a .txt file
Required fields : TYPE, REFRESH, SOURCE
Optional fields : LABEL, PRIORITY
-----------------------------------------------------------------*/
void Display::LoadFileDisplay(const string& file) {
    
    try{
        auto display = LoadVar::parseFile(file);

        // ==== Required fields ====
        if (!display.count("TYPE")) 
            throw runtime_error("Missing key 'TYPE' in " + file); 
        if (!display.count("REFRESH")) 
            throw runtime_error("Missing key 'REFRESH' in " + file);
        if (!display.count("SOURCE")) throw runtime_error("Missing key 'SOURCE' in " + file);
        
        type = display.at("TYPE");
        sourceLabel = display.at("SOURCE"); 

        try { 
            refreshRate = stoi(display.at("REFRESH")); 
        } catch (const invalid_argument&) {
             throw runtime_error("Non-numeric value for 'REFRESH' in " + file); 
        }

        // ==== Optional fields ====
        if(display.count("LABEL")) { label = display.at("LABEL"); }
       
        if (display.count("PRIORITY")) {
            try { setPriority(stoi(display.at("PRIORITY")));
            } catch (const invalid_argument&) {
                cerr << " Warning: invalid 'PRIORITY' value, ignored in " << file << endl;
            } 
        }
    } catch(const runtime_error& e){
        cerr << "Error while loading Display (" << file << "): " << e.what() << endl;
    } catch(const exception& e) {
        cerr << "Unexpected error while loading Display (" << file << "): " << e.what() << endl;
    }

}
