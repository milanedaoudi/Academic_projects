#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <string>
#include "LoadUtils.hpp"

using namespace std;

enum PRIORITY{ LOW=0 , MEDIUM=1 , HIGH=2};


class Component{

protected:
    string type;
    string label;

    //============ EXTRA PRORITY==================
    int priority = 0;
    //============ END EXTRA ======================

public:

    Component() = default;
    Component(const string& typ  , const string& lbl) :  type(typ) ,label(lbl){}
    virtual void simulate() = 0;
    virtual ~Component() = default;

    virtual string getLabel() const { return label; }
    string getType() const { return type; }


    //============ EXTRA PRORITY==================
    void setPriority(int p) { priority = p; }
    int getPriority() const { return priority; }
    //============ END EXTRA ======================
};
#endif