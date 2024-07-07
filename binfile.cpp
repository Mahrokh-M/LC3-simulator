#include "binfile.h"


LC3Memory memory(0xFFFF); // Create memory with size 0xFFFF (64KB)

binFile::binFile()
{

}
binFile::binFile(QString filename)
{
    file.setFileName(filename);
}

// Function to write machine code to an output file
void binFile::writeToFile(const LC3Memory& memory, uint16_t startAddress, uint16_t endAddress) {
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << qPrintable("MEMORY.bin");
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_0); // Set the data stream version if necessary

    for (uint16_t address = startAddress; address <= endAddress; ++address) {
        uint16_t value = memory.read(address);
        out << value; // Write the machine code value to the binary file
    }

    file.close();
}

// Function to read instructions from a binary file and fill the memory
bool binFile::readFromFile(uint16_t startAddress) {
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << qPrintable("MEMORY.bin");
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_0); // Set the data stream version if necessary

    uint16_t address = startAddress;
    while (!in.atEnd()) {
        uint16_t value;
        in >> value; // Use the stream extraction operator to read a uint16_t value
        memory.write(address, value); // Write the value to memory at the current address
        address++;
    }

    file.close();
    return true;
}
