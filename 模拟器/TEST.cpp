#include "Framework.h"
#if ACTION == 0
#include "F:\Code\object\Exception\MyExceptionWindows.h"
#include "util.h"
#include "FSInstructionSimilaration.h"
#include "vmp_to_confirm_correctness_of_deobfuscation.h"
#pragma comment(lib, "../../../Code/object/tool/unicorn-1.0.2-win32/unicorn.lib")
#pragma comment(lib, "../../../Code/object/tool/capstone-4.0.2-win32/capstone_dll.lib")
std::vector<cs_insn*> g_vec_cs_insn;

int main()
{
	LXY::SimulatorStatus* p_begin_now = new LXY::SimulatorStatus();
	try {
		init_simulator_byfile();
		p_begin_now->e_save_current();
		auto b_ret = p_begin_now->e_cmp_status();
	}
	catch (LXY::m_WindowsException& mwe)
	{
		mwe.e_ShowError();
		return -1;
	}
	catch (LXY::m_runtime_error& me)
	{
		me.e_ShowError();
		return -1;
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
			auto temp = p_begin_now->e_swap_current_ret();
			delete p_begin_now;
			p_begin_now = temp;
			int i_temp_c;
			while ((i_temp_c = getchar()))
			{
				if (i_temp_c == 'b' || i_temp_c == 'c')
					break;
			}
			if (i_temp_c == 'c')
				continue;
			else
				break;
		}
		read_regs();
		instruction_free(insn);
	}

	delete p_begin_now;
	simulator_destory();

	return 0;
}

#endif