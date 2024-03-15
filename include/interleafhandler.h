#ifndef INTERLEAFHANDLER_H
#define INTERLEAFHANDLER_H

#include <interleaf.h>

class InterleafHandler {
public:
    si::Interleaf::Error ReadInterleaf(char *p_filePath);
    bool StartActionSorting();

    inline std::map<size_t, std::string> *GetActionMap() { return m_actionMap; }

private:
    void AddActionsToMap(si::Core *p_object);
    void ProcessDuplicates();
    si::Interleaf m_inlf;
    size_t m_actionCount;
    std::map<size_t, std::string> *m_actionMap;
};

#endif // INTERLEAFHANDLER_H

