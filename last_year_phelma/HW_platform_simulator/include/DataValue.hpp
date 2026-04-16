#ifndef DATAVALUE_HPP
#define DATAVALUE_HPP

using namespace std; 

class DataValue {
private:
    double value;
    bool valid;
public:
    DataValue(double v = 0.0, bool isValid = false)
        : value(v), valid(isValid) {}
    bool isValid() const { return valid; }
    double getValue() const { return value; }
};

#endif // DATAVALUE_HPP