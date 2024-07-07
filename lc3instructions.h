#ifndef LC3INSTRUCTIONS_H
#define LC3INSTRUCTIONS_H

#include "lc3registers.h"
#include "lc3memory.h"



class LC3Instructions
{
public:

    static void fetch( LC3Memory& memory);
    static void decode();
    static void  evaluateAddress( LC3Memory& memory);
    static void fetchOperands(LC3Memory& memory);
    static void execute();
    static void  store(LC3Memory &memory);
    static void updateFlags(uint16_t result);
    static bool isHalt();



};

#endif // LC3INSTRUCTIONS_H
