#include "interleafhandler.h"

#include <sstream>

typedef std::map<size_t, std::string> ActionMap;

si::Interleaf::Error InterleafHandler::ReadInterleaf(char* p_filePath)
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

	// process duplicates in the map
	ProcessDuplicates();

	// success
	return true;
}

void InterleafHandler::AddActionsToMap(si::Core* p_object)
{
	// Core doesn't provide some of the data we need, so we get the
	// action as Object, so then we can retrieve the name of the action
	if (si::Object* actionAsObject = dynamic_cast<si::Object*>(p_object)) {
		// if the object is valid at all
		if (actionAsObject->type() != si::MxOb::Null) {
			// check for sub-objects
			if (actionAsObject->HasChildren()) {
				for (si::Core::Children::const_iterator it = actionAsObject->GetChildren().begin();
					 it != actionAsObject->GetChildren().end();
					 ++it) {
					// we have sub-objects, so we need to account for these
					si::Object* subObject = static_cast<si::Object*>(*it);
					if (subObject->HasChildren()) {
						// if this sub-object contains more sub-objects, recurse
						AddActionsToMap(subObject);
					}
					// push the name of each action and its index into the map
					m_actionMap->insert(std::make_pair(subObject->id(), subObject->name()));
				}
				m_actionMap->insert(std::make_pair(actionAsObject->id(), actionAsObject->name()));
			}
			else {
				m_actionMap->insert(std::make_pair(actionAsObject->id(), actionAsObject->name()));
			}
		}
	}
}

void InterleafHandler::ProcessDuplicates()
{
	// some Interleaf files can have duplicate action names,
	// which is obviously bad in the context of an enum
	// where every label is expected to be unique

	std::map<std::string, size_t> duplicatesMap;
	std::stringstream ss;

	for (ActionMap::iterator it = m_actionMap->begin(); it != m_actionMap->end(); ++it) {
		// iterate through the entire action map
		// once to collect the duplicate count
		duplicatesMap[it->second]++;
	}

	for (ActionMap::iterator it = m_actionMap->begin(); it != m_actionMap->end(); ++it) {
		// iterate again; but this time we use the previously populated duplicatesMap
		// to update the action names of all of the actions that have duplicates
		if (duplicatesMap[it->second] > 1) {
			// append the index to the name
			ss.str("");
			ss << it->second << "_" << it->first;
			it->second = ss.str();
		}
	}
}
