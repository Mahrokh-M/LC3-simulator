#ifndef FILEREADWRITE_H
#define FILEREADWRITE_H
#include <QFile>
#include <QString>
#include <QDebug>
#include "lc3memory.h"
#include <QMessageBox>


class FileReadWrite
{
public:
    FileReadWrite();
    FileReadWrite(QString);
    void writeToFile(const LC3Memory&, uint16_t, uint16_t);
    bool readFromFile(uint16_t);

    QFile file;
};

#endif // FILEREADWRITE_H
