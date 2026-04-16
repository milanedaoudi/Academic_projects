#ifndef REGISTER_HPP  
#define REGISTER_HPP


#include <queue>
#include "DataValue.hpp"

using namespace std;

class Register {
private:
    std::queue<double> values;
public:
    void write(double v) { values.push(v); }
    DataValue read() {
        if (values.empty()) return DataValue(0.0, false);
        double v = values.front(); 
        values.pop();
        return DataValue(v, true);
    }
    bool isEmpty() const { return values.empty(); }
};
#endif