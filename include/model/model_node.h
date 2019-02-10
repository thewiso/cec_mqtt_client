#pragma once

#include <vector>
#include <map>
#include <functional>
#include <string>
#include <mutex>
#include <memory>

#include "spdlog/logger.h"


enum ModelNodeChangeEventType{
    INSERT,
    UPDATE
};

class ModelNode{
    typedef std::function<void(ModelNode &modelNode, ModelNodeChangeEventType ModelNodeChangeEventType)> OnModelNodeChangeFunction;
    typedef std::vector<OnModelNodeChangeFunction> ModelChangeHandlerVector;
    typedef std::vector<ModelNode*> ModelNodePointerList;

     //TODO: destructor in ModelNode for children
    public:
        ModelNode(const std::string &mqttPathSegment, bool valueNode = false, const std::string &value = std::string(), bool triggerInsertChangeEvent = false, bool subscriptionNode = false);
        virtual ~ModelNode();
        std::string getMqttPath();
        ModelNode *getParent();
        void setParent(ModelNode *parent);
        void registerChangeHandler(const OnModelNodeChangeFunction &onModelNodeChange, bool passToChildren = false);
        void registerChangeHandler(const ModelChangeHandlerVector &onModelNodeChanges, bool passToChildren = false);
        void addChild(ModelNode *child);
        const std::vector<ModelNode*> &getChildren();
        const std::string &getValue();
        void setValue(const std::string &value, bool triggerChange=true);
        bool isValueNode();
        void pauseChangeEvents();
        void resumeChangeEvents();
        bool isSubscriptionNode();

    protected:
        void triggerChangeEvent(ModelNodeChangeEventType modelNodeChangeEventType);
        std::shared_ptr<spdlog::logger> logger;
        ModelNodePointerList children;
        ModelNode *parent;

    private:
        std::string mqttPathSegment;
        std::string value;
        std::mutex valueSetMutex;
        bool valueNode;
        bool insertChangeEventStored;
        bool updateChangeEventStored;
        bool changeEventsPaused;
        bool subscriptionNode;
        //using 2 lists instead of a map because functions cannot be compared
        ModelChangeHandlerVector modelChangeHandlers;
        ModelChangeHandlerVector modelChangeHandlersForChildren;
        

        
};

typedef std::function<void(ModelNode &modelNode, ModelNodeChangeEventType ModelNodeChangeEventType)> OnModelNodeChangeFunction;
typedef std::vector<OnModelNodeChangeFunction> ModelChangeHandlerVector;