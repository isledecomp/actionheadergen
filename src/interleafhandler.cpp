#include "interleafhandler.h"

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

    // prepare our vector for use
    m_actionVector = new std::vector<const char *>;
    
    // itereate through every action in our action count
    for (size_t i = 0; i < m_actionCount; i++) {
        // get the action as Core initially
        si::Core *actionAsCore = m_inlf.GetChildAt(i);

        // Core doesn't provide some of the data we need, so we get the
        // action as Object, so then we can retrieve the name of the action
        if (si::Object *actionAsObject = dynamic_cast<si::Object*>(actionAsCore)) {
            // push the name of the action into the vector
            m_actionVector->push_back(actionAsObject->name().c_str());
        }
    }

    // success
    return true;
}

