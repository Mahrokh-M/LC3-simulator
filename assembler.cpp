#include "assembler.h"
#include"AssemblerLogic.cpp"

// Define globalFile as an instance of binFile class, assuming it's declared properly elsewhere
FileReadWrite BinFile("MEMORY.bin");



int startAssembly(QString &assemblyCode) {
    // Convert assembly code from QString to QVector<QString> for compatibility with existing functions
    QVector<QString> codeLines = assemblyCode.split("\n", Qt::SkipEmptyParts).toVector();

    if (codeLines.isEmpty()) {
        QMessageBox::warning(nullptr, "No Code Provided", "No code provided for assembly. Exiting...");
        return 1; // Return error code
    }

    QMap<QString, uint16_t> labels = firstPass(codeLines);
    LC3Memory tempMemory(0xFFFF); // Create memory with size 0xFFFF (64KB)
    secondPass(codeLines, labels, tempMemory);

    // Assuming globalFile is an instance of a custom class that handles file operations
    BinFile.writeToFile(tempMemory, 0x3000, 0x3000 + codeLines.size() - 1);

    QMessageBox::information(nullptr, "Assembly Completed", "Assembly completed. Output written to MEMORY.bin");

    return 0;
}
