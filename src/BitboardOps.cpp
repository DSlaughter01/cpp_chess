#include "BitboardOps.hpp"

void BitboardOps::SetBit(uint64_t &bitboard, int idx) {
    bitboard |= (1ULL << idx);
}

void BitboardOps::ClearBit(uint64_t &bitboard, int idx) {
    bitboard &= ~(1ULL << idx);        
}

bool BitboardOps::HasIntersection(uint64_t &bb1, uint64_t &bb2) {
    
    if ((bb1 & bb2) != BitboardOps::EMPTY_BITBOARD)
        return true;
    else
        return false;
}
