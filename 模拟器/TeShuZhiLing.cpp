#include "TeShuZhiLing.h"
bool is_spetical_insn(cs_insn* insn) {
	if (strcmp(insn->mnemonic, "bswap") == 0) { //uc 的bswap指令模拟失败 bswap bx(ebx = 0xffff)之后
		//ebx 应该为 0 而该指令模拟为0xffff0000
		auto type = insn->detail->x86.operands[0].reg;
		if (type == X86_REG_DX) {
			int i_temp = 0;
			g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*执行一条指令*/);
			uc_reg_write(g_uc, X86_REG_EDX, &i_temp);
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}