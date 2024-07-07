#ifndef LOGIC_H
#define LOGIC_H

#include <QMainWindow>
#include "lc3instructions.h"
#include "ui_Logic.h"
#include "lc3memory.h"
#include "FileReadWrite.h"
#include "assembler.h"
#include "memorytablemodel.h"
extern LC3Registers registers;
extern int index;
extern LC3Memory memory;

QT_BEGIN_NAMESPACE


namespace Ui {
class Logic;
}

class Logic : public QMainWindow
{
    Q_OBJECT

public:
    explicit Logic(QWidget *parent = nullptr);
    ~Logic();

private slots:
    void on_RUN_clicked();
    void on_Upload_code_clicked();

   void on_ASSEMBLE_clicked();

    void on_Reset_clicked();


private:
    Ui::lc3 *ui;
    MemoryTableModel *memoryModel;

    void memoryFill();
    void updateMemory(int index);

    void setupRegisterTable();
    void setupAdditionalTable();
    void setupFlagsTable();
    void updateRegisterContent(int registerIndex, uint16_t value);
    void updateFlagContent(int index, uint16_t value);
    void updateAdditionalContent(int index, uint16_t value);
    void updateAllRegisters();
    void updateAllFlags();
    void updateAllAdditionalValues();
    void updateRegisters();
};

#endif // LOGIC_H
