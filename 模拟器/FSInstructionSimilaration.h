#pragma once
#include "Framework.h"
#include "util.h"

inline void fs_push(cs_insn* insn)
{
	DWORD dwRegFlag = insn->detail->x86.operands[0].mem.base;//寄存器
	DWORD dwOffSet;
	if(dwRegFlag == X86_REG_INVALID)
		dwOffSet = insn->detail->x86.operands[0].mem.disp;
	else
	{
		uc_reg_read(g_uc, dwRegFlag, &dwOffSet);
		dwRegFlag = insn->detail->x86.operands[0].mem.index;
		if (dwRegFlag != X86_REG_INVALID)
		{
			DWORD dwTemp;
			uc_reg_read(g_uc, dwRegFlag, &dwTemp);
			dwOffSet += dwTemp;
		}
		dwOffSet += insn->detail->x86.operands[0].mem.disp;
	}
	dwOffSet += g_Regs.seg_regs.s_fs;
	DWORD dwRead = 0;
	g_Error = uc_mem_read(g_uc, dwOffSet, &dwRead, 4);
	g_Regs.regs.r_esp -= 4;
	g_Regs.regs.r_eip += insn->size;
	g_Regs.regs.r_efl &= 0xfffffeff;//取出tf标志
	write_regs();
	g_Error = uc_mem_write(g_uc, g_Regs.regs.r_esp, &dwRead, 4);
	print_mem(g_Regs.regs.r_esp);
}
inline void fs_mov(cs_insn* insn)
{
	if (insn->detail->x86.operands[0].type == X86_OP_MEM)
	{
		DWORD dwRegFlag = insn->detail->x86.operands[0].mem.base;//寄存器
		DWORD dwOffSet;
		if (dwRegFlag == X86_REG_INVALID)
			dwOffSet = insn->detail->x86.operands[0].mem.disp;
		else
		{
			uc_reg_read(g_uc, dwRegFlag, &dwOffSet);
			dwRegFlag = insn->detail->x86.operands[0].mem.index;
			if (dwRegFlag != X86_REG_INVALID)
			{
				DWORD dwTemp;
				uc_reg_read(g_uc, dwRegFlag, &dwTemp);
				dwOffSet += dwTemp;
			}
			dwOffSet += insn->detail->x86.operands[0].mem.disp;
		}
		dwOffSet += g_Regs.seg_regs.s_fs;
		dwRegFlag = insn->detail->x86.operands[1].reg;
		DWORD dwRegValue;
		uc_reg_read(g_uc, dwRegFlag, &dwRegValue);
		g_Regs.regs.r_eip += insn->size;
		g_Regs.regs.r_efl &= 0xfffffeff;//取出tf标志
		write_regs();
		g_Error = uc_mem_write(g_uc, dwOffSet, &dwRegValue, 4);
		print_mem(g_Regs.regs.r_esp);
	}
	else if (insn->detail->x86.operands[0].type == X86_OP_REG)
	{
		DWORD dwRegFlag = insn->detail->x86.operands[1].mem.base;//寄存器
		DWORD dwOffSet;
		if (dwRegFlag == X86_REG_INVALID)
			dwOffSet = insn->detail->x86.operands[1].mem.disp;
		else
		{
			uc_reg_read(g_uc, dwRegFlag, &dwOffSet);
			dwRegFlag = insn->detail->x86.operands[1].mem.index;
			if (dwRegFlag != X86_REG_INVALID)
			{
				DWORD dwTemp;
				uc_reg_read(g_uc, dwRegFlag, &dwTemp);
				dwOffSet += dwTemp;
			}
			dwOffSet += insn->detail->x86.operands[1].mem.disp;
		}
		dwOffSet += g_Regs.seg_regs.s_fs;

		dwRegFlag = insn->detail->x86.operands[0].reg;
		DWORD dwValue;
		g_Error = uc_mem_read(g_uc, dwOffSet, &dwValue, 4);
		uc_reg_write(g_uc, dwRegFlag, &dwValue);
		read_regs();
		g_Regs.regs.r_eip += insn->size;
		g_Regs.regs.r_efl &= 0xfffffeff;//去除tf标志
		write_regs();
		print_mem(g_Regs.regs.r_esp);
	}
}
