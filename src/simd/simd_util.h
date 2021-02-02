#ifndef SIMD_UTIL_H
#define SIMD_UTIL_H

#include <bit>
#include <cstddef>
#include <cstdint>

constexpr uint64_t interleave_mask(size_t step) {
    uint64_t result = (1ULL << step) - 1;
    while (step < 32) {
        step <<= 1;
        result |= result << step;
    }
    return result;
}

inline uint64_t spread_bytes_32_to_64(uint32_t v) {
    uint64_t r = (((uint64_t)v << 16) | v) & 0xFFFF0000FFFFULL;
    return ((r << 8) | r) & 0xFF00FF00FF00FFULL;
}

inline uint8_t popcnt64(uint64_t val) {
    val -= (val >> 1) & 0x5555555555555555ULL;
    val = (val & 0x3333333333333333ULL) + ((val >> 2) & 0x3333333333333333ULL);
    val += val >> 4;
    val &= 0xF0F0F0F0F0F0F0FULL;
    val *= 0x101010101010101ULL;
    val >>= 56;
    return val;
}

#endif
