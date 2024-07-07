#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QString>
#include "lc3memory.h"
#include "FileReadWrite.h"
#include <QCoreApplication>
#include <QRegularExpression>


extern FileReadWrite BinFile;
class Assembler
{

};

int startAssembly( QString &inputFilename);
#endif // ASSEMBLER_H


