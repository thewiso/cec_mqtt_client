#include "cec_client.h"
#include "utilities.h"

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

CecClient::CecClient(const CecMqttClientProperties &properties, CecMqttClientModel *model, const std::shared_ptr<spdlog::logger> &logger){
    this->properties = properties;
    this->model = model;
    this->adapter = nullptr;
    this->callbacks = new CEC::ICECCallbacks();
    this->config = new CEC::libcec_configuration();
    this->logger = logger;
    //TODO: add model listener

}

CecClient::~CecClient(){
    cout << "foo";
    if(adapter != nullptr){
        adapter->Close();
        UnloadLibCec(adapter);
    }
}

CecClient *CecClient::getInstance(){
    //private method, so it can only be called from the callback handlers when the instance is already created
    return singleton;
}

CecClient *CecClient::getInstance(const CecMqttClientProperties &properties, CecMqttClientModel *model, const std::shared_ptr<spdlog::logger> &logger){
    //no need for thread safety, this should be called at program start in the good old main method
    if(singleton == nullptr){
        singleton = new CecClient(properties, model, logger);
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
        


void CecClient::init(){
    bcm_host_init();

    callbacks->Clear();
    callbacks->sourceActivated = &static_sourceActivatedHandler;
    callbacks->commandReceived = &static_commandReceivedHandler;
    callbacks->alert = &static_alertHandler;

    config->Clear();    
    config->clientVersion = CEC::LIBCEC_VERSION_CURRENT;
    config->bActivateSource = 0;
    config->callbacks = callbacks;
    //TODO: logical device from configuration file
    config->deviceTypes.Add(CEC::CEC_DEVICE_TYPE_PLAYBACK_DEVICE);

    //TODO: devicename from configuration file
    const std::string devicename("CECExample");
    devicename.copy(config->strDeviceName, std::min(devicename.size(),13u) );
    
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
    updateGeneralModel();
    updateDeviceModel();
}

void CecClient::updateGeneralModel(){
    adapterMutex.lock();
    CEC::cec_logical_address activeSource = adapter->GetActiveSource();
    adapterMutex.unlock();

    model->getGeneralModel()->getActiveSourceLogicalAddress()->setValue(Utilities::CEC_LOGICAL_ADRESS_2_STRING_LITERAL.at(activeSource));

    //TODO: updates after commands should be triggered (name changed -> configuration changed?)
}

void CecClient::updateDeviceModel(){
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
            //set activity and power state of other already created, now inactive devices
            std::vector<DeviceModel*>::iterator iterator = std::find_if(deviceModels.begin(), deviceModels.end(), [currentAddressString] (DeviceModel *deviceModel) { return deviceModel->getLogicalAddress()->getValue() == currentAddressString; } ); 
            if(iterator != deviceModels.end()){
                (*iterator)->getIsActive()->setValue(Utilities::FALSE_STRING_LITERAL);
                (*iterator)->getPowerStatus()->setValue(Utilities::CEC_POWER_STATUS_2_STRING_LITERAL.at(CEC::cec_power_status::CEC_POWER_STATUS_UNKNOWN));
            }

        }
    } 
}