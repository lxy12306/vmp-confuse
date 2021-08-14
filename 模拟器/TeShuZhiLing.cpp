#include "TeShuZhiLing.h"
bool is_spetical_insn(cs_insn* insn) {
	if (strcmp(insn->mnemonic, "bswap") == 0) { //uc ��bswapָ��ģ��ʧ�� bswap bx(ebx = 0xffff)֮��
		//ebx Ӧ��Ϊ 0 ����ָ��ģ��Ϊ0xffff0000
		auto type = insn->detail->x86.operands[0].reg;
		if (type == X86_REG_DX) {
			int i_temp = 0;
			g_Error = uc_emu_start(g_uc, g_Regs.regs.r_eip, 0x7fffffff, 0, 1/*ִ��һ��ָ��*/);
			uc_reg_write(g_uc, X86_REG_EDX, &i_temp);
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}