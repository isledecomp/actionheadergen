#ifndef INTERLEAFHANDLER_H
#define INTERLEAFHANDLER_H

#include <interleaf.h>

class InterleafHandler {
public:
    si::Interleaf::Error ReadInterleaf(char *p_filePath);
    bool SortActionsIntoVector();

    inline std::vector<std::string> *GetActionVector() { return m_actionVector; }

private:
    void AddActionsToMap(std::map<size_t, std::string> &p_actionMap, si::Core *p_object);
    si::Interleaf m_inlf;
    size_t m_actionCount;
    std::vector<std::string> *m_actionVector;
};

#endif // INTERLEAFHANDLER_H

