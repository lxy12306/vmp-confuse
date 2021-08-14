#pragma once
#include "../Framework.h"

#define PRRINT_MARK_VMPCONFUSE 1

void e_Mark_control_flow_instruction_init(); //初始化查询
void e_fenxi_kongzhiliu(cs_insn* cs);

void  e_print_result_vmpcontrol_flow(DWORD dwIndex);//打印结果 dwIndex==-1 表示退出

void e_print_deconfuse_result(DWORD dwIndex);