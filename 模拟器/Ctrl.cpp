#include "Framework.h"
#include "CommonFiles\LXY\VC++\Exception\MyExceptionWindows.h"
#include <vector>
#include "memory\LXY_malloc.h"
using namespace std;

#define MD_Throw_Windows(szError) throw LXY::m_WindowsException(__FILE__,__LINE__,#szError, GetLastError())
#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(szError,__FILE__,__LINE__)    
vector<Seg_Map>  g_vecSegs;//内存状态

#if _DEBUG
#define SourceFileT(name) TEXT("MemoryDumpFile\\"#name)
#define SourceFileA(name) "MemoryDumpFile\\"#name
#endif

//初始寄存器状态
#define INIT_EAX	0x751710E0
#define INIT_EBX	0x00364000
#define INIT_ECX	0x00000000
#define INIT_EDX	0x00000000
#define INIT_EBP	0x0019FF70
#define INIT_ESP	0x0019FF2C
#define INIT_ESI	0x00504BD8
#define INIT_EDI	0x00507198
#define INIT_EIP	0x0043C195
#define INIT_EFL	0x00000202

Regs g_Regs; //寄存器
uc_engine* g_uc; //模拟器内存拷贝
uc_err g_Error; //uc 错误信息相关
csh g_Handle; 

//打印内存信息
void print_mem(DWORD dwAddress)
{
	
#define PRINT_SIZE 0x10
	DWORD adwVal[PRINT_SIZE];

	char acBuffer[0x1000];
	for (int i = 0; i < PRINT_SIZE; ++i)
	{
		uc_mem_read(g_uc, dwAddress, adwVal+i, 4);
		dwAddress += sizeof(DWORD);
	}
	const char* pPrint = "|%p| |%p| |%p| |%p|\r\n";
	for (int i = 0; i < (PRINT_SIZE >> 2); ++i)
	{
		LXY::m_PrintFile(NULL, pPrint, adwVal[(i<<2)], adwVal[(i << 2)+1], adwVal[(i << 2) + 2], adwVal[(i << 2) + 3]);
	}
}

//打印内存信息
void read_mem()
{
	for (int i = 0; i < g_vecSegs.size(); ++i) {
		uc_mem_read(g_uc, g_vecSegs[i].dwBase, g_vecSegs[i].bBuffer, g_vecSegs[i].uSize);
	}
}
//打印寄存器信息

void print_regs()
{
	const char* pPrint = "eax =<%p>, ebx =<%p>,ecx =<%p>,edx =<%p>\r\n\
		ebp =<%p>, esp =<%p>\r\n\
		esi =<%p>, edi =<%p>\r\n\
		eip =<%p>\r\n\
		eflag =<%p>\r\n\
		fs = <%p>\r\n";
	char acBuffer[0x1000];
	sprintf_s(acBuffer, pPrint, g_Regs.regs.r_eax, g_Regs.regs.r_ebx, g_Regs.regs.r_ecx, g_Regs.regs.r_edx,
		g_Regs.regs.r_ebp, g_Regs.regs.r_esp,
		g_Regs.regs.r_esi, g_Regs.regs.r_edi,
		g_Regs.regs.r_eip,
		g_Regs.regs.r_efl,
		g_Regs.seg_regs.s_fs);
	LXY::m_PrintFile(NULL, acBuffer);
}
//读取寄存器信息
void read_regs()
{
	uc_reg_read(g_uc, UC_X86_REG_EAX, &g_Regs.regs.r_eax);
	uc_reg_read(g_uc, UC_X86_REG_ECX, &g_Regs.regs.r_ecx);
	uc_reg_read(g_uc, UC_X86_REG_EDX, &g_Regs.regs.r_edx);
	uc_reg_read(g_uc, UC_X86_REG_EBX, &g_Regs.regs.r_ebx);
	uc_reg_read(g_uc, UC_X86_REG_ESP, &g_Regs.regs.r_esp);
	uc_reg_read(g_uc, UC_X86_REG_EBP, &g_Regs.regs.r_ebp);
	uc_reg_read(g_uc, UC_X86_REG_ESI, &g_Regs.regs.r_esi);
	uc_reg_read(g_uc, UC_X86_REG_EDI, &g_Regs.regs.r_edi);
	uc_reg_read(g_uc, UC_X86_REG_EIP, &g_Regs.regs.r_eip);
	uc_reg_read(g_uc, UC_X86_REG_EFLAGS, &g_Regs.regs.r_efl);
	//uc_reg_read(g_uc, UC_X86_REG_FS_BASE, &g_Regs.seg_regs.s_fs);
}
//写寄存器信息
void write_regs() {
	uc_reg_write(g_uc, UC_X86_REG_EAX, &g_Regs.regs.r_eax);
	uc_reg_write(g_uc, UC_X86_REG_ECX, &g_Regs.regs.r_ecx);
	uc_reg_write(g_uc, UC_X86_REG_EDX, &g_Regs.regs.r_edx);
	uc_reg_write(g_uc, UC_X86_REG_EBX, &g_Regs.regs.r_ebx);
	uc_reg_write(g_uc, UC_X86_REG_ESP, &g_Regs.regs.r_esp);
	uc_reg_write(g_uc, UC_X86_REG_EBP, &g_Regs.regs.r_ebp);
	uc_reg_write(g_uc, UC_X86_REG_ESI, &g_Regs.regs.r_esi);
	uc_reg_write(g_uc, UC_X86_REG_EDI, &g_Regs.regs.r_edi);
	uc_reg_write(g_uc, UC_X86_REG_EIP, &g_Regs.regs.r_eip);
	uc_reg_write(g_uc, UC_X86_REG_EFLAGS, &g_Regs.regs.r_efl);
	//uc_reg_write(g_uc, UC_X86_REG_FS_BASE, &g_Regs.seg_regs.s_fs);
}

