#include "..\Framework.h"
#if ACTION == 2
#include "..\CommonFiles\LXY\VC++\Exception\MyExceptionWindows.h"
#include "..\util.h"
#include "..\FSInstructionSimilaration.h"
#include "..\TeShuZhiLing.h"
#include "vmp_to_confuse.h"
#include "vmp_to_confirm_correctness_of_deobfuscation.h"

#include "Mark_vmpcontrol_flow_instruction.h"

#include <string>
#include <vector>
#pragma comment(lib, "CommonFiles/ToolLibrary/unicorn-1.0.2-win32/unicorn.lib")
#pragma comment(lib, "CommonFiles/ToolLibrary/capstone-4.0.2-win32/capstone_dll.lib")


std::vector<cs_insn*> g_vec_cs_insn;
unsigned char g_uc_control = 1;
extern DWORD g_dwIndex; //记录是第几块
extern std::vector<int> g_vecb_is_useful;//有用的指令的标记

void e_print_g_vec_cs_insn_all() {

#define	FILE(dwIndex) sprintf_s(ac_buffer,"OutPutFile\\Debug\\g_vec_cs_insn_all.txt")

	char ac_buffer[MAX_PATH];
	FILE(dwIndex);
	FILE* p_file = NULL;
	auto error = fopen_s(&p_file, ac_buffer, "w");
	if (error != 0)
		return;

	for (int i = 0; i < g_vec_cs_insn.size(); ++i)
	{
		std::string strTemp = std::to_string(i);
		strTemp += ": ";
		strTemp += g_vec_cs_insn[i]->mnemonic;
		strTemp += " ";
		strTemp += g_vec_cs_insn[i]->op_str;
		fprintf_s(p_file, "%-40s\r\n", strTemp.c_str(), 40);
	}

	fclose(p_file);
}


