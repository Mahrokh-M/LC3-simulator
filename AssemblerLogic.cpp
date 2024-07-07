#include"AssemblerLogic.h"
#include "qregularexpression.h"




QVector<QString> readFile(const QString &filename)
{
    QVector<QString> lines;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "Error", "Cannot open file for reading: " + file.errorString());
        return lines;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        lines.append(trim(line));
    }
    file.close(); // Always close the file after reading
    return lines;
}

//labels and their corresponding memory addresses
QMap<QString, uint16_t> firstPass(const QVector<QString> &lines)
{
    QMap<QString, uint16_t> labels;
    uint16_t address = 0x3000; // Starting address
    for (const QString &line : lines)
    {
        if (line.isEmpty() || line.startsWith(';'))
            continue; // Ignore empty lines and comments

        QRegularExpression re("\\s+");
        QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);
        if (tokens[0] == "ORG")
        {
            QString addrString = tokens[1];
            bool ok;
            uint16_t newAddress = addrString.toInt(&ok, 16);
            if (!ok)
            {
                QMessageBox::critical(nullptr, "Error", "Error converting address: " + addrString);
            }
            else
            {
                address = newAddress; // Set starting address
                qDebug() << "Setting address to:" << address;
            }
            continue; // Skip further processing for ORG directive
        }
        else if (tokens[0].endsWith(','))
        {
            labels[tokens[0].chopped(1)] = address;
            if (tokens.size() > 1)   ///////////////////////////////////
            {
                address++; // If there's an instruction on the same line, increment address
            }
        }
        else if (tokens[0] == "END")
        {
            break; // Stop processing at the end directive
        }
        else
        {
            address++;
        }
    }
    return labels;
}

//assembly instructions to machine code
void secondPass(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, LC3Memory &memory)
{
    uint16_t address = 0x3000; // Starting address
    for (const QString &line : lines)
    {

        if (line.isEmpty() || line.startsWith(';'))
            continue; // Ignore empty lines and comments

        //QVector<QString> tokens = split(line, ' ');
        QRegularExpression re("\\s+");
        QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);

        if (tokens.size() > 1 && tokens[0] == "ORG")
        {
            QString addrString = tokens[1];
            bool ok;
            uint16_t newAddress = static_cast<uint16_t>(addrString.toInt(&ok, 16));
            if (!ok)
            {
                QMessageBox::critical(nullptr, "Error", "Error converting address: " + addrString);
            }
            else
            {
                address = newAddress; // Set starting address
                qDebug() << "Setting address to:" << address;
            }
        }

        else if (tokens.size() >= 1 && tokens[0] == "END")
        {
            break; // End of the program
        }

        else if (tokens[0].endsWith(','))
        {
            // This is a label, process the next token as an instruction if it exists
            if (tokens.size() > 1)
            {
                // Extract the instruction part after the label and comma
                QString instruction = line.mid(line.indexOf(',') + 1).trimmed();

                // Split the instruction into tokens excluding comments
                QVector<QString> tokens2 = Split(instruction, ' ');

                if (validateInstruction(tokens2, labels))
                {
                    QString binaryInstruction = assembleInstruction(instruction, labels, address);
                    bool ok;
                    uint16_t machineCode = static_cast<uint16_t>(binaryInstruction.toUInt(&ok, 2));
                    if (!ok)
                    {
                        QMessageBox::critical(nullptr, "Error", "Failed to convert binary instruction to machine code");
                        continue; // Skip writing invalid instruction to memory
                    }
                    memory.write(address, machineCode);
                    address++;
                }
                else
                {
                    qWarning() << "Skipping invalid instruction:" << line;
                }
            }
        }

        else
        {
            QVector<QString> tokens2 = Split(line, ' ');
            if (validateInstruction(tokens2, labels))
            { // validateInstruction(tokens, labels) fix
                // Regular instruction without label
                QString binaryInstruction = assembleInstruction(line, labels, address);

                bool ok;
                uint16_t machineCode = static_cast<uint16_t>(binaryInstruction.toUInt(&ok, 2));
                if (!ok)
                {

                    QMessageBox::critical(nullptr, "Error", "Failed to convert binary instruction to machine code");
                    continue; // Skip writing invalid instruction to memory
                }

                memory.write(address, machineCode); // Write machine code to memory
                address++;                          // Increment address
            }
            else
            {
                QMessageBox::critical(nullptr, "Error", "Skipping invalid instruction: " + line);
            }
        }
    }
}

