cmake_minimum_required(VERSION 3.1.0)
project(cec_mqtt_client VERSION 0.1.0)

#include(CTest)
#enable_testing()

set(SRCDIR ${PROJECT_SOURCE_DIR}/src)
set(SRCFILES 
    ${SRCDIR}/main.cpp 
    ${SRCDIR}/utilities.cpp
    ${SRCDIR}/mqtt_client.cpp
    ${SRCDIR}/mqtt_client_callback.cpp
    ${SRCDIR}/cec_client.cpp

    ${SRCDIR}/properties/properties.cpp
    ${SRCDIR}/properties/cec_mqtt_client_properties.cpp
    ${SRCDIR}/properties/property_holder.cpp
    ${SRCDIR}/properties/string_property_holder.cpp
    ${SRCDIR}/properties/int_property_holder.cpp
    
    ${SRCDIR}/model/cec_mqtt_client_model.cpp
    ${SRCDIR}/model/model_node.cpp
    ${SRCDIR}/model/general_model.cpp
    ${SRCDIR}/model/device_model.cpp
    
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#executatble definition
add_executable(cec_mqtt_client ${SRCFILES})

#Custom paths for libraries
if(NOT DEFINED INCLUDE_BCM_HOST_DIR)
    set(INCLUDE_BCM_HOST_DIR /opt/vc/include/)
endif(NOT DEFINED INCLUDE_BCM_HOST_DIR)
#TODO: change the path so that simly cloning SPDLOG in log folder is enough
if(NOT DEFINED INCLUDE_SPDLOG_DIR)
    set(INCLUDE_SPDLOG_DIR lib)
endif(NOT DEFINED INCLUDE_SPDLOG_DIR)

#include directories
target_include_directories(cec_mqtt_client 
    PUBLIC include
    PUBLIC include/properties
    PUBLIC include/model
    PUBLIC ${INCLUDE_BCM_HOST_DIR}      #BCM_HOST
    PUBLIC ${INCLUDE_SPDLOG_DIR}        #SPDLOG
)

#Custom paths for libraries
if(NOT DEFINED LIB_BCM_HOST_DIR)
    set(LIB_BCM_HOST_DIR /opt/vc/lib)
endif(NOT DEFINED LIB_BCM_HOST_DIR)

#use e.g. -DPAHO_MQTT_C_DIR=/foo/bar as command parameter to specify a custom directory for cmake to search for the library

#libraries
find_library(LIB_PAHO_MQTT_C    paho-mqtt3a     PATHS ${LIB_PAHO_MQTT_C_DIR})
find_library(LIB_PAHO_MQTT_CPP  paho-mqttpp3    PATHS ${LIB_PAHO_MQTT_CPP_DIR})
find_library(LIB_LIB_CEC        cec             PATHS ${LIB_LIB_CEC_DIR})
find_library(LIB_BCM_HOST       bcm_host        PATHS ${LIB_BCM_HOST_DIR})

message(STATUS "Paho MQTT C found at    ${LIB_PAHO_MQTT_C}")
message(STATUS "Paho MQTT C++ found at  ${LIB_PAHO_MQTT_CPP}")
message(STATUS "LibCEC found at         ${LIB_LIB_CEC}")
message(STATUS "BCM_HOST found at       ${LIB_BCM_HOST}")

target_link_libraries(cec_mqtt_client 
    ${LIB_PAHO_MQTT_C} 
    ${LIB_PAHO_MQTT_CPP} 
    ${LIB_LIB_CEC} 
    ${LIB_BCM_HOST}
    dl
)
#TODO: can dl be found everywhere?


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

include(CPack)
