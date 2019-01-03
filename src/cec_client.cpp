#include "cec_client.h"

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

CecClient::CecClient(CecMqttClientProperties properties, CecMqttClientModel &model){
    this->properties = properties;
    this->model = &model;
    this->adapter = nullptr;
    this->callbacks = new CEC::ICECCallbacks();
    this->config = new CEC::libcec_configuration();

}

CecClient::~CecClient(){
    if(adapter != nullptr){
        adapter->Close();
        UnloadLibCec(adapter);
    }
}

CecClient *CecClient::getInstance(){
    //private method, so it can only be called from the static callback handlers when the instance is already created
    return singleton;
}

CecClient *CecClient::getInstance(CecMqttClientProperties properties, CecMqttClientModel &model){
    //no need for thread safety, this should be called at program start in the good old main method
    if(singleton == nullptr){
        singleton = new CecClient(properties, model);
    }
    return singleton;
}

void CecClient::static_sourceActivatedHandler(void* UNUSED, const CEC::cec_logical_address logicalAddress, const uint8_t bActivated){
    getInstance()->sourceActivatedHandler(logicalAddress, bActivated);
}


void CecClient::init(){
    bcm_host_init();

    callbacks->Clear();
    callbacks->sourceActivated = &static_sourceActivatedHandler;
    //TODO: alert callbacks usage for connection lost handling!

    config->Clear();    
    config->clientVersion = CEC::LIBCEC_VERSION_CURRENT;
    config->bActivateSource = 0;
    config->callbacks = callbacks;
    //TODO: device type from configuration file
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
    updateModel();
}

void CecClient::updateModel(){

    // cout << std::to_string(adapter->PowerOnDevices())  << "\n";

//     cout << "Adresses:\n";
    // CEC::cec_logical_addresses logicalAdresses = adapter->GetActiveDevices();
//    int logicalAdressesLength = (sizeof(logicalAdresses.addresses)/sizeof(*logicalAdresses.addresses));
//     for(int i=0; i<logicalAdressesLength;i++){
//         if(logicalAdresses[i]){
//             cout << adapter->GetDeviceOSDName((CEC::cec_logical_address)i) << "\n";
//         }
//     } 
    adapterMutex.lock();
    model->getGeneralModel()->getActiveSourceDeviceId()->setValue(std::to_string((int)adapter->GetActiveSource()));

    adapterMutex.unlock();

}
#include <thread>
void CecClient::sourceActivatedHandler(const CEC::cec_logical_address logicalAddress, const uint8_t bActivated){
    adapterMutex.lock();
    int activeSource = (int)adapter->GetActiveSource();
    adapterMutex.unlock();

    model->getGeneralModel()->getActiveSourceDeviceId()->setValue(std::to_string(activeSource));
}