//assemble a single instruction into binary
QString assembleInstruction(const QString &instruction, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    //QVector<QString> tokens = split(instruction, ' ');
    QRegularExpression re("\\s+");
    QVector<QString> tokens = instruction.split(re, Qt::SkipEmptyParts);
    for (int i = 0; i < tokens.size(); ++i)
    {
        tokens[i] = tokens[i].remove(',').trimmed(); // Remove commas and trim tokens
    }
    QString opcode = tokens[0];

    if (opcode == "ADD")
    {
        if (tokens.size() < 4) {
            QMessageBox::critical(nullptr, "Error", "ADD instruction requires at least 3 operands.");
            return "";
        }
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        QString sr1 = toBinary(tokens[2].mid(1).toInt(), 3);
        if (tokens[3].startsWith('R'))
        {
            QString sr2 = toBinary(tokens[3].mid(1).toInt(), 3);
            return "0001" + dr + sr1 + "000" + sr2;
        }
        else
        {
            int imm5 = tokens[3].mid(1).toInt(); // Remove the '#' and convert to integer
            return "0001" + dr + sr1 + "1" + toBinary(imm5, 5);
        }
    }

    if (opcode == "AND")
    {
        if (tokens.size() < 4) {
            QMessageBox::critical(nullptr, "Error", "AND instruction requires at least 3 operands.");
            return "";
        }
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);  // Destination register
        QString sr1 = toBinary(tokens[2].mid(1).toInt(), 3); // Source register 1

        if (tokens[3].startsWith('R'))
        {
            // Register-to-register AND operation
            QString sr2 = toBinary(tokens[3].mid(1).toInt(), 3); // Source register 2
            return "0101" + dr + sr1 + "000" + sr2;
        }
        else
        {
            // Immediate AND operation
            int imm5 = tokens[3].mid(1).toInt(); // Immediate value
            return "0101" + dr + sr1 + "1" + toBinary(imm5, 5);
        }
    }

    if (opcode.startsWith("BR"))
    {
        if (tokens.size() < 2) {
            QMessageBox::critical(nullptr, "Error", "BR instruction requires a condition flag and a label.");
            return "";
        }
        QString n = tokens[0].contains('n') ? "1" : "0"; // Check if 'n' condition flag is present
        QString z = tokens[0].contains('z') ? "1" : "0"; // Check if 'z' condition flag is present
        QString p = tokens[0].contains('p') ? "1" : "0"; // Check if 'p' condition flag is present

        int offset = labels.value(tokens[1]) - currentAddress - 1;
        QString offsetBinary = toBinary(offset, 9); // This will handle both positive and negative offsets
        return "0000" + n + z + p + offsetBinary;
    }

    if (opcode == "JMP")
    {
        if (tokens.size() < 2) {
            QMessageBox::critical(nullptr, "Error", "JMP instruction requires a register.");
            return "";
        }
        QString BaseR = tokens[1];
        int BaseRNum = BaseR.mid(1).toInt(); // Remove 'R' and convert to int

        // Construct binary instruction
        QString binaryInstruction = "1100000";
        binaryInstruction += toBinary(BaseRNum, 3);
        binaryInstruction += "000000";

        return binaryInstruction;
    }

    if (opcode == "JSR")
    {
        if (tokens.size() < 2) {
            QMessageBox::critical(nullptr, "Error", "JSR instruction requires a subroutine label.");
            return "";
        }
        QString label = tokens[1];
        uint16_t subroutineAddress = labels.value(label);

        // Calculate the offset from current address
        int16_t offset = subroutineAddress - currentAddress - 1;

        // Convert offset to binary with 11 bits
        QString offsetBinary = toBinary(offset, 11);

        // Construct the binary instruction
        QString binaryInstruction = "0100";
        binaryInstruction += "1";
        binaryInstruction += offsetBinary;

        return binaryInstruction;
    }

    if (opcode == "JSRR")
    {
        if (tokens.size() < 2) {
            QMessageBox::critical(nullptr, "Error", "JSRR instruction requires a register.");
            return "";
        }
        QString BaseR = tokens[1];
        int BaseRNum = BaseR.mid(1).toInt();

        // Construct binary instruction
        QString binaryInstruction = "0100000";
        binaryInstruction += toBinary(BaseRNum, 3);
        binaryInstruction += "000000";

        return binaryInstruction;
    }

    if (opcode == "LD")
    {
        if (tokens.size() < 3) {
            QMessageBox::critical(nullptr, "Error", "LD instruction requires a destination register and a label.");
            return "";
        }
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        int offset = labels.value(tokens[2]) - currentAddress - 1;
        return "0010" + dr + toBinary(offset, 9);
    }

    if (opcode == "LDI")
    {
        if (tokens.size() < 3) {
            QMessageBox::critical(nullptr, "Error", "LDI instruction requires a destination register and a label.");
            return "";
        }
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        int offset = labels.value(tokens[2]) - currentAddress - 1;
        return "1010" + dr + toBinary(offset, 9);
    }

    if (opcode == "LDR")
    {
        if (tokens.size() < 4) {
            QMessageBox::critical(nullptr, "Error", "LDR instruction requires a destination register, base register, and an offset.");
            return "";
        }
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        QString baser = toBinary(tokens[2].mid(1).toInt(), 3);
        int offset = tokens[3].mid(1).toInt();
        return "0110" + dr + baser + toBinary(offset, 6);
    }

    if (opcode == "LEA")
    {
        if (tokens.size() < 3) {
            QMessageBox::critical(nullptr, "Error", "LEA instruction requires a destination register and a label.");
            return "";
        }
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        int offset = labels.value(tokens[2]) - currentAddress - 1;
        return "1110" + dr + toBinary(offset, 9);
    }

    if (opcode == "NOT")
    {
        if (tokens.size() < 3) {
            QMessageBox::critical(nullptr, "Error", "NOT instruction requires a destination register and a source register.");
            return "";
        }
        QString dr = toBinary(tokens[1].mid(1).toInt(), 3);
        QString sr = toBinary(tokens[2].mid(1).toInt(), 3);
        return "1001" + dr + sr + "111111";
    }

    if (opcode == "RET")
    {
        return "1100000111000000";
    }

    if (opcode == "ST")
    {
        if (tokens.size() < 3) {
            QMessageBox::critical(nullptr, "Error", "ST instruction requires a source register and a label.");
            return "";
        }
        QString sr = toBinary(tokens[1].mid(1).toInt(), 3);
        int offsetValue = labels.value(tokens[2]) - currentAddress - 1;
        QString offset = toBinary(offsetValue, 9);
        return "0011" + sr + offset;
    }

    if (opcode == "STI")
    {
        if (tokens.size() < 3) {
            QMessageBox::critical(nullptr, "Error", "STI instruction requires a source register and a label.");
            return "";
        }
        QString sr = toBinary(tokens[1].mid(1).toInt(), 3);
        QString offset = toBinary(labels.value(tokens[2]) - currentAddress - 1, 9);
        return "1011" + sr + offset;
    }

    if (opcode == "STR")
    {
        QString sr = toBinary(tokens[1].mid(1).toInt(), 3);
        QString baseR = toBinary(tokens[2].mid(1).toInt(), 3);
        int offset6 = tokens[3].mid(1).toInt();
        return "0111" + sr + baseR + toBinary(offset6, 6);
    }

    if (opcode == "HALT")
    {
        return "1111000000100101"; // TRAP x25
    }

    if (opcode == "DEC")
    {
        bool ok;
        int16_t value = tokens[1].toInt(&ok);
        if (!ok)
        {
            qWarning() << "Invalid .DEC value:" << tokens[1];
            return "";
        }
        return toBinary(value, 16);
    }

    if (opcode == "HEX")
    {
        bool ok;
        uint16_t value = tokens[1].toUInt(&ok, 16);
        if (!ok)
        {
            qWarning() << "Invalid .HEX value:" << tokens[1];
            return "";
        }
        return toBinary(value, 16);
    }

    return "";
}

