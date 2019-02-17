#include "model_node.h"
#include "utilities.h"

#include "spdlog/spdlog.h"

#include <stdexcept>
ModelNode::ModelNode(const std::string &mqttPathSegment, bool valueNode, const std::string &value, bool triggerInsertChangeEvent, bool subscriptionNode){
    this->parent = nullptr;
    this->mqttPathSegment = mqttPathSegment;
    this->valueNode = valueNode;
    this->logger = spdlog::get(Utilities::GENERAL_LOGGER_NAME);
    this->insertChangeEventStored = triggerInsertChangeEvent;
    this->updateChangeEventStored = false;
    this->changeEventsPaused = true;
    this->subscriptionNode = subscriptionNode;

    if(valueNode){
        this->value = value;
    }
}

ModelNode::~ModelNode(){
    
}

void ModelNode::init(){
    for(auto it = children.begin(); it != children.end(); ++it){
        (*it)->init();
    }
}

const std::string ModelNode::getMqttPath(){
    std::string retVal = "/" + mqttPathSegment;

    if(parent != nullptr){
        retVal.insert(0, parent->getMqttPath());
    }

    return retVal;
}

ModelNode &ModelNode::getParent(){
    return *parent;
}

void ModelNode::setParent(std::shared_ptr<ModelNode> parent){
    this->parent = parent;
}

void ModelNode::registerChangeHandler(const OnModelNodeChangeFunction &onModelNodeChange, bool passToChildren){
    modelChangeHandlers.push_back(onModelNodeChange);

    if(passToChildren){
        modelChangeHandlersForChildren.push_back(onModelNodeChange);
        for(auto it = children.begin(); it != children.end(); ++it){
            (*it)->registerChangeHandler(onModelNodeChange, true);
        }
    }
}

void ModelNode::registerChangeHandler(const ModelChangeHandlerVector &onModelNodeChanges, bool passToChildren){
    modelChangeHandlers.insert(modelChangeHandlers.end(), onModelNodeChanges.begin(), onModelNodeChanges.end());

    if(passToChildren){
        modelChangeHandlersForChildren.insert(modelChangeHandlersForChildren.end(), onModelNodeChanges.begin(), onModelNodeChanges.end());
        for(auto it = children.begin(); it != children.end(); ++it){
            (*it)->registerChangeHandler(onModelNodeChanges, true);
        }
    }
}

void ModelNode::addChild(std::shared_ptr<ModelNode> child){
    child->setParent(shared_from_this());
    children.push_back(child);

    if(changeEventsPaused){
        child->pauseChangeEvents();
    }
    child->triggerChangeEvent(ModelNodeChangeEventType::INSERT);
    child->registerChangeHandler(modelChangeHandlersForChildren, true);
}

void ModelNode::triggerChangeEvent(ModelNodeChangeEventType modelNodeChangeEventType){
    if(!changeEventsPaused && isValueNode()){
        bool triggerChangeEvent = false;
        switch(modelNodeChangeEventType){
            case ModelNodeChangeEventType::UPDATE:  
                triggerChangeEvent = true;
                updateChangeEventStored = false;
                break;
            case ModelNodeChangeEventType::INSERT: 
                triggerChangeEvent = insertChangeEventStored; 
                insertChangeEventStored = false;
                break;
        }
        if(triggerChangeEvent){
            for(auto it = modelChangeHandlers.begin(); it != modelChangeHandlers.end(); ++it){
                (*it)(*this, modelNodeChangeEventType);
            }
        }
    }else if(modelNodeChangeEventType ==  ModelNodeChangeEventType::UPDATE){
        updateChangeEventStored = true;
    }
}

bool ModelNode::isValueNode(){
    return valueNode;
}

const std::string &ModelNode::getValue(){
    return value;
}

void ModelNode::setValue(const std::string &value, bool triggerChange){
    //whole mqtt path should only be generated if the log level is fitting:
    if(logger->should_log(spdlog::level::level_enum::trace)){
        logger->trace("setValue with value '{}' on node '{}'", value, getMqttPath());
    }
    
    if(!valueNode){
       throw std::logic_error("Illegal operation: Can not set value on non-value node '" + mqttPathSegment + "'");
    }
    
    valueSetMutex.lock();
    triggerChange = triggerChange && this->value != value;
    this->value = value;
    valueSetMutex.unlock();

    if(triggerChange){
        this->triggerChangeEvent(ModelNodeChangeEventType::UPDATE);
    }
}

void ModelNode::pauseChangeEvents(){
    changeEventsPaused = true;
    
    for(auto it = children.begin(); it != children.end(); ++it){
        (*it)->pauseChangeEvents();
    }
}

void ModelNode::resumeChangeEvents(){
    changeEventsPaused = false;
    if(updateChangeEventStored){
        triggerChangeEvent(ModelNodeChangeEventType::UPDATE);
    }if(insertChangeEventStored){
        triggerChangeEvent(ModelNodeChangeEventType::INSERT);
    }    

    for(auto it = children.begin(); it != children.end(); ++it){
        (*it)->resumeChangeEvents();
    }
}

bool ModelNode::isSubscriptionNode(){
    return subscriptionNode;
}