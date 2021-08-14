#pragma once
#include <memory>

#if _WIN64
#else
#define lxy_memcmp lxy_memcmp32
#endif

std::shared_ptr<unsigned char> lxy_memcmp(void* rhs, void* lhs, size_t size);
int lxy_internet_checksum(void* source, size_t size, uint16_t* answer);
int lxy_check_internet_checksum(void* source, size_t size, uint16_t check);