/// <summary>
/// 
/// </summary>
/// <param name="p_begin_last"></param>
void rebuild_and_reecx(LXY::SimulatorStatus* p_begin_last) {
	p_begin_last->e_swap_current_noret();

	int i_size = g_vecb_is_useful.size();
	int i = 0;
	while (i<i_size)
	{
		cs_insn* insn = NULL;
		instruction_disamble(&insn);
		if (g_vecb_is_useful[i]== 0) {
			g_Regs.regs.r_eip += insn->size;
			write_regs();
			++i;
			continue;
		}
		if ((insn->detail->x86.operands[0].type == X86_OP_MEM && insn->detail->x86.operands[0].reg == X86_REG_FS)
			|| (insn->detail->x86.operands[1].type == X86_OP_MEM && insn->detail->x86.operands[1].reg == X86_REG_FS))
		{
			LXY::m_PrintFile(NULL, "fs寄存器操作指令\r\n");
			if (strcmp(insn->mnemonic, "push") == 0)
			{
				fs_push(insn);
			}
			else if (strcmp(insn->mnemonic, "mov") == 0)
			{
				fs_mov(insn);
			}
			else
			{
				g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
			}
		}
		else if (is_spetical_insn(insn)) {

		}
		else
		{
			g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
		}

		if (g_Error != 0)
		{
			printf("Exception with error returned %u: %s\n",
				g_Error, uc_strerror(g_Error));

			print_regs();
			print_mem(g_Regs.regs.r_esp);
			__asm int 3
			break;
		}
		read_regs();
		instruction_free(insn);
		++i;
	}

	//执行那一条特殊指令
	cs_insn* insn = NULL;
	instruction_disamble(&insn);
	if ((insn->detail->x86.operands[0].type == X86_OP_MEM && insn->detail->x86.operands[0].reg == X86_REG_FS)
		|| (insn->detail->x86.operands[1].type == X86_OP_MEM && insn->detail->x86.operands[1].reg == X86_REG_FS))
	{
		LXY::m_PrintFile(NULL, "fs寄存器操作指令\r\n");
		if (strcmp(insn->mnemonic, "push") == 0)
		{
			fs_push(insn);
		}
		else if (strcmp(insn->mnemonic, "mov") == 0)
		{
			fs_mov(insn);
		}
		else if (is_spetical_insn(insn)) {

		}
		else
		{
			g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
		}
	}
	else
	{
		g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
	}

	if (g_Error != 0)
	{
		printf("Exception with error returned %u: %s\n",
			g_Error, uc_strerror(g_Error));

		print_regs();
		print_mem(g_Regs.regs.r_esp);
		__asm int 3
	}
	read_regs();
	instruction_free(insn);


	if (!p_begin_last->e_cmp_status()) {
		printf("%d\r\n", g_dwIndex);
		e_print_g_vec_cs_insn_all();
		__asm int 3;
		p_begin_last->e_swap_current_noret();//恢复原来的状态
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="p_begin_last"></param>
void rebuild_and_reecx2(LXY::SimulatorStatus* p_begin_last) {
	p_begin_last->e_swap_current_noret();

	int i_size = g_vecb_is_useful.size();
	int i = 0;
	while (i < i_size)
	{
		cs_insn* insn = NULL;
		instruction_disamble(&insn);
		if ((insn->detail->x86.operands[0].type == X86_OP_MEM && insn->detail->x86.operands[0].reg == X86_REG_FS)
			|| (insn->detail->x86.operands[1].type == X86_OP_MEM && insn->detail->x86.operands[1].reg == X86_REG_FS))
		{
			LXY::m_PrintFile(NULL, "fs寄存器操作指令\r\n");
			if (strcmp(insn->mnemonic, "push") == 0)
			{
				fs_push(insn);
			}
			else if (strcmp(insn->mnemonic, "mov") == 0)
			{
				fs_mov(insn);
			}
			else
			{
				g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
			}
		}
		else
		{
			g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
		}

		if (g_Error != 0)
		{
			printf("Exception with error returned %u: %s\n",
				g_Error, uc_strerror(g_Error));

			print_regs();
			print_mem(g_Regs.regs.r_esp);
			__asm int 3
			break;
		}
		read_regs();
		instruction_free(insn);
		++i;
	}

	//执行那一条特殊指令
	cs_insn* insn = NULL;
	instruction_disamble(&insn);
	if ((insn->detail->x86.operands[0].type == X86_OP_MEM && insn->detail->x86.operands[0].reg == X86_REG_FS)
		|| (insn->detail->x86.operands[1].type == X86_OP_MEM && insn->detail->x86.operands[1].reg == X86_REG_FS))
	{
		LXY::m_PrintFile(NULL, "fs寄存器操作指令\r\n");
		if (strcmp(insn->mnemonic, "push") == 0)
		{
			fs_push(insn);
		}
		else if (strcmp(insn->mnemonic, "mov") == 0)
		{
			fs_mov(insn);
		}
		else
		{
			g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
		}
	}
	else
	{
		g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
	}

	if (g_Error != 0)
	{
		printf("Exception with error returned %u: %s\n",
			g_Error, uc_strerror(g_Error));

		print_regs();
		print_mem(g_Regs.regs.r_esp);
		__asm int 3
	}
	read_regs();
	instruction_free(insn);


	p_begin_last->e_cmp_status();
}


int main()
{
	LXY::SimulatorStatus* p_begin_now = new LXY::SimulatorStatus();
	e_Mark_control_flow_instruction_init();
	try {
		init_simulator_byfile();
		p_begin_now->e_save_current();
	}
	catch (LXY::m_WindowsException& mwe)
	{
		mwe.e_ShowError();
	}
	while (true)
	{
		cs_insn* insn = NULL;
		instruction_disamble(&insn);
		if ((insn->detail->x86.operands[0].type == X86_OP_MEM && insn->detail->x86.operands[0].reg == X86_REG_FS)
			|| (insn->detail->x86.operands[1].type == X86_OP_MEM && insn->detail->x86.operands[1].reg == X86_REG_FS))
		{
			LXY::m_PrintFile(NULL, "fs寄存器操作指令\r\n");
			if (strcmp(insn->mnemonic, "push") == 0)
			{
				fs_push(insn);
			}
			else if (strcmp(insn->mnemonic, "mov") == 0)
			{
				fs_mov(insn);
			}
			else
			{
				g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
			}
		}
		else if (is_spetical_insn(insn)) {

		}
		else
		{
			g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
		}

		if (g_Error != 0)
		{
			printf("Exception with error returned %u: %s\n",
				g_Error, uc_strerror(g_Error));

			print_regs();
			print_mem(g_Regs.regs.r_esp);
			__asm int 3
			break;
		}		
		read_regs();

		if (LXY::is_special_instructions_begin(insn))//判断是否为特殊的控制指令
		{
			g_uc_control = 0;
		}
		if (g_uc_control == 0) //如果是关键指令 则去除上一次存储在g_vec_cs_insn中元素的混淆
		{
			if (!g_vec_cs_insn.empty())
			{
				LXY::begin_analysis();
				rebuild_and_reecx(p_begin_now);
				e_fenxi_kongzhiliu(insn);
				LXY::e_clear();
			}
			p_begin_now->e_save_current();
			g_uc_control = 1;
		}
		else if (g_uc_control == 1)
		{
			g_vec_cs_insn.push_back(insn);
		}

		//instruction_free(insn);
	}


	simulator_destory();

	return 0;
}

#endif
