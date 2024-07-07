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

QVector<QString> readLinesFromFile(const QString &filename);
QMap<QString, uint16_t> processLabels(const QVector<QString> &lines);
void assembleInstructionSetA(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, LC3Memory &memory);
QString assembleInstructionSetB(const QString &instruction, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QVector<QString> splitLine(const QString &line, const QChar &separator);
bool validateInstructionFormat(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels);
QString intToBinaryString(int value, int bits);
QVector<QString> splitInstruction(const QString &instruction);
bool parseOrgDirective(const QVector<QString> &tokens, uint16_t &address);
bool processInstruction(const QString &line, uint16_t &address, const QMap<QString, uint16_t> &labels, LC3Memory &memory);
bool handleLabelledInstruction(const QString &line, uint16_t &address, const QMap<QString, uint16_t> &labels, LC3Memory &memory);
void assembleInstructionSetA(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, LC3Memory &memory);
QString assembleAdd(const QVector<QString> &tokens);
QString assembleAnd(const QVector<QString> &tokens);
QString assembleBr(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QString assembleJmp(const QVector<QString> &tokens);
QString assembleJsr(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QString assembleJsrr(const QVector<QString> &tokens);
QString assembleLd(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QString assembleLdi(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QString assembleLdr(const QVector<QString> &tokens);
QString assembleLea(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QString assembleNot(const QVector<QString> &tokens);
QString assembleRet();
QString assembleSt(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QString assembleSti(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress);
QString assembleStr(const QVector<QString> &tokens);
QString assembleHalt();
QString assembleDec(const QVector<QString> &tokens);
QString assembleHex(const QVector<QString> &tokens);
#endif // ASSEMBLERLOGIC_H
