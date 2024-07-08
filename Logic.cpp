
#include "Logic.h"
#include "lc3instructions.h"
#include "ui_Logic.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QScrollBar>
LC3Memory memory(0xFFFF);
LC3Registers registers;
LC3Instructions instructions;
QString fileName;
int index;
int sc=1;
Logic::Logic(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::lc3)
{
    ui->setupUi(this);
    setFixedSize(1260, 610);
    ui->textEdit->setPlaceholderText("You can write your code here or click 'Upload File' to upload a .asm file. Afterward, click 'Assemble', wait, and then click 'Next Phase' to start doing the LC3 cycles.");
    memoryFill();
    setupRegisterTable();
    setupAdditionalTable();
    setupFlagsTable();
}

Logic::~Logic()
{
    delete ui;
}

void Logic::setupRegisterTable() {
    // Hide horizontal and vertical headers
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->verticalHeader()->setVisible(false);

    // Set up the table with "Register" and "Content" labels in the first column
    QStringList registerNames = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"};
    int numColumns = registerNames.size() + 1; // Register names + 1 for "Content"

    ui->tableWidget->setRowCount(2); // Two rows for "Register" and "Content"
    ui->tableWidget->setColumnCount(numColumns); // Set total number of columns

    // Manually add "Register" and "Content" labels in the first column
    QTableWidgetItem *registerLabel = new QTableWidgetItem("Register");
    QTableWidgetItem *contentLabel = new QTableWidgetItem("Content");
    registerLabel->setTextAlignment(Qt::AlignCenter); // Align text to center
    contentLabel->setTextAlignment(Qt::AlignCenter);  // Align text to center

    ui->tableWidget->setItem(0, 0, registerLabel); // Place "Register" label in first row
    ui->tableWidget->setItem(1, 0, contentLabel);  // Place "Content" label in second row

    // Add register names and initial content
    for (int i = 0; i < registerNames.size(); ++i) {
        QTableWidgetItem *registerItem = new QTableWidgetItem(registerNames[i]);
        QTableWidgetItem *contentItem = new QTableWidgetItem("0x0000");
        registerItem->setTextAlignment(Qt::AlignCenter); // Align text to center
        contentItem->setTextAlignment(Qt::AlignCenter);  // Align text to center

        // Make items non-editable
        registerItem->setFlags(registerItem->flags() & ~Qt::ItemIsEditable);
        contentItem->setFlags(contentItem->flags() & ~Qt::ItemIsEditable);

        ui->tableWidget->setItem(0, i + 1, registerItem); // Offset by 1 to skip first column
        ui->tableWidget->setItem(1, i + 1, contentItem);  // Offset by 1 to skip first column
    }

    // Make entire table non-editable and non-selectable
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // Make columns stretch to fit the widget
    for (int i = 0; i < ui->tableWidget->columnCount(); ++i) {
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    // Make rows stretch to fit the widget
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        ui->tableWidget->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    // Ensure the last section stretches
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->verticalHeader()->setStretchLastSection(true);
}

void Logic::setupAdditionalTable() {
    // Hide horizontal and vertical headers
    ui->additionalTableWidget->horizontalHeader()->setVisible(false);
    ui->additionalTableWidget->verticalHeader()->setVisible(false);

    // Set up the table with "Register" and "Content" labels in the first column
    QStringList additionalNames = {"MAR", "MDR", "PC", "IR"};
    ui->additionalTableWidget->setRowCount(2); // Two rows for Register and Content
    ui->additionalTableWidget->setColumnCount(additionalNames.size() + 1); // Columns for additional names + 1 for Register/Content

    // Manually add "Register" and "Content" labels in the first column
    QTableWidgetItem *registerLabel = new QTableWidgetItem("Register");
    QTableWidgetItem *contentLabel = new QTableWidgetItem("Content");
    registerLabel->setTextAlignment(Qt::AlignCenter); // Align text to center
    contentLabel->setTextAlignment(Qt::AlignCenter);  // Align text to center

    ui->additionalTableWidget->setItem(0, 0, registerLabel); // Place "Register" label in first row
    ui->additionalTableWidget->setItem(1, 0, contentLabel);  // Place "Content" label in second row

    // Add additional names and initial content
    for (int i = 0; i < additionalNames.size(); ++i) {
        QTableWidgetItem *additionalItem = new QTableWidgetItem(additionalNames[i]);
        QTableWidgetItem *contentItem = new QTableWidgetItem("0x0000");
        additionalItem->setTextAlignment(Qt::AlignCenter); // Align text to center
        contentItem->setTextAlignment(Qt::AlignCenter);    // Align text to center

        // Make items non-editable
        additionalItem->setFlags(additionalItem->flags() & ~Qt::ItemIsEditable);
        contentItem->setFlags(contentItem->flags() & ~Qt::ItemIsEditable);

        ui->additionalTableWidget->setItem(0, i + 1, additionalItem); // Offset by 1 to skip first column
        ui->additionalTableWidget->setItem(1, i + 1, contentItem);     // Offset by 1 to skip first column
    }

    // Make entire table non-editable and non-selectable
    ui->additionalTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->additionalTableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // Make columns stretch to fit the widget
    for (int i = 0; i < ui->additionalTableWidget->columnCount(); ++i) {
        ui->additionalTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    // Make rows stretch to fit the widget
    for (int i = 0; i < ui->additionalTableWidget->rowCount(); ++i) {
        ui->additionalTableWidget->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    // Ensure the last section stretches
    ui->additionalTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->additionalTableWidget->verticalHeader()->setStretchLastSection(true);
}

void Logic::setupFlagsTable() {
    // Hide horizontal and vertical headers
    ui->flagsTableWidget->horizontalHeader()->setVisible(false);
    ui->flagsTableWidget->verticalHeader()->setVisible(false);

    // Set up the table with "Register" and "Content" labels in the first column
    QStringList flagsNames = {"Negative", "Positive", "Zero"};
    ui->flagsTableWidget->setRowCount(2); // Two rows for Register and Content
    ui->flagsTableWidget->setColumnCount(flagsNames.size() + 1); // Columns for flags + 1 for Register/Content

    // Manually add "Register" and "Content" labels in the first column
    QTableWidgetItem *registerLabel = new QTableWidgetItem("Register");
    QTableWidgetItem *contentLabel = new QTableWidgetItem("Content");
    registerLabel->setTextAlignment(Qt::AlignCenter); // Align text to center
    contentLabel->setTextAlignment(Qt::AlignCenter);  // Align text to center

    ui->flagsTableWidget->setItem(0, 0, registerLabel); // Place "Register" label in first row
    ui->flagsTableWidget->setItem(1, 0, contentLabel);  // Place "Content" label in second row

    // Add flags names and initial content
    for (int i = 0; i < flagsNames.size(); ++i) {
        QTableWidgetItem *flagsItem = new QTableWidgetItem(flagsNames[i]);
        QTableWidgetItem *contentItem = new QTableWidgetItem("0x0000");
        flagsItem->setTextAlignment(Qt::AlignCenter); // Align text to center
        contentItem->setTextAlignment(Qt::AlignCenter);  // Align text to center

        // Make items non-editable
        flagsItem->setFlags(flagsItem->flags() & ~Qt::ItemIsEditable);
        contentItem->setFlags(contentItem->flags() & ~Qt::ItemIsEditable);

        ui->flagsTableWidget->setItem(0, i + 1, flagsItem); // Offset by 1 to skip first column
        ui->flagsTableWidget->setItem(1, i + 1, contentItem);  // Offset by 1 to skip first column
    }

    // Make entire table non-editable and non-selectable
    ui->flagsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->flagsTableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // Make columns stretch to fit the widget
    for (int i = 0; i < ui->flagsTableWidget->columnCount(); ++i) {
        ui->flagsTableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
    // Make rows stretch to fit the widget
    for (int i = 0; i < ui->flagsTableWidget->rowCount(); ++i) {
        ui->flagsTableWidget->verticalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    // Ensure the last section stretches
    ui->flagsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->flagsTableWidget->verticalHeader()->setStretchLastSection(true);
}

void Logic::updateRegisterContent(int registerIndex, uint16_t value) {
    QString content = QString::number(value, 16).toUpper();
    ui->tableWidget->item(1, registerIndex)->setText("0x" + content);
}

void Logic::updateAllRegisters() {
    updateRegisterContent(1, registers.getR(0)); // Start from index 1
    updateRegisterContent(2, registers.getR(1));
    updateRegisterContent(3, registers.getR(2));
    updateRegisterContent(4, registers.getR(3));
    updateRegisterContent(5, registers.getR(4));
    updateRegisterContent(6, registers.getR(5));
    updateRegisterContent(7, registers.getR(6));
    updateRegisterContent(8, registers.getR(7)); // End at index 8
}

void Logic::updateFlagContent(int index, uint16_t value) {
    QString content = QString::number(value).toUpper();
    ui->flagsTableWidget->item(1, index)->setText(content);
}

void Logic::updateAllFlags() {
    updateFlagContent(1, (registers.getCC() >> 2) & 0x1); // Negative
    updateFlagContent(2, registers.getCC() & 0x1); // Positive
    updateFlagContent(3, (registers.getCC() >> 1) & 0x1); // Zero
}

void Logic::updateAdditionalContent(int index, uint16_t value) {
    QString content = QString::number(value, 16).toUpper();
    ui->additionalTableWidget->item(1, index)->setText("0x" + content);
}

void Logic::updateAllAdditionalValues() {
    updateAdditionalContent(1, registers.getMAR()); // Start from index 1
    updateAdditionalContent(2, registers.getMDR());
    updateAdditionalContent(3, registers.getPC());
    updateAdditionalContent(4, registers.getIR()); // End at index 4
}


void Logic::updateRegisters()
{
    updateAllRegisters();
    updateAllAdditionalValues();
    updateAllFlags();

}

void Logic::updateMemory(int scrollToIndex)
{
    int rowCount = ui->memoryTable->rowCount();

    for (int index = 0; index < rowCount; ++index)
    {
        QTableWidgetItem *valueItem = new QTableWidgetItem(QString("0x%1").arg(memory.read(index), 4, 16, QChar('0')).toUpper());
        valueItem->setTextAlignment(Qt::AlignCenter); // Align text to center
        ui->memoryTable->setItem(index, 1, valueItem);
    }

    // Ensure scrollToIndex is within bounds
    if (scrollToIndex >= 0 && scrollToIndex < rowCount)
    {
        // Get the item at scrollToIndex
        QTableWidgetItem *itemToScrollTo = ui->memoryTable->item(scrollToIndex, 1);

        // Scroll to the item
        if (itemToScrollTo)
        {
            ui->memoryTable->scrollToItem(itemToScrollTo, QAbstractItemView::PositionAtCenter);
        }
    }
}

void Logic::memoryFill() {
    // Set up table dimensions and headers
    ui->memoryTable->setRowCount(0xFFFF); // Set row count to 0xFFFF (65535)
    ui->memoryTable->setColumnCount(2);   // Two columns: Address and Value
    QStringList headers = {"Address", "Value"};
    ui->memoryTable->setHorizontalHeaderLabels(headers);
    ui->memoryTable->verticalHeader()->setVisible(false); // Hide the vertical header

    // Set up header style with a complementary color
    QString headerStyle = "font: 700 11pt \"UD Digi Kyokasho NK-B\";"
                          "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                          "stop:0 #5afdf0, stop:0.5 #00f9b9, stop:1 #00bfa5);"
                          "color: black;"; // Adjusted color for contrast

    // Apply style to header
    ui->memoryTable->horizontalHeader()->setStyleSheet("QHeaderView::section { " + headerStyle + " }");

    // Populate table with memory values
    for (size_t i = 0; i < 0xFFFF; ++i) {
        QTableWidgetItem *addressItem = new QTableWidgetItem(QString("0x%1").arg(i, 4, 16, QChar('0')).toUpper());
        addressItem->setTextAlignment(Qt::AlignCenter);
        ui->memoryTable->setItem(i, 0, addressItem);

        QTableWidgetItem *valueItem = new QTableWidgetItem(QString("0x%1").arg(memory.read(i), 4, 16, QChar('0')).toUpper());
        valueItem->setTextAlignment(Qt::AlignCenter);
        ui->memoryTable->setItem(i, 1, valueItem);
    }

    // Make entire table non-editable and non-selectable
    ui->memoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->memoryTable->setSelectionMode(QAbstractItemView::NoSelection);

    // Optionally, set the font and size for the items in the table
    QFont tableFont("UD Digi Kyokasho NK-B", 11);
    ui->memoryTable->setFont(tableFont);
}

void Logic::on_Upload_code_clicked()
{
    QString newFileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Assembly Files (*.asm)"));
    if (!newFileName.isEmpty())
    {
        if (newFileName.endsWith(".asm", Qt::CaseInsensitive))
        {
            QFile file(newFileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                ui->textEdit->setPlainText(in.readAll()); // Load file contents into text edit
                file.close();
                QMessageBox::information(this, tr("File Selected"), tr("You selected:\n%1").arg(newFileName));
            }
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("Failed to open file for reading."));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Invalid File"), tr("Please select a file with an .asm extension."));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("No File Selected"), tr("No file was selected."));
    }
}

void Logic::on_ASSEMBLE_clicked() {
    QString code;

    if (!ui->textEdit->toPlainText().isEmpty()) {
        // Use text from QTextEdit if it's not empty (user entered code directly)
        code = ui->textEdit->toPlainText();
    } else if (!fileName.isEmpty()) {
        // Load file contents if a file was previously uploaded
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            code = in.readAll();
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to open file for reading."));
            return; // Exit if file cannot be opened
        }
    } else {
        QMessageBox::warning(this, tr("No Code Provided"), tr("Please either upload an .asm file or enter code in the text editor."));
        return; // Exit if no file or text is provided
    }

    // Proceed with assembly using 'code'
    int result = startAssembly(code); // Ensure 'code' is a QString
    if (result != 0) {
        qWarning() << "Assembly failed with error code:" << result;
        // Handle error scenario as needed
    } else {
        BinFile.readFromFile(0x3000);
        registers.setPC(0x3000);
        index = 0x3000;
        memoryFill();
        updateMemory(index); // Ensure memory is filled and visible
    }
}

void Logic::on_Reset_clicked()
{
    // Clear the file name variable
    fileName.clear();

    // Clear the QTextEdit content
    ui->textEdit->clear();

    // Reset memory

    for (uint16_t i = 0; i < 0xFFFF; ++i) {
        memory.write(i, 0x0000);
    }
    // Reset program counter (PC)
    registers.setPC(0x0000);

    // Reset MDR, MAR, and IR
    registers.setMDR(0x0000);
    registers.setMAR(0x0000);
    registers.setIR(0x0000);
    // Update the UI to reflect these changes
    updateAllRegisters();
    updateAllAdditionalValues();
    updateAllFlags();

    // Clear memory table contents
    for (int i = 0; i < ui->memoryTable->rowCount(); ++i) {
        QTableWidgetItem *addressItem = ui->memoryTable->item(i, 0);
        QTableWidgetItem *valueItem = ui->memoryTable->item(i, 1);
        if (addressItem) addressItem->setText(QString("0x%1").arg(i, 4, 16, QChar('0')).toUpper());
        if (valueItem) valueItem->setText("0x0000");
    }
    ui->Phase->clear();
    // Reset simulation phase counter
    sc = 1;
}

void Logic::on_nextCycle_clicked()
{
    if (sc == -1)
    {
        // HALT
        if (instructions.isHalt())
        {
            QMessageBox::information(this, "Program Done", "The program has reached the HALT instruction and is done.");
        }
    }
    else if (sc == 1)
    {
        instructions.fetch(memory);
        if (instructions.isHalt())
        {
            QMessageBox::information(this, "Program Done", "The program has reached the HALT instruction and is done.");
            sc = -1;
        }
        else
        {
            updateRegisters();
            ui->Phase->setText("Fetch");
            sc++;
        }
    }
    else if (sc == 2)
    {
        instructions.decode();
        updateRegisters();
        ui->Phase->setText("Decode");
        sc++;
    }
    else if (sc == 3)
    {
        instructions.evaluateAddress(memory);
        updateRegisters();
        ui->Phase->setText("Evaluate Address");
        sc++;
    }
    else if (sc == 4)
    {
        instructions.fetchOperands(memory);
        updateRegisters();
        ui->Phase->setText("Fetch Operands");
        sc++;

    } else if(sc == 5){
        instructions.execute();
        updateRegisters();
        ui->Phase->setText("Execute");
        sc++;
    }
    else if (sc == 6)
    {
        instructions.store(memory);
        updateRegisters();
        ui->Phase->setText("Store");
        updateMemory(index);
        sc = 1;
    }

}

void Logic::on_SampleCode_clicked()
{
    QString sampleCode = R"(; This symbol is used for comments ;
; A program to calculate addition of numbers from 1 to N
ORG 0x3000
; Initialize variables
LD R0, INIT ; Load R0 with the initial value (1)
LD R2, N ; Load R2 with loop size
AND R1, R1, #0 ; Clear R1 (accumulator for sum)
; Loop to add numbers from 1 to 100
LOOP, ADD R1, R1, R0 ; Add current number (R0) to sum (R1)
ADD R0, R0, #1 ; Increment current number
ADD R2, R2, #1 ; Increment loop counter
BRn LOOP ; If counter negative (not reached 0), repeat loop
; Store the result (sum of numbers 1 to 100) in memory
ST R1, RESULT ; Store the sum in memory location 'RESULT'
; Halt the program
HALT
; Initialize variables
INIT, DEC 1 ; Initial value for the loop (starting from 1)
RESULT, DEC 0 ; Memory location to store the re
N, DEC -3
END
    )";
    ui->textEdit->setText(sampleCode);
}

