#ifndef HEADERGENERATOR_H
#define HEADERGENERATOR_H

#include <fstream>
#include <iostream>
#include <vector>

class HeaderGenerator {
public:
    bool GenerateHeader(char *p_interleafName, std::vector<std::string> *p_actionVector, char *p_outputDir);

private:
    bool CreateHeader(char *p_interleafName, char *p_outputDir);
    bool WriteHeader(char *p_interleafName, std::vector<std::string> *p_actionVector);
    std::ofstream m_fout;
    char m_normalizedInlfName[512];
};

#endif // HEADERGENERATOR_H

