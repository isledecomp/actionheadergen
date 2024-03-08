#ifndef INTERLEAFHANDLER_H
#define INTERLEAFHANDLER_H

#include <interleaf.h>

class InterleafHandler {
public:
    si::Interleaf::Error ReadInterleaf(char *p_filePath);
    bool SortActionsIntoVector();

    inline std::vector<const char *> *GetActionVector() { return m_actionVector; }

private:
    si::Interleaf m_inlf;
    size_t m_actionCount;
    std::vector<const char *> *m_actionVector;
};

#endif // INTERLEAFHANDLER_H

