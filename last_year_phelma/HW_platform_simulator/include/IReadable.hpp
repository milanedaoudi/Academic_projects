#ifndef IREADABLE_HPP
#define IREADABLE_HPP


#include "DataValue.hpp"

class IReadable {
public:
    virtual DataValue read() = 0;
    virtual ~IReadable() = default;
};

#endif
