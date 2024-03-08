#include "interleafhandler.h"

typedef std::map<size_t, const char*> ActionMap;

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
    m_actionVector = new std::vector<const char *>;

    std::map<size_t, const char *> actionMap;
    
    // iterate through every action in our action count
    for (size_t i = 0; i < m_actionCount; i++) {
        // get the action as Core initially
        si::Core *actionAsCore = m_inlf.GetChildAt(i);

        // Core doesn't provide some of the data we need, so we get the
        // action as Object, so then we can retrieve the name of the action
        if (si::Object *actionAsObject = dynamic_cast<si::Object *>(actionAsCore)) {
            // check for sub-objects
            for (si::Core::Children::const_iterator it = actionAsObject->GetChildren().cbegin(); it != actionAsObject->GetChildren().cend(); it++) {
                // we have sub-objects, so we need to account for these
                si::Object *subObject = static_cast<si::Object *>(*it);
                actionMap[subObject->id()] = subObject->name().c_str();
            }

            // push the name of the action and the index into the map
            actionMap[i] = actionAsObject->name().c_str();
        }
    }

    // finished, so let's construct the ordered vector 
    // by sorting all of the actions in the map by index
    m_actionVector->reserve(actionMap.size());
    for (ActionMap::const_iterator it = actionMap.begin(); it!= actionMap.end(); it++) {
        m_actionVector->push_back(it->second);
    }

    // success
    return true;
}

