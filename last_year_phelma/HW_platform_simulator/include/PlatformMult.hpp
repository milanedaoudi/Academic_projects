#ifndef PLATEFORMMULT_HPP
#define PLATEFORMMULT_HPP



#include "Platform.hpp"

using namespace std;

class PlatformMult : public Platform {
private:
    string type;
    string label;

    vector<Platform*> subPlatforms;

public:
    PlatformMult() = default;

    ~PlatformMult() {
        for (auto* p : subPlatforms)
            delete p;
    }

    // Load of platform of platform
    void LoadFilePlatformMult(const string& configFile);

    // Simulation of all subplatform
    void simulateVerbose(int steps, bool verbose);

    const vector<Platform*>& getSubPlatforms() const { return subPlatforms; }

};
#endif
