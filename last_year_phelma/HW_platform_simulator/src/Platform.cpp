#include "Platform.hpp"
#include <algorithm>


Platform::~Platform() {
    for (auto c : components) delete c;
}

// Lookup component by its label
Component* Platform::findByLabel(const string& label) const {
    auto it = labelToComponent.find(label);
    return (it != labelToComponent.end()) ? it->second : nullptr;
}

        // Safe and robust trim function (keeps dots)
auto trim = [](string& s) {
    // Remove leading/trailing whitespace, colons, and newlines — but keep dots
    const string unwanted = " \t\r\n:";
    size_t start = s.find_first_not_of(unwanted);
    size_t end   = s.find_last_not_of(unwanted);

    if (start == string::npos) {
        s.clear();
        return;
    }

    s = s.substr(start, end - start + 1);

    // Remove inline comments if any
    size_t commentPos = s.find("//");
    if (commentPos != string::npos)
        s = s.substr(0, commentPos);

    // Trim again after removing comment
    start = s.find_first_not_of(unwanted);
    end   = s.find_last_not_of(unwanted);
    if (start == string::npos)
        s.clear();
    else
        s = s.substr(start, end - start + 1);
};

/*
// Load components without blanc in platform.txt
void Platform::loadComponents(const string& platformFile) {
    ifstream file(platformFile);
    if (!file) {
        cerr << "Could not open platform file: " << platformFile << endl;
        return;
    }

    string componentPath;
    while (getline(file, componentPath)) {
        if (componentPath.empty()) continue;

        ifstream compFile(componentPath);
        if (!compFile) {
            cerr << "Could not open component file: " << componentPath << endl;
            continue;
        }

        string type, label, programPath, source;
        int cores = 0, freq = 0, size = 0, access = 0, width = 0, refresh = 0;

        string line;
        while (getline(compFile, line)) {
            trim(line);
            if (line.empty()) continue; // skip empty or comment-only lines
            if (line.rfind("TYPE:", 0) == 0) type = line.substr(5);
            else if (line.rfind("LABEL:", 0) == 0) label = line.substr(6);
            else if (line.rfind("CORES:", 0) == 0) cores = stoi(line.substr(6));
            else if (line.rfind("FREQUENCY:", 0) == 0) freq = stoi(line.substr(10));
            else if (line.rfind("PROGRAM:", 0) == 0) programPath = line.substr(8);
            else if (line.rfind("SIZE:", 0) == 0) size = stoi(line.substr(5));
            else if (line.rfind("ACCESS:", 0) == 0) access = stoi(line.substr(7));
            else if (line.rfind("WIDTH:", 0) == 0) width = stoi(line.substr(6));
            else if (line.rfind("REFRESH:", 0) == 0) refresh = stoi(line.substr(8));
            else if (line.rfind("SOURCE:", 0) == 0) source = line.substr(7);
        }

        trim(type); trim(label); trim(source); trim(programPath);

        Component* comp = nullptr;

        if (type == "CPU") {
            Program prog;
            cout << programPath << endl;
            prog.loadFromFile(programPath);
            comp = new CPU(type, label, cores, freq, prog);
        }
        else if (type == "MEMORY" || type == "RAW"  || type == "ROM" ) {
            auto* mem = new Memory(type, label, size, access);
            mem->setSourceLabel(source);
            comp = mem;
        }
        else if (type == "BUS") {
            auto* bus = new Bus(type, label, width);
            bus->setSourceLabel(source);
            comp = bus;
        }
        else if (type == "DISPLAY") {
            auto* disp = new Display(type, label, refresh);
            disp->setSourceLabel(source);
            comp = disp;
        }

        if (comp) {
            components.push_back(comp);
            labelToComponent[label] = comp;
            cout << "Loaded " << type << ": " << label << endl;
        }
    }

    cout << "=== Finished loading " << components.size() << " components ===\n";
}
*/

