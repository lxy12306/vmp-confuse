#include <vector>
#include <set>
#include <string>
#include <string.h>
#include "Framework.h"
using namespace std;

extern vector<int> g_vecb_is_useful;//有用的指令的标记
extern vector<cs_insn*> g_vec_cs_insn;;//每个指令的对应读写寄存器的str索引
extern int gi_num_useful_instruction;//有用的指令数量的标记

namespace LXY {


	inline static int next_useful(int i) {
		for (int j = i + 1; j < g_vecb_is_useful.size(); ++j) {
			if (g_vecb_is_useful[j]) {
				return j;
			}
		}
		return -1;
	}


	inline bool is_cdq(cs_insn* cs) {

		string s = cs->mnemonic;
		if (s == "cdq") {
			return true;
		}
		return false;
	}
	inline bool is_cmov(cs_insn* cs) {
		if (strstr(cs->mnemonic, "cmov")) {
			return true;
		}
		return false;
	}

	inline bool is_sbb_adc(cs_insn* cs) {
		if (strcmp(cs->mnemonic, "sbb") == 0 || strcmp(cs->mnemonic, "adc") == 0) {
			return true;
		}
		return false;
	}//这个规则 ？ 

	inline bool is_mov_0(cs_insn* cs) {
		string s = cs->mnemonic;
		if (s == "mov" && (cs->detail->x86.operands[1].type == X86_OP_IMM) ) {
			auto p = strrchr(cs->op_str, ',') + 1;
			if (strcmp(strrchr(cs->op_str, ',') + 1, " 0") == 0) {
				return true;
			}
			return false;
		}
		return false;
	}

	void m_clean_special_ins() {
		for (int i = g_vecb_is_useful.size()-1; i != -1; --i) {
			if (g_vecb_is_useful[i] != 1) {
				continue;
			}
			if (is_cdq(g_vec_cs_insn[i])) {
				g_vecb_is_useful[i] = 0;
				gi_num_useful_instruction--;
				continue;
			}
		}
	}

	void m_clean_special_ins_when_end() {
		for (int i =0 ; i != g_vecb_is_useful.size(); ++i) {
			if (g_vecb_is_useful[i] != 1) {
				continue;
			}
			//去除 mov eax,0  sub ebx,eax 这样的指令
			if (is_cmov(g_vec_cs_insn[i])) {
				g_vecb_is_useful[i] = 0;
				gi_num_useful_instruction--;
			}
			else if (is_sbb_adc(g_vec_cs_insn[i])) {
				g_vecb_is_useful[i] = 0;
				gi_num_useful_instruction--;
			}
			else if(is_mov_0(g_vec_cs_insn[i])) {
				auto j = next_useful(i);
				if (j == -1) {
					continue;
				}
				auto ins = g_vec_cs_insn[j];
				if (strcmp(ins->mnemonic, "sub") == 0 || strcmp(ins->mnemonic, "add") == 0) {
					if (g_vec_cs_insn[i]->detail->x86.operands[0].reg == g_vec_cs_insn[j]->detail->x86.operands[1].reg) {
						g_vecb_is_useful[j] = 0;
						gi_num_useful_instruction --;
					}
				}
			}
		}
	}
}

