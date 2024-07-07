#include"AssemblerLogic.h"
#include "qregularexpression.h"




QVector<QString> readLinesFromFile(const QString &filename)
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
        lines.append(line.trimmed());
    }
    file.close(); // Always close the file after reading
    return lines;
}

//labels and their corresponding memory addresses
QMap<QString, uint16_t> processLabels(const QVector<QString> &lines)
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

// Main function to assemble instructions from lines and write to memory
void assembleInstructionSetA(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, LC3Memory &memory)
{
    uint16_t address = 0x3000; // Starting address

    for (const QString &line : lines)
    {
        if (line.isEmpty() || line.startsWith(';'))
            continue; // Ignore empty lines and comments

        QRegularExpression re("\\s+");
        QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);

        if (tokens.size() > 1 && tokens[0] == "ORG")
        {
            if (!parseOrgDirective(tokens, address))
                continue;
        }
        else if (tokens.size() >= 1 && tokens[0] == "END")
        {
            break; // End of the program
        }
        else if (tokens[0].endsWith(','))
        {
            handleLabelledInstruction(line, address, labels, memory);
        }
        else
        {
            QVector<QString> tokens2 = splitLine(line, ' ');
            if (validateInstructionFormat(tokens2, labels))
            {
                processInstruction(line, address, labels, memory);
            }
            else
            {
                QMessageBox::critical(nullptr, "Error", "Skipping invalid instruction: " + line);
            }
        }
    }
}

// Function to convert an instruction line to binary
QString assembleInstructionSetB(const QString &instruction, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QVector<QString> tokens = splitInstruction(instruction);
    QString opcode = tokens[0];

    if (opcode == "ADD") return assembleAdd(tokens);
    if (opcode == "AND") return assembleAnd(tokens);
    if (opcode.startsWith("BR")) return assembleBr(tokens, labels, currentAddress);
    if (opcode == "JMP") return assembleJmp(tokens);
    if (opcode == "JSR") return assembleJsr(tokens, labels, currentAddress);
    if (opcode == "JSRR") return assembleJsrr(tokens);
    if (opcode == "LD") return assembleLd(tokens, labels, currentAddress);
    if (opcode == "LDI") return assembleLdi(tokens, labels, currentAddress);
    if (opcode == "LDR") return assembleLdr(tokens);
    if (opcode == "LEA") return assembleLea(tokens, labels, currentAddress);
    if (opcode == "NOT") return assembleNot(tokens);
    if (opcode == "RET") return assembleRet();
    if (opcode == "ST") return assembleSt(tokens, labels, currentAddress);
    if (opcode == "STI") return assembleSti(tokens, labels, currentAddress);
    if (opcode == "STR") return assembleStr(tokens);
    if (opcode == "HALT") return assembleHalt();
    if (opcode == "DEC") return assembleDec(tokens);
    if (opcode == "HEX") return assembleHex(tokens);

    qWarning() << "Unknown opcode:" << opcode;
    return "";
}

// Function to parse ORG directive and set the address
bool parseOrgDirective(const QVector<QString> &tokens, uint16_t &address)
{
    QString addrString = tokens[1];
    bool ok;
    uint16_t newAddress = static_cast<uint16_t>(addrString.toInt(&ok, 16));
    if (!ok)
    {
        QMessageBox::critical(nullptr, "Error", "Error converting address: " + addrString);
        return false;
    }
    address = newAddress; // Set starting address
    qDebug() << "Setting address to:" << address;
    return true;
}

// Function to process an instruction line and write it to memory
bool processInstruction(const QString &line, uint16_t &address, const QMap<QString, uint16_t> &labels, LC3Memory &memory)
{
    QString binaryInstruction = assembleInstructionSetB(line, labels, address);
    bool ok;
    uint16_t machineCode = static_cast<uint16_t>(binaryInstruction.toUInt(&ok, 2));
    if (!ok)
    {
        QMessageBox::critical(nullptr, "Error", "Failed to convert binary instruction to machine code");
        return false;
    }
    memory.write(address, machineCode); // Write machine code to memory
    address++;                          // Increment address
    return true;
}


// Function to handle labelled instructions
bool handleLabelledInstruction(const QString &line, uint16_t &address, const QMap<QString, uint16_t> &labels, LC3Memory &memory)
{
    QString instruction = line.mid(line.indexOf(',') + 1).trimmed();
    QVector<QString> tokens = splitLine(instruction, ' ');
    if (validateInstructionFormat(tokens, labels))
    {
        return processInstruction(instruction, address, labels, memory);
    }
    else
    {
        QMessageBox::warning(nullptr, "Warning", "Skipping invalid instruction: " + line);
        return false;
    }
}

