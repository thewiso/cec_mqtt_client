#include "log_level_property_holder.h"
#include "property_exception.h"
#include "utilities.h"

#include <map>
#include <stdexcept>
        
LogLevelPropertyHolder::LogLevelPropertyHolder(const std::string &name, std::shared_ptr<SpdLogLevel> value, bool mandatory): GenericPropertyHolder(name, value, mandatory){
}
        
SpdLogLevel LogLevelPropertyHolder::parseValue(const std::string &valueString){
    std::string logLevelString = valueString;
    Utilities::ToLower(logLevelString);

    auto iterator = STRING_LITERAL_2_SPD_LOG_LEVEL.find(logLevelString);
    if(iterator != STRING_LITERAL_2_SPD_LOG_LEVEL.end()){
        return iterator->second;
    }else{
        throw new PropertyException("Could not parse " + valueString + " to log level");
    }
}


const std::map<std::string, SpdLogLevel> LogLevelPropertyHolder::STRING_LITERAL_2_SPD_LOG_LEVEL = {
    {"trace",       SpdLogLevel::trace      },
    {"debug",       SpdLogLevel::debug      },
    {"info",        SpdLogLevel::info       }, 
    {"warning",     SpdLogLevel::warn       }, 
    {"error",       SpdLogLevel::err        },
    {"crititcal",   SpdLogLevel::critical   },
    {"off",         SpdLogLevel::off        }
};