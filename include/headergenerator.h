#ifndef HEADERGENERATOR_H
#define HEADERGENERATOR_H

#include <fstream>
#include <iostream>
#include <map>

class HeaderGenerator {
public:
    bool CreateForwardDeclHeader(char *p_outputDir);
    bool GenerateHeader(char *p_interleafName, std::map<size_t, std::string> *p_actionMap, char *p_outputDir);
    std::ofstream m_declFout;

private:
    bool CreateHeader(char *p_interleafName, char *p_outputDir);
    bool WriteHeader(char *p_interleafName, std::map<size_t, std::string> *p_actionMap);
    std::ofstream m_fout;
    char m_normalizedInlfName[512];
};

#endif // HEADERGENERATOR_H

