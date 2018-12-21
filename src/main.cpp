#include <iostream>
#include "cec_mqtt_client_properties.h"

int main(int argc, char* argv[])
{
    CecMqttClientProperties properties;
    properties.readFile("properties.txt");
    std::cout << properties.getClientId();

    return 0;
}