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
        void updateModel();
        
        CecMqttClientProperties properties;
        CecMqttClientModel *model;
        CEC::ICECAdapter *adapter;
        CEC::ICECCallbacks *callbacks;
        CEC::libcec_configuration *config;
        std::mutex adapterMutex;


        void sourceActivatedHandler(const CEC::cec_logical_address logicalAddress, const uint8_t bActivated);
        
        static void static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated);
        static CecClient *getInstance();
        static CecClient *singleton;
};