// Load components with blanc in platform.txt
void Platform::loadComponents(const string& platformFile) {
    ifstream file(platformFile);
   
    if (!file.is_open()) {
        cerr << "Erreur : impossible d'ouvrir " << platformFile << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        string filename = line;

        ifstream f(filename);
        if (!f.is_open()) {
            cerr << "Erreur : impossible d'ouvrir " << filename << endl;
            continue;
        }

        string firstLine;
        getline(f, firstLine);
        f.close();
        
        Component* loader = nullptr;
        Component* comp = nullptr;

        if (firstLine.find("CPU") != string::npos) loader = new CPU();
        else if (firstLine.find("MEMORY") != string::npos 
                || firstLine.find("RWM") != string::npos 
                || firstLine.find("ROM") != string::npos ) loader = new Memory();
        else if (firstLine.find("BUS") != string::npos) loader = new Bus();
        else if (firstLine.find("DISPLAY") != string::npos) loader = new Display();
        else if (firstLine.find("SERIAL") != string::npos) loader = new Serial();
        else if (firstLine.find("PLATFORM") != string::npos) loader = new Platform(); //sous-plateforme !
        else {
            cerr << "Type inconnu dans " << filename << endl;
            continue;
        }

        // Créer le bon composant
        if (auto subPlatform = dynamic_cast<Platform*>(loader)) {
                subPlatform->loadComponents(filename);
                comp = subPlatform; // on garde l’objet tel quel
                cout << "Loaded PLATFORM: " << filename << endl;
        }
        else if (auto cpuLoader = dynamic_cast<CPU*>(loader)) {
            cpuLoader->LoadFileCpu(filename);
            comp = new CPU( "CPU" , cpuLoader->getLabel(), cpuLoader->getFreq(),
                           cpuLoader->getCores(), cpuLoader->getProgram());
            comp->setPriority(cpuLoader->getPriority());

        } else if (auto memLoader = dynamic_cast<Memory*>(loader)) {
            memLoader->LoadFileMem(filename);
            auto m = new Memory( "MEMORY" , memLoader->getLabel(), memLoader->getSize(),
                                memLoader->getAccessTime());
            m->setSourceLabel(memLoader->getSourceLabel());
            comp = m;
            comp->setPriority(memLoader->getPriority());

        } else if (auto busLoader = dynamic_cast<Bus*>(loader)) {
            busLoader->LoadFileBus(filename);
            auto b = new Bus( "BUS" ,busLoader->getLabel(), busLoader->getWidth());
            b->setSourceLabel(busLoader->getSourceLabel());
            comp = b;
            comp->setPriority(busLoader->getPriority());

        } else if (auto displayLoader = dynamic_cast<Display*>(loader)) {
            displayLoader->LoadFileDisplay(filename);

            string lbl = displayLoader->getLabel();
            if (lbl.empty()) {
                static int displayCounter = 1;
                lbl = "Display_" + to_string(displayCounter++);
            }

            auto d = new Display( "DISPLAY", lbl, displayLoader->getRefresh());
            d->setSourceLabel(displayLoader->getSourceLabel());
            comp = d;
            comp->setPriority(displayLoader->getPriority());
        } else if (auto serialLoader = dynamic_cast<Serial*>(loader)) {
            serialLoader->LoadFileSerial(filename);
            auto s = new Serial("SERIAL", serialLoader->getLabel());
            s->setTarget(serialLoader->getTarget());
            s->setSourceLabel(serialLoader->getSourceLabel()); 
            comp = s;
            comp->setPriority(serialLoader->getPriority());
        }

        delete loader;

        if (comp) {
            components.push_back(comp);
            labelToComponent[comp->getLabel()] = comp; 
            cout << "Loaded " << type << ": " << comp->getLabel() << endl;
        }
    }

    file.close();
}