QVector<QString> splitInstruction(const QString &instruction)
{
    QRegularExpression re("\\s+");
    QVector<QString> tokens = instruction.split(re, Qt::SkipEmptyParts);
    for (int i = 0; i < tokens.size(); ++i)
    {
        tokens[i] = tokens[i].remove(',').trimmed(); // Remove commas and trim tokens
    }
    return tokens;
}


QString assembleAdd(const QVector<QString> &tokens)
{
    QString dr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    QString sr1 = intToBinaryString(tokens[2].mid(1).toInt(), 3);
    if (tokens[3].startsWith('R'))
    {
        QString sr2 = intToBinaryString(tokens[3].mid(1).toInt(), 3);
        return "0001" + dr + sr1 + "000" + sr2;
    }
    else
    {
        int imm5 = tokens[3].mid(1).toInt(); // Remove the '#' and convert to integer
        return "0001" + dr + sr1 + "1" + intToBinaryString(imm5, 5);
    }
}

QString assembleAnd(const QVector<QString> &tokens)
{
    QString dr = intToBinaryString(tokens[1].mid(1).toInt(), 3);  // Destination register
    QString sr1 = intToBinaryString(tokens[2].mid(1).toInt(), 3); // Source register 1

    if (tokens[3].startsWith('R'))
    {
        // Register-to-register AND operation
        QString sr2 = intToBinaryString(tokens[3].mid(1).toInt(), 3); // Source register 2
        return "0101" + dr + sr1 + "000" + sr2;
    }
    else
    {
        // Immediate AND operation
        int imm5 = tokens[3].mid(1).toInt(); // Immediate value
        return "0101" + dr + sr1 + "1" + intToBinaryString(imm5, 5);
    }
}

QString assembleBr(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QString n = tokens[0].contains('n') ? "1" : "0"; // Check if 'n' condition flag is present
    QString z = tokens[0].contains('z') ? "1" : "0"; // Check if 'z' condition flag is present
    QString p = tokens[0].contains('p') ? "1" : "0"; // Check if 'p' condition flag is present

    int offset = labels.value(tokens[1]) - currentAddress - 1;
    QString offsetBinary = intToBinaryString(offset, 9); // This will handle both positive and negative offsets
    return "0000" + n + z + p + offsetBinary;
}

QString assembleJmp(const QVector<QString> &tokens)
{
    QString BaseR = tokens[1];
    int BaseRNum = BaseR.mid(1).toInt(); // Remove 'R' and convert to int

    // Construct binary instruction
    QString binaryInstruction = "1100000";
    binaryInstruction += intToBinaryString(BaseRNum, 3);
    binaryInstruction += "000000";

    return binaryInstruction;
}

QString assembleJsr(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QString label = tokens[1];
    uint16_t subroutineAddress = labels.value(label);

    // Calculate the offset from current address
    int16_t offset = subroutineAddress - currentAddress - 1;

    // Convert offset to binary with 11 bits
    QString offsetBinary = intToBinaryString(offset, 11);

    // Construct the binary instruction
    QString binaryInstruction = "0100";
    binaryInstruction += "1";
    binaryInstruction += offsetBinary;

    return binaryInstruction;
}

QString assembleJsrr(const QVector<QString> &tokens)
{
    QString BaseR = tokens[1];
    int BaseRNum = BaseR.mid(1).toInt();

    // Construct binary instruction
    QString binaryInstruction = "0100000";
    binaryInstruction += intToBinaryString(BaseRNum, 3);
    binaryInstruction += "000000";

    return binaryInstruction;
}

QString assembleLd(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QString dr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    int offset = labels.value(tokens[2]) - currentAddress - 1;
    return "0010" + dr + intToBinaryString(offset, 9);
}

QString assembleLdi(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QString dr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    int offset = labels.value(tokens[2]) - currentAddress - 1;
    return "1010" + dr + intToBinaryString(offset, 9);
}

QString assembleLdr(const QVector<QString> &tokens)
{
    QString dr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    QString baser = intToBinaryString(tokens[2].mid(1).toInt(), 3);
    int offset = tokens[3].mid(1).toInt();
    return "0110" + dr + baser + intToBinaryString(offset, 6);
}

QString assembleLea(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QString dr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    int offset = labels.value(tokens[2]) - currentAddress - 1;
    return "1110" + dr + intToBinaryString(offset, 9);
}

