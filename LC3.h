#ifndef LC3_H
#define LC3_H

#include <QMainWindow>
#include "lc3instructions.h"
#include "ui_lc3.h"
#include "lc3memory.h"
#include "binfile.h"
#include "assembler.h"
#include "memorytablemodel.h"
extern LC3Registers registers;
extern int index;


QT_BEGIN_NAMESPACE


namespace Ui {
class lc3;
}

class lc3 : public QMainWindow
{
    Q_OBJECT

public:
    explicit lc3(QWidget *parent = nullptr);
    ~lc3();

private slots:
    void on_RUN_clicked();
    void on_Upload_code_clicked();

    void on_ASSEMBLE_clicked();

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

#endif // LC3_H
