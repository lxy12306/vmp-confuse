#pragma once
#include "CommonFiles/ToolLibrary/unicorn-1.0.2-win32/include/unicorn/unicorn.h"
#include "CommonFiles/ToolLibrary//capstone-4.0.2-win32/include/capstone/capstone.h"


#define ACTION 2/*0 测试功能 1 系统调用处探寻  2.VMP去混淆*/

//内存区块结构体
struct Seg_Map {
	DWORD dwBase = 0;//对应原始内存的基址
	unsigned int uSize =0 ;
	const char* szFileName = NULL;
	unsigned char* bBuffer = NULL;	//包含文件的buffer
	Seg_Map() = default;
	Seg_Map(const Seg_Map& seg_map):dwBase(seg_map.dwBase)
	, uSize(seg_map.uSize), szFileName(seg_map.szFileName), bBuffer(seg_map.bBuffer){
	}
};

//寄存器结构体
typedef struct _Regs {
	union{
		struct {
			DWORD r_eax;
			DWORD r_ecx;
			DWORD r_edx;
			DWORD r_ebx;
			DWORD r_esp;
			DWORD r_ebp;
			DWORD r_esi;
			DWORD r_edi;
			DWORD r_eip;
			DWORD r_efl;
		} regs;
		DWORD dwU[10];
	};
	struct {
		DWORD s_fs;
	}seg_regs;
}Regs;

typedef struct _Reg_State {
	BYTE	eax_h;
	BYTE	ah;
	BYTE	al;
	BYTE	ecx_h;
	BYTE	ch;
	BYTE	cl;
	BYTE	edx_h;
	BYTE	dh;
	BYTE	dl;
	BYTE	ebx_h;
	BYTE	bh;
	BYTE	bl;
	BYTE	esi_h;
	BYTE	si;
	BYTE	edi_h;
	BYTE	di;
	BYTE	esp_h;
	BYTE	sp;
	BYTE	ebp_h;
	BYTE	bp;
	BYTE	eip;
	BYTE	eflags;
} Reg_State;

void init_simulator_byfile();
void simulator_destory();

extern Regs g_Regs; //寄存器
extern uc_err g_Error; //uc 错误信息相关
extern uc_engine* g_uc;//模拟器