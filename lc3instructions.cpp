#include "lc3instructions.h"
#include <cstdint>
#include "Logic.h"
uint16_t ir, nzp, dr, sr1, imm_flag, sr2, imm5, base_r, flag, opcode, address, v_sr1, v_sr2, GateALU, value, sr;
int16_t offset9, offset6, offset11;

void LC3Instructions::updateFlags(uint16_t result){
    if (result == 0)
    {
        registers.setCC(0x02); // Set condition code to Zero
    }
    else if (result >> 15)
    {
        registers.setCC(0x04); // Set condition code to Negative
    }
    else
    {
        registers.setCC(0x01); // Set condition code to Positive
    }
}

void LC3Instructions::fetch(LC3Memory& memory)
{
    uint16_t pc = registers.getPC();
    registers.setMAR(pc);
    registers.setMDR(memory.read(pc));
    registers.setPC(pc + 1);
    registers.setIR(registers.getMDR());
}

void LC3Instructions::decode()
{
    opcode = (registers.getIR() >> 12) & 0xF;

    if (opcode == 0x0)
    {
        // BR instruction
        ir = registers.getIR();
        nzp = (ir >> 9) & 0x7; // Extract nzp bits
        offset9 = ir & 0x1FF; // Extract PCoffset9 (9-bit)

        // Sign-extend offset9 to 16 bits if necessary
        if (offset9 & 0x100)
        {
            offset9 |= 0xFE00; // Sign extend to the left
        }
    }
    else if (opcode == 0x1)
    {
        // ADD instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x7;       // Destination register
        sr1 = (ir >> 6) & 0x7;      // Source register 1
        imm_flag = (ir >> 5) & 0x1; // Immediate flag

        if (imm_flag)
        {
            // Immediate mode
            imm5 = ir & 0x1F; // Extract immediate value (5 bits)

            // Sign-extend imm5 to 16 bits
            if (imm5 & 0x10)
            {
                imm5 |= 0xFFE0; // Sign extend to the left
            }
        }
        else
        {
            // Register mode
            sr2 = ir & 0x7; // Source register 2
        }
    }
    else if (opcode == 0x2)
    {
        // LD instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x0007;
        offset9 = ir & 0x01FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
    else if (opcode == 0xA)
    {
        // LDI instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x0007;
        offset9 = ir & 0x01FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
    else if (opcode == 0x6)
    {
        // LDR instruction
        ir = registers.getIR();
        offset6 = ir & 0x003F;
        if (offset6 & 0x0020) // Sign extension
        {
            offset6 |= 0xFFC0;
        }
        base_r = (ir >> 6) & 0x0007;
        dr = (ir >> 9) & 0x0007;
    }
    else if (opcode == 0xE)
    {
        // LEA instruction
        ir = registers.getIR();
        offset9 = ir & 0x1FF;
        if (offset9 & 0x100)
        {
            offset9 |= 0xFE00;
        }

        dr = (ir >> 9) & 0x0007;
    }
    else if (opcode == 0x3)
    {
        // ST instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x7;
        offset9 = ir & 0x1FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
    else if (opcode == 0xB)
    {
        // STI instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x7;
        offset9 = ir & 0x1FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
    else if (opcode == 0x7)
    {
        // STR instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x7;
        base_r = (ir >> 6) & 0x7;
        offset6 = ir & 0x3F;
        if (offset6 & 0x0020) // Sign extension
        {
            offset6 |= 0xFFC0;
        }
    }
    else if (opcode == 0x4)
    {
        // JSR or JSRR instruction
        ir = registers.getIR();
        flag = (ir >> 11) & 0x1;
        if (flag)
        {
            // JSR
            offset11 = ir & 0x7FF; // Extract PCoffset11
            if (offset11 & 0x0400) // Sign extension
            {
                offset11 |= 0xF800;
            }
        }
        else
        {
            // JSRR
            base_r = (ir >> 6) & 0x7;
        }
    }
    else if (opcode == 0x5)
    {
        // AND instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x7;
        sr1 = (ir >> 6) & 0x7;
        imm_flag = (ir >> 5) & 0x1;

        if (imm_flag)
        {
            // Immediate mode
            imm5 = ir & 0x1F;

            // Sign-extend imm5 to 16 bits
            if (imm5 & 0x10)
            {
                imm5 |= 0xFFE0; // Sign extend to the left
            }
        }
        else
        {
            // Register mode
            sr2 = ir & 0x7; // Source register 2
        }
    }
    else if (opcode == 0xC)
    {
        // RET or JMP instruction
        ir = registers.getIR();
        if (((ir >> 6) & 0x7) == 7)
        {
            // RET
            address = registers.getR(7); // Set PC to the value contained in R7
        }
        else
        {
            // JMP
            base_r = (ir >> 6) & 0x7;
        }
    }
    else if (opcode == 0x9)
    {
        // NOT instruction
        ir = registers.getIR();
        dr = (ir >> 9) & 0x7; // Destination register
        sr = (ir >> 6) & 0x7; // Source register
    }
}

void LC3Instructions::evaluateAddress(LC3Memory &memory)
{
    if (opcode == 0x0)
    {
        // BR instruction
        address = registers.getPC() + offset9;
    }
    else if (opcode == 0x2)
    {
        // LD instruction
        address = registers.getPC() + offset9;
        registers.setMAR(address); // Set the address in MAR
    }
    else if (opcode == 0x4)
    {
        // JSR instruction
        if (flag)
        {
            address = registers.getPC() + offset11; // Update PC with the offset
        }
        else
        {
            // JSRR instruction
            address = registers.getR(base_r);
        }
    }
    else if (opcode == 0xA)
    {
        // LDI instruction
        address = registers.getPC() + offset9;
        registers.setMAR(address);
        address = memory.read(registers.getMAR());
        registers.setMAR(address);
    }
    else if (opcode == 0x6)
    {
        // LDR instruction
        address = registers.getR(base_r) + offset6;
        registers.setMAR(address);
    }
    else if (opcode == 0xC)
    {
        // RET or JMP instruction
        if (((registers.getIR() >> 6) & 0x7) == 7)
        {
            // RET
            address = registers.getR(7); // Set PC to the value contained in R7
        }
        else
        {
            // JMP
            address = registers.getR(base_r);
        }
    }
    else if (opcode == 0xE)
    {
        // LEA instruction
        address = registers.getPC() + offset9;
    }
    else if (opcode == 0x3)
    {
        // ST instruction
        address = registers.getPC() + offset9;
    }
    else if (opcode == 0xB)
    {
        // STI instruction
        address = registers.getPC() + offset9;
    }
    else if (opcode == 0x7)
    {
        // STR instruction
        address = registers.getR(base_r) + offset6;
    }
}

void LC3Instructions::fetchOperands(LC3Memory &memory)
{
    if (opcode == 0x1)
    {
        // ADD instruction
        v_sr1 = registers.getR(sr1);
        v_sr2 = registers.getR(sr2);
    }
    else if (opcode == 0x5)
    {
        // AND instruction
        v_sr1 = registers.getR(sr1);
        v_sr2 = registers.getR(sr2);
    }
    else if (opcode == 0x9)
    {
        // NOT instruction
        v_sr1 = registers.getR(sr);
    }
    else if (opcode == 0x2)
    {
        // LD instruction
        registers.setMDR(memory.read(registers.getMAR()));
    }
    else if (opcode == 0xA)
    {
        // LDI instruction
        registers.setMDR(memory.read(registers.getMAR()));
    }
    else if (opcode == 0x6)
    {
        // LDR instruction
        registers.setMDR(memory.read(registers.getMAR()));
    }
    else if (opcode == 0x3)
    {
        // ST instruction
        value = registers.getR(dr);
    }
    else if (opcode == 0xB)
    {
        // STI instruction
        value = registers.getR(dr);
    }
    else if (opcode == 0x7)
    {
        // STR instruction
        value = registers.getR(dr);
    }
}

void LC3Instructions::execute()
{
    uint16_t opcode = (registers.getIR() >> 12) & 0xF;

    if (opcode == 0x1)
    {
        // ADD instruction
        if (imm_flag)
        {
            // Immediate mode
            GateALU = v_sr1 + static_cast<int16_t>(imm5);
        }
        else
        {
            // Register mode
            GateALU = v_sr1 + v_sr2;
        }
    }
    else if (opcode == 0x5)
    {
        // AND instruction
        if (imm_flag)
        {
            // Immediate mode
            GateALU = registers.getR(sr1) & static_cast<int16_t>(imm5);
        }
        else
        {
            // Register mode
            GateALU = registers.getR(sr1) & registers.getR(sr2);
        }
    }
    else if (opcode == 0x9)
    {
        // NOT instruction
        GateALU = ~v_sr1; // Bitwise NOT operation
    }
}

void LC3Instructions::store(LC3Memory &memory)
{
    if (opcode == 0x0)
    { // BR instruction
        // Get current condition codes
        uint16_t cc = registers.getCC();

        // Check if any of the conditions are met
        bool condition_met = ((nzp & 0x4) && (cc & 0x4)) || // n bit
                             ((nzp & 0x2) && (cc & 0x2)) || // z bit
                             ((nzp & 0x1) && (cc & 0x1));   // p bit

        if (condition_met)
        {
            // Update PC with the offset if the condition is met
            registers.setPC(address);
        }
    }
    else if (opcode == 0x1)
    { // ADD instruction
        // Set condition codes
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        updateFlags(result);
    }
    else if (opcode == 0x2)
    { // LD instruction
        // Read value from MDR
        uint16_t value = registers.getMDR();

        // Store value in destination register
        registers.setR(dr, value);

        // Update condition codes
        updateFlags(value);
    }
    else if (opcode == 0xA)
    { // LDI instruction
        uint16_t value = registers.getMDR();

        // Update condition codes
        registers.setR(dr, value);
        updateFlags(value);
    }
    else if (opcode == 0x6)
    { // LDR instruction
        uint16_t value = registers.getMDR();

        // Update condition codes
        registers.setR(dr, value);
        updateFlags(value);
    }
    else if (opcode == 0xE)
    { // LEA instruction
        // Store address in destination register
        registers.setR(dr, address);
    }
    else if (opcode == 0x3)
    { // ST instruction
        // Set the address in MAR
        registers.setMAR(address);

        // Set the value to be stored in MDR
        registers.setMDR(value);

        // Store the value in the DR to the computed address
        memory.write(registers.getMAR(), registers.getMDR());
    }
    else if (opcode == 0xB)
    { // STI instruction
        // Set the address in MAR
        registers.setMAR(address);

        // Set the value to be stored in MDR
        registers.setMDR(value);

        // Store the value in the SR to the memory at the address pointed to by the computed address
        memory.write(memory.read(registers.getMAR()), registers.getMDR());
    }
    else if (opcode == 0x7)
    { // STR instruction
        // Set the address in MAR
        registers.setMAR(address);

        // Set the value to be stored in MDR
        registers.setMDR(value);

        // Store the value in the SR to the computed address
        memory.write(registers.getMAR(), registers.getMDR());
    }
    else if (opcode == 0x4)
    { // JSR or JSRR instruction
        uint16_t currentPC = registers.getPC(); // Get the current PC

        // Store the current PC in R7
        registers.setR(7, currentPC);

        // Update PC with the offset
        registers.setPC(address);
    }
    else if (opcode == 0x5)
    { // AND instruction
        // Set condition codes
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        updateFlags(result);
    }
    else if (opcode == 0xC)
    { // RET or JMP instruction
        registers.setPC(address);
    }
    else if (opcode == 0x9)
    { // NOT instruction
        // Set condition codes
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        updateFlags(result);
    }
}

bool LC3Instructions::isHalt(){
    return (registers.getMDR() == 0xF025);
}