QString assembleNot(const QVector<QString> &tokens)
{
    QString dr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    QString sr = intToBinaryString(tokens[2].mid(1).toInt(), 3);
    return "1001" + dr + sr + "111111";
}

QString assembleRet()
{
    return "1100000111000000";
}

QString assembleSt(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QString sr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    int offsetValue = labels.value(tokens[2]) - currentAddress - 1;
    QString offset = intToBinaryString(offsetValue, 9);
    return "0011" + sr + offset;
}

QString assembleSti(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    QString sr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    QString offset = intToBinaryString(labels.value(tokens[2]) - currentAddress - 1, 9);
    return "1011" + sr + offset;
}

QString assembleStr(const QVector<QString> &tokens)
{
    QString sr = intToBinaryString(tokens[1].mid(1).toInt(), 3);
    QString baseR = intToBinaryString(tokens[2].mid(1).toInt(), 3);
    int offset6 = tokens[3].mid(1).toInt();
    return "0111" + sr + baseR + intToBinaryString(offset6, 6);
}

QString assembleHalt()
{
    return "1111000000100101"; // TRAP x25
}

QString assembleDec(const QVector<QString> &tokens)
{
    bool ok;
    int16_t value = tokens[1].toInt(&ok);
    if (!ok)
    {
        qWarning() << "Invalid .DEC value:" << tokens[1];
        return "";
    }
    return intToBinaryString(value, 16);
}

QString assembleHex(const QVector<QString> &tokens)
{
    bool ok;
    uint16_t value = tokens[1].toUInt(&ok, 16);
    if (!ok)
    {
        qWarning() << "Invalid .HEX value:" << tokens[1];
        return "";
    }
    return intToBinaryString(value, 16);
}

// Function to split a line into tokens
QVector<QString> splitLine(const QString &line, const QChar &separator)
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

// Function to validate the instruction format
bool validateInstructionFormat(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels)
{
    if (tokens.isEmpty())
        return false;

    QString opcode = tokens[0].trimmed();

    auto isRegister = [](const QString &token)
    {
        return token.size() >= 2 && token.startsWith('R') && token.at(1).isDigit() && token.mid(1).toInt() >= 0 && token.mid(1).toInt() <= 7;
    };

    if ((opcode == "ADD" || opcode == "AND") && tokens.size() == 4)
    {
        return isRegister(tokens[1]) && isRegister(tokens[2]) &&
               (tokens[3].startsWith('R') ? isRegister(tokens[3]) : tokens[3].mid(1).toInt() >= -16 && tokens[3].mid(1).toInt() <= 15);
    }
    else if (opcode.startsWith("BR") && tokens.size() == 2)
    {
        return labels.contains(tokens[1]);
    }
    else if ((opcode == "JMP" || opcode == "JSRR") && tokens.size() == 2)
    {
        return isRegister(tokens[1]);
    }
    else if (opcode == "JSR" && tokens.size() == 2)
    {
        return labels.contains(tokens[1]);
    }
    else if ((opcode == "LD" || opcode == "LDI" || opcode == "LEA" || opcode == "ST" || opcode == "STI") && tokens.size() == 3)
    {
        return isRegister(tokens[1]) && labels.contains(tokens[2]);
    }
    else if ((opcode == "LDR" || opcode == "STR") && tokens.size() == 4)
    {
        return isRegister(tokens[1]) && isRegister(tokens[2]) &&
               tokens[3].toInt() >= -32 && tokens[3].toInt() <= 31;
    }
    else if (opcode == "NOT" && tokens.size() == 3)
    {
        return isRegister(tokens[1]) && isRegister(tokens[2]);
    }
    else if ((opcode == "RET" || opcode == "HALT" || opcode == "END") && tokens.size() == 1)
    {
        return true;
    }
    else if ((opcode == "WORD" || opcode == "BYTE") && tokens.size() == 2)
    {
        bool ok;
        tokens[1].toUInt(&ok);
        return ok;
    }
    else if (opcode == "DEC" && tokens.size() == 2)
    {
        bool ok;
        tokens[1].toInt(&ok);
        return ok;
    }
    else if (opcode == "HEX" && tokens.size() == 2)
    {
        bool ok;
        tokens[1].toUInt(&ok, 16);
        return ok;
    }
    else
    {
        QMessageBox::critical(nullptr, "Error", "Invalid opcode: " + opcode);
        return false;
    }
}


QString intToBinaryString(int value, int bits)
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
