#include <ctype.h>

#include "headergenerator.h"

typedef std::map<size_t, std::string> ActionMap;

// the generic extension to the header filename, used by weaver
const char *g_headerExt = "_actions.h";

// the generic extension used for the include guard
const char *g_headerGuard = "_ACTIONS_H";

// enum entry prefix used in the isle decomp
const char *g_enumEntryPrefix = "c_";

// notice to not edit autogenerated headers
const char *g_doNotEditNotice = "// This file was automatically generated by the actionheadergen tool.\n// Please do not manually edit this file.\n";

bool HeaderGenerator::GenerateHeader(char *p_interleafName, std::map<size_t, std::string> *p_actionMap, char *p_outputDir)
{
    // attempt to create header file
    if (!CreateHeader(p_interleafName, p_outputDir)) {
        printf("Failed to create header, check file permissions?\n");
        return false;
    }

    // attempt to write header file
    if (!WriteHeader(p_interleafName, p_actionMap)) {
        printf("Failed to write header\n");
        return false;
    }

    // success
    return true;
}

bool HeaderGenerator::CreateHeader(char *p_interleafName, char *p_outputDir)
{
    char headerName[1024];
    char outputPath[2048];

    strcpy(m_normalizedInlfName, p_interleafName);

    // format Interleaf name to mostly lowercase, 
    // to make acceptable by decomp styling guidelines
    for (int i = 1; i < strlen(p_interleafName); i++) {
        m_normalizedInlfName[i] = tolower(m_normalizedInlfName[i]);
    }

    // set the header name to the Interleaf name 
    strcpy(headerName, m_normalizedInlfName);

    // set first character to lowercase, which 
    // was skipped by the previous operation
    headerName[0] = tolower(headerName[0]);

    // append the generic extension
    strcat(headerName, g_headerExt);

    // generate the full path of the output file
    strcpy(outputPath, p_outputDir);
    strcat(outputPath, "/");
    strcat(outputPath, headerName);

    m_fout.open(outputPath);

    // make sure we can actually create this file
    if (!m_fout.is_open()) {
        return false;
    }

    return true;
}

bool HeaderGenerator::WriteHeader(char *p_interleafName, std::map<size_t, std::string> *p_actionMap)
{
    // create boilerplate
    char guardBuffer[2048];
    char guardName[1024];

    strcpy(guardName, p_interleafName);
    strcat(guardName, g_headerGuard);

    // might be messy, but I don't know a better way to do it
    strcpy(guardBuffer, "#ifndef ");
    strcat(guardBuffer, guardName);
    strcat(guardBuffer, "\n");
    strcat(guardBuffer, "#define ");
    strcat(guardBuffer, guardName);

    // insert notice to not edit this header
    m_fout << g_doNotEditNotice;

    // insert include guard
    m_fout << guardBuffer << "\n\n";

    // we need to scope this enum to avoid conflicts
    m_fout << "class " << m_normalizedInlfName << "Script {\n";
    m_fout << "public:\n";

    // declare enum
    m_fout << "    enum Script {\n";

    // add generic "none" state to the enum
    m_fout << "        c_none" << m_normalizedInlfName << " = -1,\n\n";

    // iterate through the action map and insert each action into enum
    bool isLast = false;
    size_t i = 0;
    ActionMap::iterator prev = p_actionMap->begin();
    for (ActionMap::iterator it = p_actionMap->begin(); it != p_actionMap->end(); it++) {
        isLast = it == (--p_actionMap->end());

        // seperate distanced action ids with whitespace (styling)
        if (it->first != prev->first + 1 && i != 0) {
            m_fout << "\n";
        }

        // actually write the enum entry
        m_fout << "        " << g_enumEntryPrefix << it->second << " = " << it->first;
        if (!isLast) {
            // if there are still more entries, we need write a comma
            m_fout << ",";
        }
        m_fout << "\n";
        prev = it;
        i++;
    }

    // all done with actions, so lets close the enum and class
    m_fout << "    };\n};\n\n";

    // finally, close the include guard
    m_fout << "#endif // " << guardName << "\n";

    // close the file
    m_fout.close();

    return true;
}

