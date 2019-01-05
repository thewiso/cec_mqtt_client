#pragma once

#include "cec_mqtt_client_properties.h"
#include "cec_mqtt_client_model.h"

#include "spdlog/logger.h"
#include <libcec/cec.h>

#include <mutex>
#include <memory>

//The class is using a global singleton to make it available for function callbacks from libCEC
class CecClient
{
    public:
        ~CecClient();
        void init();

        static CecClient *getInstance(const CecMqttClientProperties &properties, CecMqttClientModel *model, const std::shared_ptr<spdlog::logger> &logger);

    private:
        CecClient(const CecMqttClientProperties &properties, CecMqttClientModel *model, const std::shared_ptr<spdlog::logger> &logger);
        void updateGeneralModel();
        void updateDeviceModel();
        
        CecMqttClientProperties properties;
        CecMqttClientModel *model;
        CEC::ICECAdapter *adapter;
        CEC::ICECCallbacks *callbacks;
        CEC::libcec_configuration *config;
        std::mutex adapterMutex;
        std::shared_ptr<spdlog::logger> logger;
        
        static void static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated);
        static void static_commandReceivedHandler(void* UNUSED, const CEC::cec_command* command);
        static void static_alertHandler(void* UNUSED, const CEC::libcec_alert alert, const CEC::libcec_parameter param);
        
        static CecClient *getInstance();
        static CecClient *singleton;

        static const CEC::cec_opcode RELEVANT_OPCODES[];

};