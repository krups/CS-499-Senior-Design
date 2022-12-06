#include "copyBits.h"

/**
 * @brief Copy bits from big endian src to big endian dst
 * 
 * @param src big endian
 * @param srcStartBit 
 * @param dst big endian
 * @param dstStartBit 
 * @param n number of bits to copy 
 */
void copyBitsB(uint8_t* src, unsigned int srcStartBit, uint8_t* dst, unsigned int dstStartBit, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[srcPos / 8] << (srcPos % 8)) & (1 << 7);
        dst[dstPos / 8] &= ~(1 << (7 - (dstPos % 8)));
        dst[dstPos / 8] |= (desiredBit >> (dstPos % 8));
    }
}

/**
 * @brief Copy bits from little endian src to little endian dst
 * 
 * @param src little endian
 * @param srcStartBit 
 * @param srcSizeBytes 
 * @param dst little endian
 * @param dstStartBit 
 * @param dstSizeBytes 
 * @param n number of bits to copy 
 */
void copyBitsL(uint8_t* src, unsigned int srcStartBit, unsigned int srcSizeBytes, uint8_t* dst, unsigned int dstStartBit,unsigned int dstSizeBytes, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[(srcSizeBytes - 1) - (srcPos / 8)] << (srcPos % 8)) & (1 << 7);
        dst[(dstSizeBytes - 1) - (dstPos / 8)] &= ~(1 << (7 - (dstPos % 8)));
        dst[(dstSizeBytes - 1) - (dstPos / 8)] |= (desiredBit >> (dstPos % 8));
    }
}

/**
 * @brief Copy bits from big endian src to little endian dst
 * 
 * @param src big endian
 * @param srcStartBit 
 * @param dst little endian
 * @param dstStartBit 
 * @param dstSizeBytes 
 * @param n number of bits to copy 
 */
void copyBitsB2L(uint8_t* src, unsigned int srcStartBit, uint8_t* dst, unsigned int dstStartBit, unsigned int dstSizeBytes, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[srcPos / 8] << (srcPos % 8)) & (1 << 7);
        dst[(dstSizeBytes - 1) - (dstPos / 8)] &= ~(1 << (7 - (dstPos % 8)));
        dst[(dstSizeBytes - 1) - (dstPos / 8)] |= (desiredBit >> (dstPos % 8));
    }
}

/**
 * @brief Copy bits from little endian src to big endian dst
 * 
 * @param src 
 * @param srcStartBit 
 * @param srcSizeBytes 
 * @param dst 
 * @param dstStartBit 
 * @param n number of bits to copy 
 */
void copyBitsL2B(uint8_t* src, unsigned int srcStartBit, unsigned int srcSizeBytes, uint8_t* dst, unsigned int dstStartBit, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        int srcPos = srcStartBit + i;
        int dstPos = dstStartBit + i;
        uint8_t desiredBit = (src[(srcSizeBytes - 1) - (srcPos / 8)] << (srcPos % 8)) & (1 << 7);
        dst[dstPos / 8] &= ~(1 << (7 - (dstPos % 8)));
        dst[dstPos / 8] |= (desiredBit >> (dstPos % 8));
    }
}
