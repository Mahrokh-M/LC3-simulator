#ifndef LC3REGISTERS_H
#define LC3REGISTERS_H

#include <cstdint>

class LC3Registers
{
public:
    LC3Registers();

    // Register getters and setters
    uint16_t getPC() const;
    void setPC(uint16_t value);

    uint16_t getIR() const;
    void setIR(uint16_t value);

    uint16_t getCC() const;
    void setCC(uint16_t value);

    uint16_t getR(uint8_t index) const;
    void setR(uint8_t index, uint16_t value);

    uint16_t getMAR() const;
    void setMAR(uint16_t value);

    uint16_t getMDR() const;
    void setMDR(uint16_t value);

private:
    uint16_t PC;        // Program Counter
    uint16_t IR;        // Instruction Register
    uint16_t CC;        // Condition Code Register
    uint16_t R[8];      // General-purpose registers R0-R7
    uint16_t MAR;       // Memory Address Register
    uint16_t MDR;       // Memory Data Register
};

#endif // LC3REGISTERS_H
