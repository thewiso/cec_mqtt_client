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

class ModelNode: public std::enable_shared_from_this<ModelNode>{
    typedef std::function<void(ModelNode &modelNode, ModelNodeChangeEventType ModelNodeChangeEventType)> OnModelNodeChangeFunction;
    typedef std::vector<OnModelNodeChangeFunction> ModelChangeHandlerVector;
    typedef std::vector<std::shared_ptr<ModelNode>> ModelNodePointerList;
    typedef std::shared_ptr<ModelNode> ModelNodePointer;
    
    public:
        ModelNode(const std::string &mqttPathSegment, bool valueNode = false, const std::string &value = std::string(), bool triggerInsertChangeEvent = false, bool subscriptionNode = false);
        virtual ~ModelNode();
        //init function is needed, because of the call of shared_from_this() in setParent.
        //if a derived class adds children to itself in the c'tor, shared_from_this() will throw a bad weak ptr exception, because there exists no other shared ptr of itself while the object is constructed.
        virtual void init();
        const std::string getMqttPath();
        ModelNode &getParent();
        void setParent(std::shared_ptr<ModelNode> parent);
        void registerChangeHandler(const OnModelNodeChangeFunction &onModelNodeChange, bool passToChildren = false);
        void registerChangeHandler(const ModelChangeHandlerVector &onModelNodeChanges, bool passToChildren = false);
        void addChild(std::shared_ptr<ModelNode> child);
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
        ModelNodePointer parent;

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