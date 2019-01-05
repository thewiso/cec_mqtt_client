#include "model_node.h"
#include "utilities.h"

#include "spdlog/spdlog.h"

#include <stdexcept>

ModelNode::ModelNode(const std::string &mqttPathSegment, bool valueNode, const std::string &value){
    this->parent = nullptr;
    this->mqttPathSegment = mqttPathSegment;
    this->valueNode = valueNode;
    this->logger = spdlog::get(Utilities::GENERAL_LOGGER_NAME);
    
    if(valueNode){
        this->value = value;
    }
}
std::string ModelNode::getMqttPath(){
    std::string retVal = "/" + mqttPathSegment;

    if(parent != nullptr){
        retVal.insert(0, parent->getMqttPath());
    }

    return retVal;
}

ModelNode* ModelNode::getParent(){
    return parent;
}

void ModelNode::setParent(ModelNode *parent){
    this->parent = parent;
}

void ModelNode::registerChangeHandler(const OnModelNodeChangeFunction &onModelNodeChange, bool passToChildren){
    modelChangeHandlers.push_back(onModelNodeChange);

    if(passToChildren){
        modelChangeHandlersForChildren.push_back(onModelNodeChange);
        for(ModelNodePointerList::iterator it = children.begin(); it != children.end(); ++it){
            (*it)->registerChangeHandler(onModelNodeChange, true);
        }
    }
}

void ModelNode::registerChangeHandler(const ModelChangeHandlerVector &onModelNodeChanges, bool passToChildren){
    modelChangeHandlers.insert(modelChangeHandlers.end(), onModelNodeChanges.begin(), onModelNodeChanges.end());

    if(passToChildren){
        modelChangeHandlersForChildren.insert(modelChangeHandlersForChildren.end(), onModelNodeChanges.begin(), onModelNodeChanges.end());
        for(ModelNodePointerList::iterator it = children.begin(); it != children.end(); ++it){
            (*it)->registerChangeHandler(onModelNodeChanges, true);
        }
    }
}

void ModelNode::addChild(ModelNode *child){
    child->parent = this;
    children.push_back(child);

    child->registerChangeHandler(modelChangeHandlersForChildren, true);
    child->triggerChange(ModelNodeChangeType::INSERT);
}

void ModelNode::triggerChange(ModelNodeChangeType modelNodeChangeType){
    for(ModelChangeHandlerVector::iterator it = modelChangeHandlers.begin(); it != modelChangeHandlers.end(); ++it){
        (*it)(*this, modelNodeChangeType);
    }
}

bool ModelNode::isValueNode(){
    return valueNode;
}

const std::string &ModelNode::getValue(){
    return value;
}

void ModelNode::setValue(const std::string &value, bool triggerChange){
    //TODO: log whole mqttPath if trace level is set 
    logger.get()->trace("setValue with value '{}' on node '{}'", value, mqttPathSegment);
    if(!valueNode){
       throw std::runtime_error("Illegal operation: Can not set value on non-value node '" + mqttPathSegment + "'");
    }
    
    //a change of the value and the resulting trigger of the changeHandlers should not be interrupted by parallel changes 
    valueSetMutex.lock();
    triggerChange = triggerChange && this->value != value;
    this->value = value;
    
    if(triggerChange){
        this->triggerChange(ModelNodeChangeType::UPDATE);
    }
    valueSetMutex.unlock();
}

