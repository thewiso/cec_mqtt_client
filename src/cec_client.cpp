#include "cec_client.h"
#include "utilities.h"
#include "spdlog/spdlog.h"

#include <algorithm> 
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <iostream>
// cecloader.h uses std::cout _without_ including iosfwd or iostream
// Furthermore is uses cout and not std::cout
using std::cout;
using std::endl;

#include <libcec/cecloader.h>
#include "bcm_host.h"

CecClient *CecClient::singleton = nullptr;
//opcodes from cec_commands that should trigger a model update
const CEC::cec_opcode CecClient::RELEVANT_OPCODES[] = {
    CEC::cec_opcode::CEC_OPCODE_ACTIVE_SOURCE,      //is sent if TV is switched ON and
                                                    //is sent if external Device is switchted ON/OFF
    CEC::cec_opcode::CEC_OPCODE_STANDBY             //is sent if TV is switched OFF
};

CecClient::CecClient(const CecMqttClientProperties &properties, CecMqttClientModel *model){
    this->properties = properties;
    this->model = model;
    this->adapter = nullptr;
    this->callbacks = new CEC::ICECCallbacks();
    this->config = new CEC::libcec_configuration();
    this->logger = spdlog::get(Utilities::CEC_LOGGER_NAME);

    model->getGeneralModel()->getClientOSDNameCommand()->registerChangeHandler(std::bind( &CecClient::clientOSDNameCommandNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);
    model->getGeneralModel()->getActiveSourceLogicalAddress()->registerChangeHandler(std::bind( &CecClient::activeSourceLogicalAddressCommandNodeChangeHandler, this, std::placeholders::_1, std::placeholders::_2), true);

    //TODO: add model listener

    bcm_host_init();

    callbacks->Clear();
    callbacks->sourceActivated = &static_sourceActivatedHandler;
    callbacks->commandReceived = &static_commandReceivedHandler;
    callbacks->alert = &static_alertHandler;

    config->Clear();    
    config->clientVersion = CEC::LIBCEC_VERSION_CURRENT;
    config->bActivateSource = 0;
    config->callbacks = callbacks;
    config->deviceTypes.Add(CEC::CEC_DEVICE_TYPE_PLAYBACK_DEVICE);

    //TODO: devicename from configuration file
    const std::string devicename("CECExample");
    devicename.copy(config->strDeviceName, std::min(devicename.size(),13u) );

}

CecClient::~CecClient(){
    if(adapter != nullptr){
        adapter->Close();
        UnloadLibCec(adapter);
    }
}

CecClient *CecClient::getInstance(){
    //private method, so it can only be called from the callback handlers when the instance is already created
    return singleton;
}

CecClient *CecClient::getInstance(const CecMqttClientProperties &properties, CecMqttClientModel *model){
    //no need for thread safety, this should be called at program start in the good old main method
    if(singleton == nullptr){
        singleton = new CecClient(properties, model);
    }
    return singleton;
}

void CecClient::static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated){
    getInstance()->updateGeneralModel();
    //TODO: debug log
}

void CecClient::static_commandReceivedHandler(void* UNUSED, const CEC::cec_command* command){
    //if no poll command
    if(command->opcode_set == 1){
        if (std::find(std::begin(RELEVANT_OPCODES), std::end(RELEVANT_OPCODES), command->opcode) != std::end(RELEVANT_OPCODES)){
            getInstance()->updateDeviceModel();
            getInstance()->updateGeneralModel();
        }
        //TODO: debug log
        // cout << "from " << std::dec << (int)command->initiator << " to " << (int)command->destination << " command: " << std::hex << "0x" << (int)command->opcode << "\n";
    }
    
}

void CecClient::static_alertHandler(void* UNUSED, const CEC::libcec_alert alert, const CEC::libcec_parameter param){
    cout << "alert\n";
        //TODO: alert callbacks usage for connection lost handling!
        //TODO: debug log

}
        


