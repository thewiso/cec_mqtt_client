#include "property_holder.h"

PropertyHolder::PropertyHolder(const std::string &name, bool mandatory){
    this->name = name;
    this->mandatory = mandatory;
    this->valueGiven = false;
}

bool PropertyHolder::isValueGiven(){
    return valueGiven;
}
 
bool PropertyHolder::isMandatory(){
    return mandatory;
}

const std::string &PropertyHolder::getName(){
    return name;
}