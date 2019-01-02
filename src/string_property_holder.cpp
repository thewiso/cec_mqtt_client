#include "string_property_holder.h"

StringPropertyHolder::StringPropertyHolder(std::string name, std::string *value, bool mandatory): PropertyHolder(name, mandatory){
    this->value = value;
}

void StringPropertyHolder::setValue(std::string valueString){
    *value = valueString;
    valueGiven = true;
}
