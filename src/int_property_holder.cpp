#include "int_property_holder.h"

IntPropertyHolder::IntPropertyHolder(std::string name, int *value, bool mandatory): GenericPropertyHolder(name, value, mandatory){

}
        
int IntPropertyHolder::parseValue(std::string valueString){
    return std::stoi(valueString);
}