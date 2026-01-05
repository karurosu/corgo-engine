//
//  utils/bitset.h
//  A simple bitset implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_UTILS_BITSET_H
#define CORGO_UTILS_BITSET_H

#include "helpers.h"
#include "error.h"

#define CE_BITSET_MAX_BITS 128
#define CE_BITSET_ARRAY_SIZE ((CE_BITSET_MAX_BITS + 7) / 8) // Number of bytes needed

typedef struct CE_Bitset {
    size_t m_size;      // Number of bits in the bitset < = CE_BITSET_MAX_BITS
    uint8_t m_bits[CE_BITSET_ARRAY_SIZE];    // Pointer to the bit array
} CE_Bitset;

CE_Result CE_Bitset_init(OUT CE_Bitset* bitset, IN size_t size);
CE_Result CE_Bitset_setBit(INOUT CE_Bitset* bitset, IN size_t index);
CE_Result CE_Bitset_clearBit(INOUT CE_Bitset* bitset, IN size_t index);
CE_Result CE_Bitset_toggleBit(INOUT CE_Bitset* bitset, IN size_t index);
bool CE_Bitset_isBitSet(IN const CE_Bitset* bitset, IN size_t index);

#endif // CORGO_UTILS_BITSET_H
