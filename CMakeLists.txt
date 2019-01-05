cmake_minimum_required(VERSION 3.1.0)
project(cec_mqtt_client VERSION 0.1.0)

#include(CTest)
#enable_testing()

set(SRCDIR ${PROJECT_SOURCE_DIR}/src)
set(SRCFILES 
    ${SRCDIR}/main.cpp 
    ${SRCDIR}/properties.cpp
    ${SRCDIR}/cec_mqtt_client_properties.cpp
    ${SRCDIR}/mqtt_client.cpp
    ${SRCDIR}/cec_client.cpp
    ${SRCDIR}/property_holder.cpp
    ${SRCDIR}/string_property_holder.cpp
    ${SRCDIR}/int_property_holder.cpp
    ${SRCDIR}/cec_mqtt_client_model.cpp
    ${SRCDIR}/model_node.cpp
    ${SRCDIR}/general_model.cpp
    ${SRCDIR}/device_model.cpp
    ${SRCDIR}/utilities.cpp
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#executatble definition
add_executable(cec_mqtt_client ${SRCFILES})

#include directories

target_include_directories(cec_mqtt_client 
    PUBLIC include
    PUBLIC /opt/vc/include/
)
#TODO: make include directories settable via command parameter 

#Custom paths for libraries
if(NOT DEFINED BCM_HOST_DIR)
    set(BCM_HOST_DIR /opt/vc/lib)
endif(NOT DEFINED BCM_HOST_DIR)

#use e.g. -DPAHO_MQTT_C_DIR=/foo/bar as command parameter to specify a custom directory for cmake to search for the library

#libraries
find_library(PAHO_MQTT_C    paho-mqtt3a     PATHS ${PAHO_MQTT_C_DIR})
find_library(PAHO_MQTT_CPP  paho-mqttpp3    PATHS ${PAHO_MQTT_CPP_DIR})
find_library(LIB_CEC        cec             PATHS ${LIB_CEC_DIR})
find_library(BCM_HOST       bcm_host        PATHS ${BCM_HOST_DIR})

message(STATUS "Paho MQTT C found at    ${PAHO_MQTT_C}")
message(STATUS "Paho MQTT C++ found at  ${PAHO_MQTT_CPP}")
message(STATUS "LibCEC found at         ${LIB_CEC}")
message(STATUS "BCM_HOST found at       ${BCM_HOST}")

target_link_libraries(cec_mqtt_client 
    ${PAHO_MQTT_C} 
    ${PAHO_MQTT_CPP} 
    ${LIB_CEC} 
    ${BCM_HOST}
    dl
)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

include(CPack)
