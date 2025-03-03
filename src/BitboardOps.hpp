#pragma once 
#include <bitset>
#include <cstdint>

namespace BitboardOps {

    const uint64_t EMPTY_BITBOARD = 0ULL;
    const uint64_t FULL_BITBOARD = 1ULL;

    void SetBit(uint64_t &bitboard, int idx);
    void ClearBit(uint64_t &bitboard, int idx);
    bool HasIntersection(uint64_t &bb1, uint64_t &bb2);
}