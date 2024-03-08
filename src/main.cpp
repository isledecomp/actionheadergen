#include <dirent.h>
#include <stdio.h>
#include <string>

#include "headergenerator.h"
#include "interleafhandler.h"

bool RecursivelyFindInterleaf(const char *p_path, std::vector<std::string> &p_interleafFiles)
{
    // TODO: *by default* this is a UNIX-specific solution 
    // Windows is capable of using dirent through third party implementations,
    // but it would be nice to have something that works out of the box on Win32
    // this is one of the pains of C++98, there is no good directory support in the standard
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
                // found an Interleaf file, return it
                char fullPathBuffer[1024];

                // construct the full path to the file first
                snprintf(fullPathBuffer, sizeof(fullPathBuffer), "%s/%s", p_path, dentry->d_name);

                p_interleafFiles.push_back(fullPathBuffer);
            }
        }
    }

    // success
    closedir(directory);
    return true;
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
        if (!ihandler.SortActionsIntoVector()) {
            printf("No actions found in this Interleaf, exiting\n");
            return 1;
        }

        // set filename to filePath without directory appended
        // libweaver unfortunately doesn't provide a way to get
        // the Interleaf name so we have to construct it ourselves
        char *filename = strrchr(currentFilePath, '/');
        if (filename) {
            // we don't need ".SI" for our purposes 
            // so just remove the last 3 characters
            filename[strlen(filename) - 3] = '\0';
            // remove the first character as well, since it will be "/"
            filename = filename + 1;
        }

        HeaderGenerator hgenerator;

        // generate the actual header
        // we use the Interleaf name for the filename
        // and the vector previously generated to make the enum data
        if (!hgenerator.GenerateHeader(filename, ihandler.GetActionVector(), outputDir)) {
            // we failed for some reason, so let's just move on
            printf("Failure generating header for %s, skipping\n", filename);

            delete ihandler.GetActionVector();
            continue;
        }

        // all steps were successful
        delete ihandler.GetActionVector();

        printf("Succesfully generated header for %s\n", filename);
    }

    printf("Finished!\n");
    return 0;
}

