#include "lc3registers.h"

LC3Registers::LC3Registers()
{
    PC = 0;
    IR = 0;
    CC = 0;
    for (int i = 0; i < 8; ++i) {
        R[i] = 0;
    }
    MAR = 0;
    MDR = 0;
}

uint16_t LC3Registers::getPC() const
{
    return PC;
}

void LC3Registers::setPC(uint16_t value)
{
    PC = value;
}

uint16_t LC3Registers::getIR() const
{
    return IR;
}

void LC3Registers::setIR(uint16_t value)
{
    IR = value;
}

uint16_t LC3Registers::getCC() const
{
    return CC;
}

void LC3Registers::setCC(uint16_t value)
{
    CC = value;
}

uint16_t LC3Registers::getR(uint8_t index) const
{
    if (index < 8) {
        return R[index];
    } else {
        // Handle error or throw exception
        return 0;
    }
}

void LC3Registers::setR(uint8_t index, uint16_t value)
{
    if (index < 8) {
        R[index] = value;
    } else {
        // Handle error or throw exception
    }
}

uint16_t LC3Registers::getMAR() const
{
    return MAR;
}

void LC3Registers::setMAR(uint16_t value)
{
    MAR = value;
}

uint16_t LC3Registers::getMDR() const
{
    return MDR;
}

void LC3Registers::setMDR(uint16_t value)
{
    MDR = value;
}
