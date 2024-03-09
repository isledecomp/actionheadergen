#include "interleafhandler.h"

si::Interleaf::Error InterleafHandler::ReadInterleaf(char *p_filePath)
{
    // this is basically a wrapper function for libweaver's
    // Read() so we can set private member variables
    return m_inlf.Read(p_filePath);
}

bool InterleafHandler::StartActionSorting()
{
    // if there's no actions in this Interleaf, exit
    if (!m_inlf.HasChildren()) {
        return false;
    }

    // get the amount of actions in this Interleaf
    m_actionCount = m_inlf.GetChildCount();

    // prepare our map for use
    m_actionMap = new std::map<size_t, std::string>();

    // generate the map using recursion
    for (size_t i = 0; i < m_actionCount; i++) {
        AddActionsToMap(m_inlf.GetChildAt(i));
    }

    // success
    return true;
}

void InterleafHandler::AddActionsToMap(si::Core *p_object)
{
    // Core doesn't provide some of the data we need, so we get the
    // action as Object, so then we can retrieve the name of the action
    if (si::Object *actionAsObject = dynamic_cast<si::Object *>(p_object)) {    
        // if the object is valid at all
        if (actionAsObject->type() != si::MxOb::Null) {
            // check for sub-objects
            if (actionAsObject->HasChildren()) {
                for (si::Core::Children::const_iterator it = actionAsObject->GetChildren().cbegin(); it != actionAsObject->GetChildren().cend(); it++) {
                    // we have sub-objects, so we need to account for these
                    si::Object *subObject = static_cast<si::Object *>(*it);
                    if (subObject->HasChildren()) {
                        // if this sub-object contains more sub-objects, recurse
                        AddActionsToMap(subObject);
                    }
                    // push the name of each action and its index into the map
                    m_actionMap->insert(std::make_pair(subObject->id(), subObject->name()));
                }
                m_actionMap->insert(std::make_pair(actionAsObject->id(), actionAsObject->name()));
            }
            else m_actionMap->insert(std::make_pair(actionAsObject->id(), actionAsObject->name()));
        }
    }
}

