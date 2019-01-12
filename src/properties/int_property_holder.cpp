#include "int_property_holder.h"
#include "property_exception.h"
#include <stdexcept> 

IntPropertyHolder::IntPropertyHolder(const std::string &name, int *value, bool mandatory): GenericPropertyHolder(name, value, mandatory){

}
        
int IntPropertyHolder::parseValue(const std::string &valueString){
    try{
        return std::stoi(valueString);
    }catch(const std::invalid_argument &e){
        throw new PropertyException("Could not parse " + valueString + " to int");
    }
}