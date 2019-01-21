#include "bool_property_holder.h"
#include "property_exception.h"

#include <algorithm>
#include <ios>
#include <sstream>
#include <cctype>

BoolPropertyHolder::BoolPropertyHolder(const std::string &name, bool *value, bool mandatory): GenericPropertyHolder(name, value, mandatory){

}
        
bool BoolPropertyHolder::parseValue(const std::string &valueString){
    std::string boolString = valueString;
    std::transform(boolString.begin(), boolString.end(), boolString.begin(), ::tolower);
    std::istringstream stream(valueString);

    bool retVal;
    stream >> std::boolalpha >> retVal;
    return retVal;   
}