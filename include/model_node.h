#pragma once

#include <vector>
#include <map>
#include <functional>
#include <string>

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
        ModelNode(std::string mqttPathSegment, bool valueNode = false, std::string value = "");
        std::string getMqttPath();
        ModelNode *getParent();
        void setParent(ModelNode *parent);
        void registerChangeHandler(OnModelNodeChangeFunction onModelNodeChange, bool passToChildren = false);
        void registerChangeHandler(ModelChangeHandlerVector onModelNodeChanges, bool passToChildren = false);
        void addChild(ModelNode *child);
        ModelNode *findChildByMqttPathSegment(std::string mqttPathSegment);
        std::string getValue();
        void setValue(std::string value, bool triggerChange=true);
        bool isValueNode();

    protected:
        void triggerChange(ModelNodeChangeType modelNodeChangeType);

    private:
        std::string mqttPathSegment;
        ModelNode *parent;
        ModelNodePointerList children;
        std::string value;
        bool valueNode;
        //using 2 lists instead of a map because functions cannot be compared
        ModelChangeHandlerVector modelChangeHandlers;
        ModelChangeHandlerVector modelChangeHandlersForChildren;
        
};

typedef std::function<void(ModelNode &modelNode, ModelNodeChangeType modelNodeChangeType)> OnModelNodeChangeFunction;
typedef std::vector<OnModelNodeChangeFunction> ModelChangeHandlerVector;