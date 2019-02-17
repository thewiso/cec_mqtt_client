#include "string_property_holder.h"

StringPropertyHolder::StringPropertyHolder(const std::string &name, std::shared_ptr<std::string> value, bool mandatory): PropertyHolder(name, mandatory){
    this->value = value;
}

void StringPropertyHolder::setValue(const std::string &valueString){
    *value = valueString;
    valueGiven = true;
}