void init_regs() {
	g_Regs.regs.r_eax = INIT_EAX;
	g_Regs.regs.r_ecx = INIT_ECX;
	g_Regs.regs.r_edx = INIT_EDX;
	g_Regs.regs.r_ebx = INIT_EBX;
	g_Regs.regs.r_esp = INIT_ESP;
	g_Regs.regs.r_ebp = INIT_EBP;
	g_Regs.regs.r_esi = INIT_ESI;
	g_Regs.regs.r_edi = INIT_EDI;
	g_Regs.regs.r_eip = INIT_EIP;
	g_Regs.regs.r_efl = INIT_EFL;
}
void init_regs2(DWORD (&adwRegs)[10]) {
	g_Regs.regs.r_eax = adwRegs[0];
	g_Regs.regs.r_ecx = adwRegs[1];
	g_Regs.regs.r_edx = adwRegs[2];
	g_Regs.regs.r_ebx = adwRegs[3];
	g_Regs.regs.r_esp = adwRegs[4];
	g_Regs.regs.r_ebp = adwRegs[5];
	g_Regs.regs.r_esi = adwRegs[6];
	g_Regs.regs.r_edi = adwRegs[7];
	g_Regs.regs.r_eip = adwRegs[8];
	g_Regs.regs.r_efl = adwRegs[9] &0x0eff;
}

void init_simulator_byfile()
{
	HANDLE hFile = CreateFile(SourceFileT(memdumpData), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MD_Throw_Windows(CreateFile);
	}
	DWORD adwRegs[10];
	if (!ReadFile(hFile, adwRegs, 40, NULL, NULL))
	{
		CloseHandle(hFile);
		MD_Throw_Windows(ReadFile);
	}
	
	int iSize = 0;
	if (!ReadFile(hFile, &iSize, 4, NULL, NULL))
	{
		CloseHandle(hFile);
		MD_Throw_Windows(ReadFile);
	}

	DWORD* pdwVirtualAndSize = new DWORD[2 * iSize];
	DWORD* pTemp = pdwVirtualAndSize;
	if (!ReadFile(hFile, pdwVirtualAndSize, 4*2*iSize, NULL, NULL))
	{
		CloseHandle(hFile);
		MD_Throw_Windows(ReadFile);
	}
	if (!ReadFile(hFile, &g_Regs.seg_regs.s_fs, 4, NULL, NULL))
	{
		CloseHandle(hFile);
		MD_Throw_Windows(ReadFile);
	}
	CloseHandle(hFile);
	const char* pccTemp = SourceFileA(memdump%x);
	char acBuffer[0x20];
	char* pcBuffer;
	Seg_Map psmTemp;

	for (int i = 0; i < iSize; ++i)
	{
		psmTemp.dwBase = *pTemp++;
		psmTemp.uSize = *pTemp++;
		sprintf_s(acBuffer, pccTemp, psmTemp.dwBase);
		pcBuffer = new char[0x20]; 
		strcpy_s(pcBuffer,0x20, acBuffer);
		psmTemp.szFileName = pcBuffer;
		psmTemp.bBuffer = NULL;
		g_vecSegs.push_back(psmTemp);
	}
	delete[] pdwVirtualAndSize;

	/*以上初始化从文件导入的寄存器基础信息 以及内存信息*/



	//模拟器环境初始化
	g_Error = uc_open(UC_ARCH_X86, UC_MODE_32, &g_uc);
	if (g_Error != UC_ERR_OK) {
		char pError[0x50];
		sprintf_s(pError,"Failed on uc_open() with error returned: %u\n", g_Error);
		MD_Throw_Runtime(pError);
	}
	//模拟器内存初始化
	for (int i = 0; i < g_vecSegs.size(); i++) {
		g_vecSegs[i].bBuffer = (unsigned char*)lxy_malloc(g_vecSegs[i].uSize);
		FILE* fp = fopen(g_vecSegs[i].szFileName, "rb");
		fread(g_vecSegs[i].bBuffer,g_vecSegs[i].uSize, 1, fp);
		fclose(fp);
		// map memory for this emulation
		g_Error = uc_mem_map(g_uc, g_vecSegs[i].dwBase, g_vecSegs[i].uSize, UC_PROT_ALL);
		// write machine code to be emulated to memory
		g_Error = uc_mem_write(g_uc, g_vecSegs[i].dwBase, g_vecSegs[i].bBuffer, g_vecSegs[i].uSize);
	}

	//模拟器寄存器初始化
	init_regs2(adwRegs);
	write_regs();

	//反汇编引擎初始化
	if (cs_open(CS_ARCH_X86, CS_MODE_32, &g_Handle)) {
		uc_close(g_uc);
		char pError[0x50];
		sprintf_s(pError, "ERROR: Failed to initialize engine!\n");
		MD_Throw_Runtime(pError);
	}
	cs_option(g_Handle, CS_OPT_DETAIL, CS_OPT_ON);
}
void simulator_destory() 
{
	for (auto value : g_vecSegs)
	{
		delete[] value.szFileName;
		lxy_free(value.bBuffer);
	}

	cs_close(&g_Handle);
	uc_close(g_uc);
}

void instruction_disamble(cs_insn** insn)
{
	BYTE code[32];
	uc_mem_read(g_uc, g_Regs.regs.r_eip, code, 32);
	cs_disasm(g_Handle, code, 32, g_Regs.regs.r_eip, 1, insn);
}

void instruction_free(cs_insn* insn)
{
	cs_free(insn, 1);
}