#include <Windows.h>

void __declspec(naked) test1() {
	__asm {
		mov eax,4
		mov edx,8
		or eax,edx
		ret
	}
}


int main() {
	
	test1();
	return 0;
}