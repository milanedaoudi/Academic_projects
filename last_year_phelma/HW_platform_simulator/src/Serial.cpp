#include <iostream>
#include "Serial.hpp"

/*-----------------------------------------------------------------
* Serial simulation step.
 * - Checks if a source component is bound; if not, does nothing.
 * - Opens the target file in append mode.
 * - Writes all lines from the source's debug buffer to the file.
 * - Clears the source debug buffer after writing.
 * - Closes the file.
-----------------------------------------------------------------*/
void Serial::simulate() {
            if (!source) return;
            
            ofstream out(targetFile, ios::app);
            if (!out.is_open()) return;

            auto buffer = source->getDebugBuffer();  // Get all debug output from source
            for (const auto& line : buffer) {
                out << line << endl;            // Write each line to file
            }
            source->clearDebugBuffer();
            out.close();
}


/*-----------------------------------------------------------------
LoadFileSerial: loads SERIAL configuration from a .txt file
Required fields : TYPE, LABEL, SOURCE, TARGET
Optional fields : none
-----------------------------------------------------------------*/
void Serial::LoadFileSerial(const string& file) {
    try {
        auto serial = LoadVar::parseFile(file);

        // Required fields
        if (!serial.count("TYPE")) throw runtime_error("Missing key 'TYPE'");
        if (!serial.count("LABEL")) throw runtime_error("Missing key 'LABEL'");
        if (!serial.count("SOURCE")) throw runtime_error("Missing key 'SOURCE'");
        if (!serial.count("TARGET")) throw runtime_error("Missing key 'TARGET'");

        type = serial.at("TYPE");
        if (type != "SERIAL") {
            throw runtime_error("TYPE must be 'SERIAL'");
        }

        label = serial.at("LABEL");
        sourceLabel = serial.at("SOURCE");
        targetFile = serial.at("TARGET");

    } catch (const runtime_error& e) {
        cerr << "Missing required key in SERIAL file '" << file << "': " << e.what() << endl;
    } catch (const exception& e) {
        cerr << "Unexpected error while loading SERIAL file '" << file << "': " << e.what() << endl;
    }
}
