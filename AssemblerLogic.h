#ifndef ASSEMBLERLOGIC_H
#define ASSEMBLERLOGIC_H

#include "lc3memory.h"
#include <QString>
#include <QVector>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <bitset>

// Function declarations
QString trim(const QString &str);
QVector<QString> split(const QString &str, QChar delimiter);
QString toBinary(int value, int bits);
QVector<QString> readFile(const QString &filename);
QMap<QString, uint16_t> firstPass(const QVector<QString> &lines);
QString assembleInstruction(const QString &instruction, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QVector<QString> Split(const QString &line, const QChar &separator);
bool validateInstruction(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels);
void secondPass(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, LC3Memory &memory);

#endif // ASSEMBLERLOGIC_H
