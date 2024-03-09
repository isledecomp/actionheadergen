#include "interleafhandler.h"

typedef std::map<size_t, std::string> ActionMap;

si::Interleaf::Error InterleafHandler::ReadInterleaf(char *p_filePath)
{
    // this is basically a wrapper function for libweaver's
    // Read() so we can set private member variables
    return m_inlf.Read(p_filePath);
}

bool InterleafHandler::SortActionsIntoVector()
{
    // if there's no actions in this Interleaf, exit
    if (!m_inlf.HasChildren()) {
        return false;
    }

    // get the amount of actions in this Interleaf
    m_actionCount = m_inlf.GetChildCount();

    // prepare our vector and map for use
    m_actionVector = new std::vector<std::string>;

    std::map<size_t, std::string> actionMap;

    // generate the map using recursion
    for (size_t i = 0; i < m_actionCount; i++) {
        AddActionsToMap(actionMap, m_inlf.GetChildAt(i));
    }

    // finished, so let's construct the ordered vector 
    // by sorting all of the actions in the map by index
    size_t index = 0;
    for (ActionMap::const_iterator it = actionMap.begin(); it != actionMap.end(); it++) {
        while (index < it->first) {
            // we add padding to keep the vector in order
            m_actionVector->push_back("_ahg_pad_");
            index++;
        }
        // we're back in order, so we can 
        // push this action back to the vector
        m_actionVector->push_back(it->second);
        index++;
    }

    // success
    return true;
}

void InterleafHandler::AddActionsToMap(std::map<size_t, std::string> &p_actionMap, si::Core *p_object)
{
    // Core doesn't provide some of the data we need, so we get the
    // action as Object, so then we can retrieve the name of the action
    if (si::Object *actionAsObject = dynamic_cast<si::Object *>(p_object)) {
        // check for sub-objects
        if (actionAsObject->HasChildren()) {
            for (si::Core::Children::const_iterator it = actionAsObject->GetChildren().cbegin(); it != actionAsObject->GetChildren().cend(); it++) {
                // we have sub-objects, so we need to account for these
                si::Object *subObject = static_cast<si::Object *>(*it);
                if (subObject->HasChildren()) {
                    // if this sub-object contains more sub-objects, recurse
                    AddActionsToMap(p_actionMap, subObject);
                }
                // push the name of each action and its index into the map
                p_actionMap[subObject->id()] = subObject->name();
            }
        }
        else p_actionMap[actionAsObject->id()] = actionAsObject->name(); 
    }
}