void CecClient::connect(){
    logger.get()->info("Connecting to CEC Hub...");
    
    adapter = LibCecInitialise(config);
    if( !adapter )
    { 
        //TODO: throw exception?
        std::cerr << "Failed loading libcec.so\n"; 
    }
    std::array<CEC::cec_adapter_descriptor, 10> devices;
    int8_t devices_found = adapter->DetectAdapters(devices.data(), devices.size(), nullptr, false /*quickscan*/);
    if( devices_found <= 0)
    {
        std::cerr << "Could not automatically determine the cec adapter devices\n";
        UnloadLibCec(adapter);
        //TODO: throw exception
    }

    if(adapter->Open(devices[0].strComName)){
        //TODO: throw exception
    }

    logger.get()->info("Connecting to CEC Hub succedeed.");
    updateGeneralModel();
    updateDeviceModel();
}

void CecClient::updateGeneralModel(){
    logger.get()->debug("Updating GeneralModel");

    adapterMutex.lock();
    CEC::cec_logical_address activeSource = adapter->GetActiveSource();
    adapterMutex.unlock();

    model->getGeneralModel()->getActiveSourceLogicalAddress()->setValue(Utilities::CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(activeSource));

    //TODO: updates after commands should be triggered (name changed -> configuration changed?)
}

void CecClient::updateDeviceModel(){
    logger.get()->debug("Updating DeviceModel");
    //TODO: log debug duration of method
    //TODO: when to use adapter->RescanActiveDevices ?
    adapterMutex.lock();
    CEC::cec_logical_addresses activeDevices = adapter->GetActiveDevices();
    adapterMutex.unlock();

    int activeDevicesLength = (sizeof(activeDevices.addresses)/sizeof(*activeDevices.addresses));
    std::vector<DeviceModel*> deviceModels = model->getDeviceModels();

    for(int i=0; i<activeDevicesLength; i++){
        CEC::cec_logical_address currentAddress = (CEC::cec_logical_address)i;
        std::string currentAddressString = Utilities::CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(currentAddress);
        
        if(activeDevices[i]){
            
            adapterMutex.lock();
                std::string odsName = adapter->GetDeviceOSDName(currentAddress);
                CEC::cec_power_status powerStatus = adapter->GetDevicePowerStatus(currentAddress);
            adapterMutex.unlock();

            std::pair<DeviceModel*, bool> activeDevicePair = model->getOrCreateDeviceModel(currentAddressString);
            DeviceModel *activeDevice = activeDevicePair.first;
            activeDevice->getName()->setValue(odsName);
            activeDevice->getIsActive()->setValue(Utilities::TRUE_STRING_LITERAL);
            activeDevice->getPowerStatus()->setValue(Utilities::CEC_POWER_STATUS_2_STRING_LITERAL.at(powerStatus));

            if(activeDevicePair.second){
                //TODO: register change handler on new model
            }
        }else{
            //set activity and power state of other, already created and now inactive devices
            std::vector<DeviceModel*>::iterator iterator = std::find_if(deviceModels.begin(), deviceModels.end(), [currentAddressString] (DeviceModel *deviceModel) { return deviceModel->getLogicalAddress()->getValue() == currentAddressString; } ); 
            if(iterator != deviceModels.end()){
                (*iterator)->getIsActive()->setValue(Utilities::FALSE_STRING_LITERAL);
                (*iterator)->getPowerStatus()->setValue(Utilities::CEC_POWER_STATUS_2_STRING_LITERAL.at(CEC::cec_power_status::CEC_POWER_STATUS_UNKNOWN));
            }

        }
    } 
}

void CecClient::clientOSDNameCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType){
    std::string deviceName = modelNode.getValue();
    if(!deviceName.empty()){
        (*logger).debug("Changing client OSD name to {}...", deviceName);
        deviceName.copy(config->strDeviceName, std::min(deviceName.size(),13u));
        
        adapterMutex.lock();
        bool success = adapter->SetConfiguration(config);
        adapterMutex.unlock();

        if(success){
            (*logger).debug("Changing client OSD name to {} succedeed.", deviceName);
        }else{
            //TODO: log error with all attributes of config
        }
    }
}

void CecClient::activeSourceLogicalAddressCommandNodeChangeHandler(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType){
    (*logger).debug("Changing active device to {}", modelNode.getValue());
}