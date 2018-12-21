#include "cec_mqtt_client_properties.h"

CecMqttClientProperties::CecMqttClientProperties() : Properties(){
    //declare defaults
    clientId = "TV_CEC_CLIENT";
}

void CecMqttClientProperties::fillPropertyMap(){
    propertyKey2Value["NamedPipePath"] = &clientId;
}

std::string CecMqttClientProperties::getClientId(){
    return clientId;
}