// Bind all components that have sources
void Platform::bindComponents() {
    cout << "\n=== Binding components ===\n";
    for (auto* c : components) {
        if (auto* bus = dynamic_cast<Bus*>(c)) {
            if (!bus->getSourceLabel().empty()) {
                Component* src = findByLabel(bus->getSourceLabel());
                if (src) bus->bindSource(dynamic_cast<IReadable*>(src));
            }
        } else if (auto* mem = dynamic_cast<Memory*>(c)) {
            if (!mem->getSourceLabel().empty()) {
                Component* src = findByLabel(mem->getSourceLabel());
                if (src) mem->bindSource(dynamic_cast<IReadable*>(src));
            }
        } else if (auto* disp = dynamic_cast<Display*>(c)) {
            if (!disp->getSourceLabel().empty()) {
                Component* src = findByLabel(disp->getSourceLabel());
                if (src) disp->bindSource(dynamic_cast<IReadable*>(src));
            }
        } else if (auto ser = dynamic_cast<Serial*>(c)) {
            if (!ser->getSourceLabel().empty()) {
                Component* src = findByLabel(ser->getSourceLabel());
                if (src) ser->bind(dynamic_cast<IDebuggable*>(src));
        }
    }
    cout << "=== Binding complete ===\n";
}
}

// Simulate components one by one
void Platform::simulate(int steps) {
    cout << "\n=== Starting simulation for " << steps << " steps ===\n";

    // Sort components by priority (highest first)
        std::sort(components.begin(), components.end(),
                  [](const Component* a, const Component* b) {
                      return a->getPriority() > b->getPriority();
                  });

        // Print the order after sorting
        std::cout << "Component order by priority:" << std::endl;
        for (const auto* comp : components) {
            std::cout << " - " << comp->getLabel()
                      << " (priority: " << comp->getPriority() << ")" << std::endl;
        }

        std::cout << "----------------------------------" << std::endl;


    for (int i = 1; i <= steps; ++i) {
        cout << "\n\033[1;33m--- Step " << i << " ---\033[0m\n";

        for (auto* c : components) {
            cout << "\033[1;36mSimulating:\033[0m " << c->getLabel() << endl;
                c->simulate();  // simulate each component sequentially

                if (verbose) {
                    if (auto* cpu = dynamic_cast<CPU*>(c)) {
                        cout << "  [DEBUG] CPU simulated.\n"
                                << "  [CPU] " << cpu->getLabel()
                                << " | cores=" << cpu->getCores()
                                << " | ActiveCores=" << cpu->getActiveCore()
                                << " | freq=" << cpu->getFreq()
                                << " | ProgName=" << cpu->getProgName() << "\n";
                                //<< " | last instr=" << cpu->getLastInstruction()
                                //<< " | result=" << cpu->getLastResult() << "\n";
                    } 
                    else if (auto* mem = dynamic_cast<Memory*>(c)) {
                        cout << "  [Memory] " << mem->getLabel()
                                << " | access=" << mem->getAccessTime()
                                << " | stepCounter=" << mem->getStepCounter() << "\n";

                        cout << "  [DEBUG] Memory buffer: ";
                        auto buf = mem->getBuffer();
                        cout << "[ ";
                        for (double v : buf) cout << v << " ";
                        cout << "]\n";
                    } 
                    else if (auto* bus = dynamic_cast<Bus*>(c)) {
                        cout << "  [Bus] " << bus->getLabel()
                                << " | Width=" << bus->getWidth()
                                << " | Access count=" << bus->getAccessCount() << "\n";
                        cout << "  [DEBUG] Bus pending values:\n";
                        bus->printPending();
                        cout << "  [DEBUG] Bus ready values:\n";
                        bus->printReady();
                    } 
                    else if (auto* disp = dynamic_cast<Display*>(c)) {
                        cout << "  [Display] " << disp->getLabel() << " (refresh=" 
                                << disp->getRefresh() << ")\n";
                        cout << "  [DEBUG] Display refreshed if needed.\n";
                    }
            }
            cout << endl;
        }
    }

    cout << "\n=== Simulation complete ===\n";
}


