#include "bool_property_holder.h"
#include "property_exception.h"
#include "utilities.h"

#include <ios>
#include <sstream>

BoolPropertyHolder::BoolPropertyHolder(const std::string &name, bool *value, bool mandatory): GenericPropertyHolder(name, value, mandatory){

}
        
bool BoolPropertyHolder::parseValue(const std::string &valueString){
    std::string boolString = valueString;
    Utilities::ToLower(boolString);
    std::istringstream stream(boolString);

    bool retVal;
    stream >> std::boolalpha >> retVal;
    return retVal;   
}