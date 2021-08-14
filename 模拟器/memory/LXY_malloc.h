#pragma once
#include <malloc.h>
#include <Windows.h>

#if _WIN64
#else
#define lxy_memset lxy_memset32
#define lxy_memcpy_aligned lxy_memcpy_aligned32
#define lxy_memcpy_noaligned lxy_memcpy_noaligned32

extern "C" void lxy_memset(void* source, int val, size_t size);
extern "C" void lxy_memcpy_aligned(void* lhs, size_t size_l, void* rhs, size_t size_r);
extern "C" void lxy_memcpy_noaligned(void* lhs, size_t size_l, void* rhs, size_t size_r);
#endif

extern "C" inline void* lxy_malloc(size_t size){

	if (size & 0x3f) {
		size += 0x40;
	}
	size &= (size_t)-0x40;
	return _aligned_malloc(size, 0x40);
}

extern "C" inline void lxy_free(void* addr) {

	addr = (void*)(size_t(addr) & size_t(-0x40));

	_aligned_free(addr);
}

extern "C" inline void lxy_memcpy(void* lhs, size_t size_l, void* rhs, size_t size_r) {

	if ((size_t(rhs) & 0x3f) == 0 && (size_t(lhs) & 0x3f) == 0) {
		lxy_memcpy_aligned(lhs, size_l, rhs, size_r);
	}
	else {
		lxy_memcpy_noaligned(lhs, size_l, rhs, size_r);
	}
}

