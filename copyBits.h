#include <cstdint>

void copyBitsB(uint8_t* src, unsigned int srcStartBit, uint8_t* dst, unsigned int dstStartBit, unsigned int n);

void copyBitsL(uint8_t* src, unsigned int srcStartBit, unsigned int srcSizeBytes, uint8_t* dst, unsigned int dstStartBit,unsigned int dstSizeBytes, unsigned int n);

void copyBitsB2L(uint8_t* src, unsigned int srcStartBit, uint8_t* dst, unsigned int dstStartBit, unsigned int dstSizeBytes, unsigned int n);

void copyBitsL2B(uint8_t* src, unsigned int srcStartBit, unsigned int srcSizeBytes, uint8_t* dst, unsigned int dstStartBit, unsigned int n);