QVector<QString> Split(const QString &line, const QChar &separator)
{
    QVector<QString> tokens;
    QString currentToken;
    bool commentFound = false;

    for (int i = 0; i < line.size(); ++i)
    {
        QChar ch = line.at(i);

        if (ch == ';')
        {
            commentFound = true;
            break;
        }
        else if (ch == separator)
        {
            if (!currentToken.isEmpty())
            {
                tokens.append(currentToken.trimmed());
                currentToken.clear();
            }
        }
        else
        {
            currentToken += ch;
        }
    }

    if (!currentToken.isEmpty())
    {
        tokens.append(currentToken.trimmed());
    }

    return tokens;
}

bool validateInstruction(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels)
{
    if (tokens.isEmpty())
        return false;

    QString opcode = tokens[0].trimmed(); // Trim any leading or trailing whitespace

    auto isRegister = [](const QString &token)
    {
        int regNum = token.at(1).digitValue(); // Get the digit after 'R'

        bool hey = token.startsWith('R') && regNum >= 0 && regNum <= 7;
        return hey;
    };

    if (opcode == "ADD" || opcode == "AND")
    {
        if (tokens.size() != 4)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
        if (tokens[3].startsWith('R'))
        {
            if (!isRegister(tokens[3]))
                return false;
        }
        else
        {
            bool ok;
            int imm = tokens[3].mid(1).toInt(&ok);
            if (!ok || imm < -16 || imm > 15)
                return false;
        }
    }
    else if (opcode.startsWith("BR"))
    {
        if (tokens.size() != 2)
            return false;
        if (!labels.contains(tokens[1]))
            return false;
    }
    else if (opcode == "JMP" || opcode == "JSRR")
    {
        if (tokens.size() != 2)
            return false;
        if (!isRegister(tokens[1]))
            return false;
    }
    else if (opcode == "JSR")
    {
        if (tokens.size() != 2)
            return false;
        if (!labels.contains(tokens[1]))
            return false;
    }
    else if (opcode == "LD" || opcode == "LDI" || opcode == "LEA" || opcode == "ST" || opcode == "STI")
    {
        if (tokens.size() != 3)
            return false;
        if (!isRegister(tokens[1]))
            return false;
        if (!labels.contains(tokens[2]))
            return false;
    }
    else if (opcode == "LDR" || opcode == "STR")
    {
        if (tokens.size() != 4)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
        bool ok;
        int offset = tokens[3].toInt(&ok);
        if (!ok || offset < -32 || offset > 31)
            return false;
    }
    else if (opcode == "NOT")
    {
        if (tokens.size() != 3)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
    }
    else if (opcode == "RET" || opcode == "HALT" || opcode == "END")
    {
        if (tokens.size() != 1)
            return false;
    }
    else if (opcode == "WORD" || opcode == "BYTE")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toUInt(&ok);
        if (!ok)
            return false;
    }
    else if (opcode == "DEC")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toInt(&ok);
        if (!ok)
            return false;
    }

    else if (opcode == "HEX")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toUInt(&ok, 16); // Validate hex value
        if (!ok)
            return false;
    }
    else
    {
        QMessageBox::critical(nullptr, "Error", "Invalid opcode: " + opcode);
        return false;
    }

    return true;
}


QString trim(const QString &str)
{
   return str.trimmed();
}

QString toBinary(int value, int bits)
{
    if (value < 0)
    {
        value = (1 << bits) + value; // Calculate two's complement for negative values
        return QString::fromStdString(std::bitset<16>(value).to_string()).right(bits).rightJustified(bits, '1');
    }
    else
    {
        return QString::fromStdString(std::bitset<16>(value).to_string()).right(bits).rightJustified(bits, '0');
    }
}
