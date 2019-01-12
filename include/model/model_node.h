#pragma once

#include <vector>
#include <map>
#include <functional>
#include <string>
#include <mutex>
#include <memory>

#include "spdlog/logger.h"


enum ModelNodeChangeType{
    INSERT,
    UPDATE
};

class ModelNode{
    typedef std::function<void(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType)> OnModelNodeChangeFunction;
    typedef std::vector<OnModelNodeChangeFunction> ModelChangeHandlerVector;
    typedef std::vector<ModelNode*> ModelNodePointerList;

     //TODO: destructor in ModelNode for children
    public:
        ModelNode(const std::string &mqttPathSegment, bool valueNode = false, const std::string &value = "");
        std::string getMqttPath();
        ModelNode *getParent();
        void setParent(ModelNode *parent);
        void registerChangeHandler(const OnModelNodeChangeFunction &onModelNodeChange, bool passToChildren = false);
        void registerChangeHandler(const ModelChangeHandlerVector &onModelNodeChanges, bool passToChildren = false);
        void addChild(ModelNode *child, bool triggerInsertChange = false);
        const std::string &getValue();
        void setValue(const std::string &value, bool triggerChange=true);
        bool isValueNode();

    protected:
        void triggerChange(ModelNodeChangeType modelNodeChangeType);
        std::shared_ptr<spdlog::logger> logger;

    private:
        std::string mqttPathSegment;
        ModelNode *parent;
        ModelNodePointerList children;
        std::string value;
        std::mutex valueSetMutex;
        bool valueNode;
        //using 2 lists instead of a map because functions cannot be compared
        ModelChangeHandlerVector modelChangeHandlers;
        ModelChangeHandlerVector modelChangeHandlersForChildren;
        

        
};

typedef std::function<void(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType)> OnModelNodeChangeFunction;
typedef std::vector<OnModelNodeChangeFunction> ModelChangeHandlerVector;