#include "utilities.h"

const std::string Utilities::TRUE_STRING_LITERAL = "true";
const std::string Utilities::FALSE_STRING_LITERAL = "false";
const std::string Utilities::INVALID_STRING_LITERAL = "-1";

const std::map<CEC::cec_logical_address, std::string> Utilities::CEC_LOGICAL_ADRESS_2_STRING_LITERAL = {
    {CEC::cec_logical_address::CECDEVICE_UNKNOWN         , "UNKNOWN"},
    {CEC::cec_logical_address::CECDEVICE_TV              , "TV"},
    {CEC::cec_logical_address::CECDEVICE_RECORDINGDEVICE1, "RECORDINGDEVICE1"},
    {CEC::cec_logical_address::CECDEVICE_RECORDINGDEVICE2, "RECORDINGDEVICE2"},
    {CEC::cec_logical_address::CECDEVICE_TUNER1          , "TUNER1"},
    {CEC::cec_logical_address::CECDEVICE_PLAYBACKDEVICE1 , "PLAYBACKDEVICE1"},
    {CEC::cec_logical_address::CECDEVICE_AUDIOSYSTEM     , "AUDIOSYSTEM"},
    {CEC::cec_logical_address::CECDEVICE_TUNER2          , "TUNER2"},
    {CEC::cec_logical_address::CECDEVICE_TUNER3          , "TUNER3"},
    {CEC::cec_logical_address::CECDEVICE_PLAYBACKDEVICE2 , "PLAYBACKDEVICE2"},
    {CEC::cec_logical_address::CECDEVICE_RECORDINGDEVICE3, "RECORDINGDEVICE3"},
    {CEC::cec_logical_address::CECDEVICE_TUNER4          , "TUNER4"},
    {CEC::cec_logical_address::CECDEVICE_PLAYBACKDEVICE3 , "PLAYBACKDEVICE3"},
    {CEC::cec_logical_address::CECDEVICE_RESERVED1       , "RESERVED1"},
    {CEC::cec_logical_address::CECDEVICE_RESERVED2       , "RESERVED2"},
    {CEC::cec_logical_address::CECDEVICE_FREEUSE         , "FREEUSE"},
    {CEC::cec_logical_address::CECDEVICE_UNREGISTERED    , "UNREGISTERED"},
    {CEC::cec_logical_address::CECDEVICE_BROADCAST       , "BROADCAST"}
};

const std::map<CEC::cec_power_status, std::string> Utilities::CEC_POWER_STATUS_2_STRING_LITERAL = {
    {CEC::cec_power_status::CEC_POWER_STATUS_ON                         , "ON"},
    {CEC::cec_power_status::CEC_POWER_STATUS_STANDBY                    , "STANDBY"},
    {CEC::cec_power_status::CEC_POWER_STATUS_IN_TRANSITION_STANDBY_TO_ON, "IN_TRANSITION_STANDBY_TO_ON"},
    {CEC::cec_power_status::CEC_POWER_STATUS_IN_TRANSITION_ON_TO_STANDBY, "IN_TRANSITION_ON_TO_STANDBY"},
    {CEC::cec_power_status::CEC_POWER_STATUS_UNKNOWN                    , "UNKNOWN"}
};


int Utilities::parseStringToBool(std::string string){
    //TODO: true, TRUE, 1
    return -1;
}
