#include "headergenerator.h"
#include "interleafhandler.h"

#ifdef _WIN32
#include <shlwapi.h>
#include <windows.h>
#define strcasecmp _stricmp
#undef ERROR_SUCCESS
#else
#include <dirent.h>
#endif

#include <stdio.h>
#include <string>

bool RecursivelyFindInterleaf(const char* p_path, std::vector<std::string>& p_interleafFiles)
{
#ifdef _WIN32
    // C++98 compliant Windows implementation
    HANDLE hFind;
    WIN32_FIND_DATAA findData;

    std::string searchPattern = std::string(p_path) + "\\*";

    hFind = FindFirstFile(searchPattern.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Could not open directory %s, exiting\n", p_path);
        return false;
    }

    while (true)
    {
        // filter out backwards paths
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
        {
            if (!FindNextFileA(hFind, &findData))
            {
                // no more files left, exit
                break;
            }
            continue;
        }

        // this is a subdirectory, so recurse
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            char nextPathBuffer[1024];
            snprintf(nextPathBuffer, sizeof(nextPathBuffer), "%s\\%s", p_path, findData.cFileName);

            if (!RecursivelyFindInterleaf(nextPathBuffer, p_interleafFiles))
                // couldn't access this subdirectory, abort
                return false;
        }
        else
        {
            // found an Interleaf file
            if (StrStrI(findData.cFileName, ".si") != NULL)
            {
                // construct the full path to the file first
                char fullPathBuffer[1024];
                snprintf(fullPathBuffer, sizeof(fullPathBuffer), "%s\\%s", p_path, findData.cFileName);

                // push it to the vector
                p_interleafFiles.push_back(fullPathBuffer);
            }
        }

        if (!FindNextFile(hFind, &findData))
        {
            // no more files left, exit
            break;
        }
    }

    // success
    FindClose(hFind);
    return true;
#else
    // POSIX implementation using dirent
    DIR *directory = opendir(p_path);
    if (!directory) {
        printf("Could not open directory %s, exiting\n", p_path);
        return false;
    }

    struct dirent *dentry;
    // start scouring the directory
    while ((dentry = readdir(directory)) != NULL) {
        // if this path is a directory
        if (dentry->d_type == DT_DIR) {
            // filter out backwards paths
            if (strcmp(dentry->d_name, ".") != 0 && strcmp(dentry->d_name, "..") != 0) {
                // this is a subdirectory, so recurse
                char nextPathBuffer[1024];
                snprintf(nextPathBuffer, sizeof(nextPathBuffer), "%s/%s", p_path, dentry->d_name);

                if (!RecursivelyFindInterleaf(nextPathBuffer, p_interleafFiles)) {
                    // couldn't access this subdirectory, abort
                    return false;
                }
            }
        }
        else {
            if (strcasestr(dentry->d_name, ".si") != NULL) {
                // found an Interleaf file
                char fullPathBuffer[1024];

                // construct the full path to the file first
                snprintf(fullPathBuffer, sizeof(fullPathBuffer), "%s/%s", p_path, dentry->d_name);

                // push it to the vector
                p_interleafFiles.push_back(fullPathBuffer);
            }
        }
    }

    // success
    closedir(directory);
    return true;
#endif
}

int main(int argc, char *argv[])
{
    // no file or output directory provided
    if (argc < 3) {
        printf("Usage: %s (<interleaf.si> or <input directory>) <output directory>\n", argv[0]);
        return 1;
    }

    const char *interleafExtension = ".si";

    std::vector<std::string> interleafFiles;

    // cache the arguments so we don't have to enter the array unnecessarily
    char *filePath = argv[1];
    char *outputDir = argv[2];

    // if filePath doesn't end in ".si",
    // we got passed a directory, enter batch mode
    if (strcasecmp(filePath + strlen(filePath) - strlen(interleafExtension), interleafExtension) != 0) { 
        // find each Interleaf recursively in the directory
        // get all files and folders in current directory
        if (!RecursivelyFindInterleaf(filePath, interleafFiles)) {
            return 1;
        }
    }
    else interleafFiles.push_back(filePath); // we were provided a single file, but we'll still use the vector

    // generate special forward declaration header
    HeaderGenerator hgenerator;

    if (!hgenerator.CreateForwardDeclHeader(outputDir)) {
        return 1;  
    }

    // iterate through every Interleaf in the vector and perform our operations
    for (int i = 0; i < interleafFiles.size(); i++) {

        InterleafHandler ihandler;

        char currentFilePath[1024];
        strcpy(currentFilePath, interleafFiles.at(i).c_str());

        // load the Interleaf provided to us in the first argument 
        // InterleafHandler is using libweaver for this
        // if we're not successful, exit
        if (ihandler.ReadInterleaf(currentFilePath) != si::Interleaf::ERROR_SUCCESS) {
            printf("Failure reading Interleaf, exiting\n");
            return 1;
        }

        // sort the Interleaf's actions into a vector 
        // if libweaver is stable enough, a failure is only
        // possible if no actions exist in the Interleaf
        if (!ihandler.StartActionSorting()) {
            printf("No actions found in this Interleaf, exiting\n");
            return 1;
        }

        // set filename to filePath without directory appended
        // libweaver unfortunately doesn't provide a way to get
        // the Interleaf name so we have to construct it ourselves
        char *lastIndexSlash = strrchr(currentFilePath, '/');
        char *lastIndexBackslash = strrchr(currentFilePath, '\\');
        char *filename = (lastIndexSlash > lastIndexBackslash) ? lastIndexSlash : lastIndexBackslash;

        if (filename) {
            // we don't need ".SI" for our purposes 
            // so just remove the last 3 characters
            filename[strlen(filename) - 3] = '\0';
            // remove the first character as well, since it will be "/"
            filename = filename + 1;
        }

        // generate the actual header
        // we use the Interleaf name for the filename
        // and the vector previously generated to make the enum data
        if (!hgenerator.GenerateHeader(filename, ihandler.GetActionMap(), outputDir)) {
            // we failed for some reason, so let's just move on
            printf("Failure generating header for %s, skipping\n", filename);

            delete ihandler.GetActionMap();
            continue;
        }

        // all steps were successful
        delete ihandler.GetActionMap();

        printf("Successfully generated header for %s\n", filename);
    }
    
    hgenerator.m_declFout << "#endif // ACTIONSFWD_H\n";
    hgenerator.m_declFout.close();

    printf("Finished!\n");
    return 0;
}

