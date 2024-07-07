#include "lc3instructions.h"
#include <cstdint>
#include "LC3.h"
uint16_t ir;
uint16_t nzp;
uint16_t dr;
uint16_t sr1;
uint16_t imm_flag;
uint16_t sr2;
uint16_t imm5;
int16_t offset9;
int16_t offset6;
int16_t offset11;
uint16_t base_r;
uint16_t flag;
uint16_t opcode;
uint16_t address;
uint16_t v_sr1;
uint16_t v_sr2;
uint16_t GateALU;
uint16_t value;
uint16_t sr;
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

    switch (opcode)
    {
    case 0x0:
        // BR
    {
        ir = registers.getIR();
        nzp = (ir >> 9) & 0x7; // Extract nzp bits
        offset9 = ir & 0x1FF;
        // Extract PCoffset9 (9-bit)
        if (offset9 & 0x100)
        {                      // Check if the sign bit (bit 8) is set
            offset9 |= 0xFE00; // Sign extend to the left
        }
    }

        break;
    case 0x1:
        // ADD
    {
        dr = (registers.getIR() >> 9) & 0x7;       // Destination register
        sr1 = (registers.getIR() >> 6) & 0x7;      // Source register 1
        imm_flag = (registers.getIR() >> 5) & 0x1; // Immediate flag

        if (imm_flag)
        {
            // Immediate mode
            imm5 = registers.getIR() & 0x1F; // Extract immediate value (5 bits)

            // Sign-extend imm5 to 16 bits
            if (imm5 & 0x10)
            {                   // Check if the sign bit (bit 4) is set
                imm5 |= 0xFFE0; // Sign extend to the left
            }
        }
        else
        {
            // Register mode
            sr2 = registers.getIR() & 0x7; // Source register 2
        }
    }

        break;
    case 0x2:
        // LD
    {
        dr = (registers.getIR() >> 9) & 0x0007;
        offset9 = registers.getIR() & 0x01FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
        break;
    case 0xA:
        // LDI
    {
        dr = (registers.getIR() >> 9) & 0x0007;
        offset9 = registers.getIR() & 0x01FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
        break;
    case 0x6:
        // LDR
    {
        offset6 = registers.getIR() & 0x003F;
        if (offset6 & 0x0020) // Sign extension
        {
            offset6 |= 0xFFC0;
        }
        base_r = (registers.getIR() >> 6) & 0x0007;
        dr = (registers.getIR() >> 9) & 0x0007;
    }
        break;
    case 0xE:
        // LEA
    {
        offset9 = ir & 0x1FF;
        if (offset9 & 0x100)
        {
            offset9 |= 0xFE00;
        }

        dr = (registers.getIR() >> 9) & 0x0007;
    }
        break;
    case 0x3:
        //ST
    {dr = (registers.getIR() >> 9) & 0x7;
        offset9 = registers.getIR() & 0x1FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
        break;
    case 0xB:
        //STI
    {
        dr = (registers.getIR() >> 9) & 0x7;
        offset9 = registers.getIR() & 0x1FF;
        if (offset9 & 0x0100) // Sign extension
        {
            offset9 |= 0xFE00;
        }
    }
        break;
    case 0x7:
        //STR
    {
        dr = (registers.getIR() >> 9) & 0x7;
        base_r = (registers.getIR() >> 6) & 0x7;
        offset6 = registers.getIR() & 0x3F;
        if (offset6 & 0x0020) // Sign extension
        {
            offset6 |= 0xFFC0;
        }
    }
        break;
    case 0x4:
        //JSR
    {
        flag = (registers.getIR() >> 11) & 0x1;
        if (flag){
            offset11 = registers.getIR() & 0x7FF; // Extract PCoffset11
            if (offset11 & 0x0400) // Sign extension
            {
                offset11 |= 0xF800;
            }
        }else{
            //JSRR
            base_r = (registers.getIR() >> 6) & 0x7;
        }
    }
        break;
    case 0x5:
        //AND
    {
        dr = (registers.getIR() >> 9) & 0x7;
        sr1 = (registers.getIR() >> 6) & 0x7;
        imm_flag = (registers.getIR() >> 5) & 0x1;

        if (imm_flag) {
            // Immediate mode
            imm5 = registers.getIR() & 0x1F;

            // Sign-extend imm5 to 16 bits
            if (imm5 & 0x10) { // Check if the sign bit (bit 4) is set
                imm5 |= 0xFFE0; // Sign extend to the left
            }
        }
        else{
            sr2 = registers.getIR() & 0x7; // Source register 2
        }
    }
        break;

    case 0xC:
    {
        if (((registers.getIR() >> 6) & 0x7) == 7) { // Check if it's a RET instruction
            //RET

        } else {
            //JMP
            base_r = (registers.getIR() >> 6) & 0x7;
        }
    }
        break;
    case 0x9:
        //NOT
    {
        dr = (registers.getIR() >> 9) & 0x7; // destination register
        sr = (registers.getIR() >> 6) & 0x7; // source register
    }
        break;
    default:
        // Handle unsupported opcode
        break;
    }
}
void LC3Instructions::evaluateAddress(LC3Memory &memory)
{

    switch (opcode) {
    case 0x0:
        //BR
        address=registers.getPC() + offset9;
    case 0x2:
        // LD
        address = registers.getPC() + offset9;
        // Set the address in MAR
        registers.setMAR(address);
        break;
    case 0x4:
        //JSR
        if (flag){
            address=registers.getPC() + offset11; // Update PC with the offset
        }else{
            //JSRR
            address=registers.getR(base_r);
        }
    case 0xA:
        // LDI
        address = registers.getPC() + offset9;
        registers.setMAR(address);
        address = memory.read(registers.getMAR());
        registers.setMAR(address);
        break;
    case 0x6:
        // LDR
        address = registers.getR(base_r) + offset6;
        registers.setMAR(address);
        break;
    case 0xC:
        //RET
    {
        if (((registers.getIR() >> 6) & 0x7) == 7) {
            // Set PC to the value contained in R7
            address=registers.getR(7);
        }
        //JMP
        else {
            address=registers.getR(base_r);
        }
    }
    case 0xE:
        // LEA
        address = registers.getPC() + offset9;
        break;
    case 0x3:
        // ST
        address = registers.getPC() + offset9;
        break;
    case 0xB:
        // STI
        address = registers.getPC() + offset9;
        break;
    case 0x7:
        // STR
        address = registers.getR(base_r) + offset6;
        break;
    }
}
void LC3Instructions::fetchOperands(LC3Memory &memory)
{
    switch (opcode)
    {
    case 0x1:
        // ADD
        v_sr1 = registers.getR(sr1);
        v_sr2 = registers.getR(sr2);
        break;
    case 0x5:
        // AND
        v_sr1 = registers.getR(sr1);
        v_sr2 = registers.getR(sr2);
        break;
    case 0x9:
        //NOT
        v_sr1=registers.getR(sr);
    case 0x2:
        // LD
        registers.setMDR(memory.read(registers.getMAR()));
        break;
    case 0xA:
        // LDI
        registers.setMDR(memory.read(registers.getMAR()));
        break;
    case 0x6:
        // LDR
        registers.setMDR(memory.read(registers.getMAR()));
        break;
    case 0x3:
        //ST
        value=registers.getR(dr);
        break;
    case 0xB:
        //STI
        value=registers.getR(dr);
        break;
    case 0x7:
        //STR
        value=registers.getR(dr);

    }
}
void LC3Instructions::execute()
{
    uint16_t opcode = (registers.getIR() >> 12) & 0xF;

    switch (opcode)
    {
    case 0x1:
        // ADD
        if (imm_flag)
        {
            // Immediate mode
            GateALU=v_sr1 + static_cast<int16_t>(imm5);
        } else {
            // Register mode
            GateALU=v_sr1 + v_sr2;
        }
        break;
    case 0x5:
        // AND
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
        break;
    case 0x9:
        //NOT
        GateALU=~v_sr1; // bitwise NOT operation
        break;
    }
}
void LC3Instructions::store(LC3Memory &memory)
{
    switch (opcode)
    {
    case 0x0:
    { // BR
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
        break;
    case 0x1:
        // ADD
    {
        // Set condition codes
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        if (result == 0)
        {
            registers.setCC(0x02); // Zero
        }
        else if (result >> 15)
        {
            registers.setCC(0x04); // Negative
        }
        else
        {
            registers.setCC(0x01); // Positive
        }
    }
        break;
    case 0x2:
        // LD
    {
        // Read value from MDR
        uint16_t value = registers.getMDR();
        // Store value in destination register
        registers.setR(dr, value);
        // Update condition codes
        if (value == 0)
        {
            registers.setCC(0x02); // Zero flag
        }
        else if (value & 0x8000)
        {
            registers.setCC(0x04); // Negative flag
        }
        else
        {
            registers.setCC(0x01); // Positive flag
        }
    }
        break;
    case 0xA:
        //LDI
    {
        uint16_t value = registers.getMDR();
        // Update condition codes
        registers.setR(dr, value);
        if (value == 0)
        {
            uint16_t value = registers.getMDR();
            // Update condition codes
            if (value == 0)
            {
                registers.setCC(0x02); // Zero flag
            }
            else if (value & 0x8000)
            {
                registers.setCC(0x04); // Negative flag
            }
            else
            {
                registers.setCC(0x01); // Positive flag
            }
        }
    }
        break;
    case 0x6:
        //LDR
    {
        uint16_t value = registers.getMDR();
        // Update condition codes
        registers.setR(dr, value);
        if (value == 0)
        {
            uint16_t value = registers.getMDR();
            // Update condition codes
            if (value == 0)
            {
                registers.setCC(0x02); // Zero flag
            }
            else if (value & 0x8000)
            {
                registers.setCC(0x04); // Negative flag
            }
            else
            {
                registers.setCC(0x01); // Positive flag
            }
        }
    }
        break;
    case 0xE:
        // LEA
    {
        // Store address in destination register
        registers.setR(dr, address);
    }
        break;
    case 0x3:
        //ST
    {
        // Set the address in MAR
        registers.setMAR(address);
        // Set the value to be stored in MDR
        registers.setMDR(value);

        // Store the value in the DR to the computed address
        index = registers.getMAR();
        memory.write(registers.getMAR(), registers.getMDR());
    }
        break;
    case 0xB:
        //STI
    {
        // Set the address in MAR
        registers.setMAR(address);
        // Set the value to be stored in MDR
        registers.setMDR(value);
        // Store the value in the SR to the memory at the address pointed to by the computed address
        memory.write(memory.read(registers.getMAR()), registers.getMDR());
        // Store the value in the DR to the computed address
        index=memory.read(registers.getMAR());
    }
        break;
    case 0x7:
        //STR
    {
        // Set the address in MAR
        registers.setMAR(address);
        // Set the value to be stored in MDR
        registers.setMDR(value);

        // Store the value in the SR to the computed address
        index = registers.getMAR();
        memory.write(registers.getMAR(), registers.getMDR());
    }
        break;
    case 0x4:
    {

        // JSR JSRR
        uint16_t currentPC = registers.getPC(); // Get the current PC
        // Store the current PC in R7
        registers.setR(7, currentPC);
        registers.setPC(address); // Update PC with the offset
    }
        break;
    case 0x5:
        // AND
    {
        // Set condition codes
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        if (result == 0)
        {
            registers.setCC(0x02);
        }
        else if (result >> 15)
        {
            registers.setCC(0x04); // Negative
        }
        else
        {
            registers.setCC(0x01); // Positive
        }
    }
        break;
    case 0xC:
        //RET JMP
    {

        registers.setPC(address);

    }
        break;
    case 0x9:
        // NOT
    {
        // Set condition codes
        registers.setR(dr, GateALU);
        uint16_t result = registers.getR(dr);
        if (result == 0)
        {
            registers.setCC(0x02); // Zero
        }
        else if (result >> 15)
        {
            registers.setCC(0x04); // Negative
        }
        else
        {
            registers.setCC(0x01); // Positive
        }
    }
        break;
    default:
        // Handle unsupported opcode
        break;
    }
}
bool LC3Instructions::isHalt(){
    return (registers.getMDR() == 0xF025);
}
