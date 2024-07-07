# LC3 Simulator

## Overview

This project implements a simulator for the LC3 (Little Computer 3) architecture. The simulator includes a graphical user interface (GUI) for uploading and assembling LC3 assembly code, managing memory, and executing instructions. The project is organized into several components, each handling specific aspects of the LC3 simulation.

## Features

- **Upload and Assemble**: Upload LC3 assembly code and assemble it into machine code.
- **Memory Management**: Manage and visualize LC3 memory contents.
- **Instruction Execution**: Execute LC3 instructions step-by-step.
- **File Operations**: Read from and write to files to load/save LC3 memory states.
- **Assembler**: Convert LC3 assembly code to machine code.

## Installation

### Prerequisites

- Qt framework installed
- C++ compiler

### Building and Running

1. **Clone the repository:**

2. **Open in Qt Creator:**
- Open the project in Qt Creator.
- Configure the project to ensure the Qt environment is set up correctly.

3. **Build and Run:**
- Build the project using Qt Creator.
- Run the compiled binary to start the LC3 simulator.

### Alternatively, you can also install it using the installer provided, without the need to install Qt creator or C++ compiler.

## Usage

1. **Upload Code**: Click the "Upload Code" button to upload your LC3 code.
2. **Assemble Code**: Click the "Assemble" button to assemble the uploaded code.
3. **Reset**: Click the "Reset" button to reset the simulator.
4. **Next Cycle**: Click the "Next Cycle" button to execute the next instruction cycle.
5. **Sample Code**: Click the "Sample Code" button to load a sample LC3 code.
6. **Write Code**: Write your LC3 code in text edit instead of uploading a file.

The GUI provides tables to display register values, memory contents, and flags, allowing you to monitor the state of the LC3 machine as you step through your code.

## Files and Directories

- `Logic.h`: Main application logic and user interface interaction.
- `lc3registers.h`: Definitions for LC3 CPU registers.
- `lc3memory.h`: Management of LC3 memory operations.
- `lc3instructions.h`: Implementation of the LC3 instruction set.
- `FileReadWrite.h`: File operations for reading and writing.
- `assemblerlogic.h`: Logic for assembling LC3 assembly code.
- `assembler.h`: Assembly process management.

## Classes and Functions

### Logic Class

Handles the main application logic and UI interactions.

#### Public Methods

- `Logic(QWidget *parent = nullptr)`: Constructor.
- `~Logic()`: Destructor.

#### Private Slots

- `on_Upload_code_clicked()`: Handles uploading of LC3 code.
- `on_ASSEMBLE_clicked()`: Assembles uploaded LC3 code.
- `on_Reset_clicked()`: Resets the LC3 simulator.
- `on_nextCycle_clicked()`: Executes the next instruction cycle.
- `on_SampleCode_clicked()`: Loads a sample LC3 code.

### LC3Registers Class

Manages the LC3 CPU registers.

#### Public Methods

- `LC3Registers()`: Constructor.
- `uint16_t getPC() const`: Gets the program counter.
- `void setPC(uint16_t value)`: Sets the program counter.
- `uint16_t getIR() const`: Gets the instruction register.
- `void setIR(uint16_t value)`: Sets the instruction register.
- `uint16_t getCC() const`: Gets the condition code register.
- `void setCC(uint16_t value)`: Sets the condition code register.
- `uint16_t getR(uint8_t index) const`: Gets a general-purpose register.
- `void setR(uint8_t index, uint16_t value)`: Sets a general-purpose register.
- `uint16_t getMAR() const`: Gets the memory address register.
- `void setMAR(uint16_t value)`: Sets the memory address register.
- `uint16_t getMDR() const`: Gets the memory data register.
- `void setMDR(uint16_t value)`: Sets the memory data register.

### LC3Memory Class

Manages the LC3 memory operations.

#### Public Methods

- `LC3Memory(uint16_t size)`: Constructor.
- `uint16_t read(uint16_t address) const`: Reads from a memory address.
- `void write(uint16_t address, uint16_t value)`: Writes to a memory address.

### LC3Instructions Class

Implements the LC3 instruction set.

#### Public Static Methods

- `fetch(LC3Memory& memory)`: Fetches the next instruction.
- `decode()`: Decodes the fetched instruction.
- `evaluateAddress(LC3Memory& memory)`: Evaluates the address for the instruction.
- `fetchOperands(LC3Memory& memory)`: Fetches the operands for the instruction.
- `execute()`: Executes the instruction.
- `store(LC3Memory &memory)`: Stores the result of the instruction.
- `updateFlags(uint16_t result)`: Updates the condition flags based on the result.
- `isHalt()`: Checks if the halt instruction is encountered.

