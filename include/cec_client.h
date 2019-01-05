#pragma once

#include "cec_mqtt_client_properties.h"
#include "cec_mqtt_client_model.h"

#include <libcec/cec.h>
#include <mutex>

//The class is using a global singleton to make it available for function callbacks from libCEC
class CecClient
{
    public:
        ~CecClient();
        void init();

        static CecClient *getInstance(CecMqttClientProperties properties, CecMqttClientModel &model);

    private:
        CecClient(CecMqttClientProperties properties, CecMqttClientModel &model);
        void updateGeneralModel();
        void updateDeviceModel();
        
        CecMqttClientProperties properties;
        CecMqttClientModel *model;
        CEC::ICECAdapter *adapter;
        CEC::ICECCallbacks *callbacks;
        CEC::libcec_configuration *config;
        std::mutex adapterMutex;
        
        static void static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated);
        static void static_commandReceivedHandler(void* UNUSED, const CEC::cec_command* command);
        static void static_alertHandler(void* UNUSED, const CEC::libcec_alert alert, const CEC::libcec_parameter param);
        
        static CecClient *getInstance();
        static CecClient *singleton;

        static const CEC::cec_opcode RELEVANT_OPCODES[];

};