//
//  utils/bitset.h
//  A simple bitset implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#include "bitset.h"

CE_Result CE_Bitset_init(OUT CE_Bitset* bitset, IN size_t size)
{
    if (size == 0 || size > CE_BITSET_MAX_BITS) {
        return CE_ERROR;
    }
    bitset->m_size = size;
    for (size_t i = 0; i < CE_BITSET_ARRAY_SIZE; i++) {
        bitset->m_bits[i] = 0;
    }
    return CE_OK;
}

CE_Result CE_Bitset_setBit(INOUT CE_Bitset* bitset, IN size_t index)
{
    if (index >= bitset->m_size) {
        return CE_ERROR;
    }
    bitset->m_bits[index / (sizeof(CE_BITSET_STORAGE_TYPE) * 8)] |= (1 << (index % (sizeof(CE_BITSET_STORAGE_TYPE) * 8)));
    return CE_OK;
}

CE_Result CE_Bitset_clearBit(INOUT CE_Bitset* bitset, IN size_t index)
{
    if (index >= bitset->m_size) {
        return CE_ERROR;
    }
    bitset->m_bits[index / (sizeof(CE_BITSET_STORAGE_TYPE) * 8)] &= ~(1 << (index % (sizeof(CE_BITSET_STORAGE_TYPE) * 8)));
    return CE_OK;
}

CE_Result CE_Bitset_toggleBit(INOUT CE_Bitset* bitset, IN size_t index)
{
    if (index >= bitset->m_size) {
        return CE_ERROR;
    }
    bitset->m_bits[index / (sizeof(CE_BITSET_STORAGE_TYPE) * 8)] ^= (1 << (index % (sizeof(CE_BITSET_STORAGE_TYPE) * 8)));
    return CE_OK;
}

bool CE_Bitset_isBitSet(IN const CE_Bitset* bitset, IN size_t index)
{
    if (index >= bitset->m_size) {
        return false;
    }
    return (bitset->m_bits[index / (sizeof(CE_BITSET_STORAGE_TYPE) * 8)] & (1 << (index % (sizeof(CE_BITSET_STORAGE_TYPE) * 8)))) != 0;
}

CE_Result CE_Bitset_clear(INOUT CE_Bitset* bitset)
{
    for (size_t i = 0; i < CE_BITSET_ARRAY_SIZE; i++) {
        bitset->m_bits[i] = 0;
    }
    return CE_OK;
}

bool CE_Bitset_containsBits(IN const CE_Bitset* a, IN const CE_Bitset* b)
{
    if (a->m_size != b->m_size) {
        return false;
    }

    for (size_t i = 0; i < CE_BITSET_ARRAY_SIZE; i++) {
        if ((a->m_bits[i] & b->m_bits[i]) != b->m_bits[i]) {
            return false;
        }
    }

    return true;
}
