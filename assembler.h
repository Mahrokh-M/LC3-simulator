#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QString>
#include "lc3memory.h"
#include "binfile.h"


extern binFile globalFile;
class Assembler
{
public:
    Assembler();
};

#endif // ASSEMBLER_H

int startAssembly( QString inputFilename);

