#include "FileReadWrite.h"
#include "Logic.h"



FileReadWrite::FileReadWrite(QString filename) {
    file.setFileName(filename);
}

void FileReadWrite::writeToFile(const LC3Memory &memory, uint16_t startAddress, uint16_t endAddress) {
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "Error", "Cannot open file for writing: MEMORY.bin");
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_0);

    for (uint16_t address = startAddress; address <= endAddress; ++address) {
        uint16_t value = memory.read(address);
        out << value;
    }

    file.close();
}

bool FileReadWrite::readFromFile(uint16_t startAddress) {
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Error", "Cannot open file for reading: MEMORY.bin");
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_0);

    uint16_t address = startAddress;
    while (!in.atEnd()) {
        uint16_t value;
        in >> value;
        memory.write(address, value);
        address++;
    }

    file.close();
    return true;
}
