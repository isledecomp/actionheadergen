# actionheadergen

This tool automatically generates C++ headers containing enums representing the action index found in LEGO Island Inferleaf (.SI) files. While it is primarily designed to solve a glaring labor issue with manually inserting thousands of action entries in the ongoing [LEGO Island decompilation](https://github.com/isledecomp/isle), it could be repurposed for other uses as well.

## Background

It is known that Mindscape used some form of this header structure in the original development environment, though it is currently unknown if they automatically generated them. While no retail source code for the game has ever surfaced, some [Weaver](https://www.legoisland.org/wiki/Weaver) input files (.SS) have, which showcase the use of these headers.

A small excerpt of JUKEBOXW.SS is attached below, showcasing the relevant include statement:

```
#include "..\Include\JukeBoxWorld_Actions.h"

defineSettings Configuration
{
    bufferSizeKB = 64;
    buffersNum = 16;
}
```

It is a logical assumption that these headers were shared between Weaver and the actual C++ codebase as well, given their use and support of standard C-syntax files even in a proprietary scripting tool where they had the liberties to use whatever they’d like.

It is also evident that the original codebase did not use magic numbers to play actions, as action indexes in the Interleaf files have shifted considerably between each of the currently known builds of LEGO Island. When you consider the universality needed, the use of standard headers in Weaver makes sense.

Some creative liberties possibly not present in the original codebase have been taken with the generation of these files as to match the styling guidelines of the decompilation.

## Building

This project uses the CMake build system. As this project makes use of [libweaver](https://github.com/isledecomp/SIEdit), you will need to pull the source recursively. For now, you will have to go into the libweaver subdirectory and build it manually as the CMakeLists in this project only targets the tool itself.

The code in this repository should be cross-platform, aside from the use of dirent. Dirent has third party implementations on Windows that this project should build fine with, but a native recursive file search function using the WinAPI is still needed to eliminate this edge-case.

## Usage

`./actionheadergen (<interleaf.si> or <input directory>) <output directory>`

Simply point to either a singular Interleaf file that you’d like to generate a header of, or a directory containing multiple Interleaf files. This can also simply be your “SCRIPTS” folder in your LEGO Island installation. Make sure to additionally provide an output directory for the generated headers to go.

If all goes well, you can find the generated headers in the output directory.