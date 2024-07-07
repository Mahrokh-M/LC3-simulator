#ifndef BINFILE_H
#define BINFILE_H
#include <QFile>
#include <QString>
#include <QDebug>
#include "lc3memory.h"

extern LC3Memory memory;

class binFile
{
public:
    binFile();
    binFile(QString);
    void writeToFile(const LC3Memory&, uint16_t, uint16_t);
    bool readFromFile(uint16_t);

    QFile file;
};

#endif // BINFILE_H