### FileReadWrite Class

Handles file operations for reading from and writing to files.

#### Public Methods

- `FileReadWrite()`: Default constructor.
- `FileReadWrite(QString)`: Constructor with file name.
- `writeToFile(const LC3Memory&, uint16_t, uint16_t)`: Writes memory content to a file.
- `readFromFile(uint16_t)`: Reads memory content from a file.

### Assembler Logic Functions

Logic for assembling LC3 assembly code.

#### Public Functions

- `readLinesFromFile(const QString &filename)`: Reads lines from a file.
- `processLabels(const QVector<QString> &lines)`: Processes labels in the code.
- `assembleInstructionSetA(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, LC3Memory &memory)`: Assembles instructions of set A.
- `assembleInstructionSetB(const QString &instruction, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)`: Assembles instructions of set B.
- `splitLine(const QString &line, const QChar &separator)`: Splits a line into tokens.
- `validateInstructionFormat(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels)`: Validates the instruction format.
- `intToBinaryString(int value, int bits)`: Converts an integer to a binary string.
- `splitInstruction(const QString &instruction)`: Splits an instruction into tokens.
- `parseOrgDirective(const QVector<QString> &tokens, uint16_t &address)`: Parses the ORG directive.
- `processInstruction(const QString &line, uint16_t &address, const QMap<QString, uint16_t> &labels, LC3Memory &memory)`: Processes an instruction line.
- `handleLabelledInstruction(const QString &line, uint16_t &address, const QMap<QString, uint16_t> &labels, LC3Memory &memory)`: Handles labelled instructions.

### Assembler Class

Manages the assembly process.

#### Public Functions

- `startAssembly(QString &inputFilename)`: Starts the assembly process for the given input file.
## Implementation Details

### Project Structure

The project is structured into several key components that manage different aspects of the LC3 simulation:

- **Logic**: Handles the main application logic and user interface interactions.
- **LC3Registers**: Manages the LC3 CPU registers.
- **LC3Memory**: Manages the LC3 memory operations.
- **LC3Instructions**: Implements the LC3 instruction set including fetch, decode, execute operations.
- **FileReadWrite**: Handles file operations for reading from and writing to files.
- **AssemblerLogic**: Logic for assembling LC3 assembly code into machine code.
- **Assembler**: Manages the assembly process.

### Algorithms and Methods

- **Assembly Process**: The assembler parses the assembly code, resolves labels, and converts instructions into machine code using a two-pass algorithm. It validates instruction formats and processes directives like ORG to correctly place instructions in memory.

- **Instruction Execution**: Instructions are fetched from memory, decoded, and executed in accordance with the LC3 architecture. Each instruction set (A and B) is handled separately to ensure accurate operation.

- **Memory Management**: LC3 memory is managed using a dedicated class that provides read and write operations, ensuring efficient memory access during program execution.

### Design Patterns

- **MVC Architecture**: The project follows a Model-View-Controller architecture where the logic (controller) interacts with the GUI (view) and manages data through models (registers, memory).

### Challenges and Solutions

- **Assembler Optimization**: Initially, assembler performance was a concern due to large file sizes. Implementing optimizations such as caching label addresses and using efficient data structures helped improve assembly speed.

- **GUI Responsiveness**: Ensuring the GUI remained responsive during intensive operations like instruction execution required careful thread management and event handling.

### Future Improvements

- **Debugger Functionality**: Introduce debugging tools such as breakpoints, variable inspection, and step-by-step execution.
  
- **Enhanced Memory Visualization**: Improve the memory viewer to display memory contents in various formats (hexadecimal, ASCII, etc.) for better debugging and analysis.

### Usage

- Start the simulator from Qt Creator.
- Load an assembly file by clicking 'File' > 'Open' and selecting your .asm file.
- Assembel the code and execute it using the 'next cycle'.
- or write your code in text edit 😀



### Screenshots

![LC3 Simulator Screenshot](https://s8.uupload.ir/files/screenshot_2024-07-07_233932_n8b5.png)

## Conclusion

This LC3 Simulator project provides a comprehensive tool for simulating and executing LC3 assembly code. Whether you're learning about computer architecture or debugging complex programs, our simulator offers a robust platform with intuitive features and expandable functionality.
