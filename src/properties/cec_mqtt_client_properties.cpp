#include "cec_mqtt_client_properties.h"
#include "string_property_holder.h"
#include "int_property_holder.h"

CecMqttClientProperties::CecMqttClientProperties() : Properties(){
    //declare defaults
    mqttClientId = "TV_CEC_CLIENT";
    mqttTopicPrefix = "livingRoom/devices/TV";    
}

void CecMqttClientProperties::fillPropertyMap(){
    propertyHolders.push_back(new StringPropertyHolder("MQTT_CLIENT_ID", &mqttClientId));
    propertyHolders.push_back(new StringPropertyHolder("MQTT_TOPIC_PREFIX", &mqttTopicPrefix));
    propertyHolders.push_back(new StringPropertyHolder("MQTT_BROKER_ADRESS", &mqttBrokerAdress, true));
}

const std::string &CecMqttClientProperties::getMqttClientId(){
    return mqttClientId;
}

const std::string &CecMqttClientProperties::getMqttBrokerAdress(){
    return mqttBrokerAdress;
}

const std::string &CecMqttClientProperties::getMqttTopicPrefix(){
    return mqttTopicPrefix;
}
