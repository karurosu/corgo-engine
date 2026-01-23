//
//  utils/bitset.h
//  A simple bitset implementation.
//  Copyright (c) 2026 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_UTILS_BITSET_H
#define CORGO_UTILS_BITSET_H

#include "helpers.h"
#include "error.h"

#define CE_BITSET_MAX_BITS 256
#define CE_BITSET_STORAGE_TYPE uint32_t
#define CE_BITSET_ARRAY_SIZE ((CE_BITSET_MAX_BITS + sizeof(CE_BITSET_STORAGE_TYPE) * 8 - 1) / (sizeof(CE_BITSET_STORAGE_TYPE) * 8)) // Number of 32-bit integers needed

typedef struct CE_Bitset {
    size_t m_size;      // Number of bits in the bitset < = CE_BITSET_MAX_BITS
    CE_BITSET_STORAGE_TYPE m_bits[CE_BITSET_ARRAY_SIZE];    // Pointer to the bit array
} CE_Bitset;

/**
 * @brief Initialize a bitset with the given size.
 * 
 * Sets up a bitset to hold the specified number of bits, all initially cleared.
 * 
 * @param[in,out] bitset The bitset to initialize.
 * @param[in] size The number of bits the bitset should hold.
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Bitset_init(OUT CE_Bitset* bitset, IN size_t size);

/**
 * @brief Set a bit at the specified index.
 * 
 * Sets the bit at the given index to 1.
 * 
 * @param[in,out] bitset The bitset to modify.
 * @param[in] index The index of the bit to set.
 * 
 * @return CE_OK on success, CE_ERROR if index is out of bounds.
 */
CE_Result CE_Bitset_setBit(INOUT CE_Bitset* bitset, IN size_t index);
    
/**
 * @brief Clear a bit at the specified index.
 * 
 * Sets the bit at the given index to 0.
 * 
 * @param[in,out] bitset The bitset to modify.
 * @param[in] index The index of the bit to clear.
 * 
 * @return CE_OK on success, CE_ERROR if index is out of bounds.
 */
CE_Result CE_Bitset_clearBit(INOUT CE_Bitset* bitset, IN size_t index);

/**
 * @brief Toggle a bit at the specified index.
 * 
 * Flips the bit at the given index (0 to 1 or 1 to 0).
 * 
 * @param[in,out] bitset The bitset to modify.
 * @param[in] index The index of the bit to toggle.
 * 
 * @return CE_OK on success, CE_ERROR if index is out of bounds.
 */
CE_Result CE_Bitset_toggleBit(INOUT CE_Bitset* bitset, IN size_t index);

/**
 * @brief Check if a bit at the specified index is set.
 * 
 * Returns true if the bit at the given index is 1, false if it is 0.
 * 
 * @param[in] bitset The bitset to query.
 * @param[in] index The index of the bit to check.
 * 
 * @return true if the bit is set, false if clear or index is out of bounds.
 */
bool CE_Bitset_isBitSet(IN const CE_Bitset* bitset, IN size_t index);

/**
 * @brief Clear all bits in the bitset.
 * 
 * Sets all bits in the bitset to 0.
 * 
 * @param[in,out] bitset The bitset to clear.
 * 
 * @return CE_OK on success.
 */
CE_Result CE_Bitset_clear(INOUT CE_Bitset* bitset);

/**
 * @brief Get the size of the bitset.
 * 
 * Returns the number of bits the bitset can hold.
 * 
 * @param[in] bitset The bitset to query.
 * 
 * @return The size of the bitset in bits.
 */
inline size_t CE_Bitset_getSize(IN const CE_Bitset* bitset) {
    return bitset->m_size;
}

/**
 * @brief Check if bitset A has all the bits of bitset B.
 * 
 * Compares two bitsets and returns true if all set bits in B are also set in A.
 * A may have additional bits set, those are ignored
 * Both bitsets must be of the same size.
 * 
 * @param[in] a The first bitset to compare.
 * @param[in] b The second bitset to compare.
 * 
 * @return true if all bits match, false otherwise or if sizes differ.
 */
bool CE_Bitset_containsBits(IN const CE_Bitset* a, IN const CE_Bitset* b);

#endif // CORGO_UTILS_BITSET_H
