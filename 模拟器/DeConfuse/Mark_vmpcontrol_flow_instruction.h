#pragma once
#include "../Framework.h"

#define PRRINT_MARK_VMPCONFUSE 1

void e_Mark_control_flow_instruction_init(); //��ʼ����ѯ
void e_fenxi_kongzhiliu(cs_insn* cs);

void  e_print_result_vmpcontrol_flow(DWORD dwIndex);//��ӡ��� dwIndex==-1 ��ʾ�˳�

void e_print_deconfuse_result(DWORD dwIndex);