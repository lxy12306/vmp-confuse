#include <vector>
#include <unordered_map>
#include <set>
#include <string>
#include "Mark_vmpcontrol_flow_instruction.h"
#include "../CommonFiles/LXY/VC++/Exception/MyExcetpionCpp.h"

#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(#szError,__FILE__,__LINE__)    
using namespace std;

extern const char* asz_reg_names[26];
extern vector<set<string>> g_vecsetstr_flags;//每个指令的对应读写寄存器的str索引
extern vector<cs_insn*> g_vec_cs_insn;;//每个指令的对应读写寄存器的str索引
extern vector<int> g_vecb_is_useful;//有用的指令的标记
extern int gi_num_useful_instruction;//有用的指令数量的标记
extern DWORD g_dwIndex; //记录是第几块


namespace LXY {
	string setFlags(cs_insn* insn);
}

///////////////////////////////////////////////////////////////////////
//环境初始化
unordered_map<string, int> g_reg_type; //用于查询寄存器类型


int* ga_i_index;//有用的指令的索引
bool* ga_b_is_vmpcontrol_flow;

cs_insn* g_insn_end = NULL;
///////////////////////////////////////////////////////////////////////


void e_Mark_control_flow_instruction_init() {

#if 1
	g_reg_type.emplace("eax", 0x10);
	g_reg_type.emplace("edx", 0x10);
	g_reg_type.emplace("ecx", 0x10);
	g_reg_type.emplace("ebx", 0x10);
#endif //eax edx ecx ebx

#if 1
	g_reg_type.emplace("ax", 0x11);
	g_reg_type.emplace("dx", 0x11);
	g_reg_type.emplace("cx", 0x11);
	g_reg_type.emplace("bx", 0x11);
#endif //ax dx cx bx

#if 1
	g_reg_type.emplace("al", 0x12);
	g_reg_type.emplace("dl", 0x12);
	g_reg_type.emplace("cl", 0x12);
	g_reg_type.emplace("bl", 0x12);
#endif //al dl cl bl

#if 1
	g_reg_type.emplace("ah", 0x13);
	g_reg_type.emplace("dh", 0x13);
	g_reg_type.emplace("ch", 0x13);
	g_reg_type.emplace("bh", 0x13);
#endif //ah dh ch bh
	/*以上为eax ebx ecx edx 系*/

#if 1
	g_reg_type.emplace("esp", 0x20);
	g_reg_type.emplace("ebp", 0x20);
	g_reg_type.emplace("esi", 0x20);
	g_reg_type.emplace("edi", 0x20);
#endif //esp ebp esi edi

#if 1
	g_reg_type.emplace("sp", 0x21);
	g_reg_type.emplace("bp", 0x21);
	g_reg_type.emplace("si", 0x21);
	g_reg_type.emplace("di", 0x21);
#endif //esp ebp esi edi
	/*以上为esp ebp esi edi系*/
}

///////////////////////////////////////////////////////////////////////


static inline bool is_ret(cs_insn* cs) {
	if (strcmp(cs->mnemonic, "ret") == 0) {
		return true;
	}
	return false;
}
static inline bool is_indirect_jump(cs_insn* cs) {
	if (strcmp(cs->mnemonic, "jmp") == 0 && (cs->detail->x86.operands[0].type != X86_OP_IMM)) {
		return true;
	}
	return false;
}

/// <summary>
/// 递归进行判断是否为vmp控制流
/// </summary>
/// <param name="set_str_flag"></param>
/// <param name="i_index"></param>
void m_dg_finc_vmpcontrol_flow(set<string>& set_str_flag, int i_index);

void e_fenxi_kongzhiliu(cs_insn *insn) {
	if (g_insn_end != NULL) {
		cs_free(g_insn_end,1);//释放之前的最后一个指令
	}
	g_insn_end = insn; //保存最后一个指令
	e_print_deconfuse_result(g_dwIndex++);//保存去混淆结果

	ga_i_index = new int[gi_num_useful_instruction];
	ga_b_is_vmpcontrol_flow = new bool[gi_num_useful_instruction];
	for (int i = g_vecb_is_useful.size() - 1,j=0; i != -1; --i) {
		if (g_vecb_is_useful[i] == 1) {
			ga_b_is_vmpcontrol_flow[j] = false;//现在全部应该不是vmp控制流
			ga_i_index[j++] = i;
		}
	}
	/*a_i_index 为从下到上 有用指令的索引数组*/

	set<string> set_str_flag;
	char ac_buffer[0x10];

	if (is_indirect_jump(insn)) {
		string str_shuxing = LXY::setFlags(insn);

		for (size_t i = 0; i < sizeof(asz_reg_names) / sizeof(asz_reg_names[0]); ++i)
		{
			if (str_shuxing[2 * i] != '0')
			{
				sprintf_s(ac_buffer, "%s", asz_reg_names[i]);
				set_str_flag.emplace(ac_buffer);
			}//只用关心读取
		}
		//处理分析最后的跳转指令的读取
	}
	else if (is_ret(insn)) {

		for (int i = 0; i < gi_num_useful_instruction; ++i) {
			auto ins = g_vec_cs_insn[ga_i_index[i]];
			if (strcmp(ins->mnemonic, "push") == 0) {
				if (ins->detail->x86.operands[0].type == X86_OP_IMM)
				{
					goto _exit;
				}
				else {
					sprintf_s(ac_buffer, "%s", g_vec_cs_insn[ga_i_index[i]]->op_str);
					ga_b_is_vmpcontrol_flow[i] = true;
					set_str_flag.emplace(ac_buffer);
					break;//找到第一个 就退出了
				}
			}//找到最后的一个push指令 ，其寄存器就是读写
		}
	}
	else {
		goto _exit;
	}

	m_dg_finc_vmpcontrol_flow(set_str_flag,0);//递归进行查询
	e_print_result_vmpcontrol_flow(g_dwIndex - 1);
	_exit:
	delete[] ga_i_index;
	delete[] ga_b_is_vmpcontrol_flow;
}
/////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 寻找污点传播 str_reg为污点寄存器 i_index简介指向要搜索的指令
/// </summary>
/// <param name="set_str_flag"></param>
/// <param name="str_reg"></param>
/// <param name="i_index"></param>
/// <returns>该指令是否与str_reg相关</returns>
inline bool m_excfind(set<string>& set_str_flag,string str_reg,int i_index) {
	vector<string> vec_str_find;
	switch (g_reg_type[str_reg])
	{
	case 0x10: {
		string c_temp = str_reg.substr(1,1);//a c d b
		vec_str_find = std::move(vector<string>{
			string("e" + c_temp + "x_w"), string(c_temp + "x_w"),
				string(c_temp + "h_w"), string( c_temp + "l_w"),
		});
	}break;
	case 0x11: {
		string c_temp = str_reg.substr(0, 1);//a c d b
		vec_str_find = std::move(vector<string>{
			string("e" + c_temp + "x_w"), string(c_temp + "x_w"),
				string(c_temp + "h_w"), string("e" + c_temp + "l_w"),
		});
	}break;
	case 0x12: {
		string c_temp = str_reg.substr(0, 1);//a c d b
		vec_str_find = std::move(vector<string>{
			string("e" + c_temp + "x_w"), string(c_temp + "x_w"),
			string("e" + c_temp + "l_w"),
		});
	}break;
	case 0x13: {
		string c_temp = str_reg.substr(0, 1);//a c d b
		vec_str_find = std::move(vector<string>{
			string("e" + c_temp + "x_w"), string(c_temp + "x_w"),
				string("e" + c_temp + "h_w"),
		});
	}break;
	case 0x20: {
		string c_temp = str_reg.substr(1, 2);//a c d b
		vec_str_find = std::move(vector<string>{
			string("e" + c_temp + "_w"), string(c_temp + "_w"),});
	}break;
	case 0x21: {
		string c_temp = str_reg.substr(0, 2);//a c d b
		vec_str_find = std::move(vector<string>{
			string("e" + c_temp + "_w"), string(c_temp + "_w"),});
	}break;
	default:
		break;
	}//获取到所有可能影响这个的寄存器操作
	int i_index0 = ga_i_index[i_index];
	auto set = g_vecsetstr_flags[i_index0];
	for (auto value : vec_str_find) {
		if (set.find(value) != set.end()) {
#if PRRINT_MARK_VMPCONFUSE
			printf("finded:\r\n");
			printf("%s,", value.c_str());
			printf("\r\n");
#endif
			set_str_flag.erase(str_reg);//将该查询删除
			if (set.find("mem_r") != set.end()) {
				auto ins = g_vec_cs_insn[i_index0];
				if (ins->detail->x86.operands[1].mem.base == X86_REG_ESP) {
					return true;
				}
			}
			ga_b_is_vmpcontrol_flow[i_index] = true; //该指令是vmp控制流的指令
			for (auto value2 : set) {
				int i_size = value2.size();
				if (value2[i_size - 1] == 'r') {
					auto str_temp123 = value2.substr(0, i_size - 2);
					if (str_temp123 != "mem" && str_temp123 != "eflag") { //不考虑内存读和eflag读
						set_str_flag.emplace(str_temp123);
					}
				}
			}//将新的读取加入vec_str_temp 中

			return true; //找到一个就退出了 因为一般是写一小类寄存器中的一个
		}
	}
	return false;//该指令关于str_reg无关
}

/// <summary>
/// dg进行vmp控制流的分析
/// </summary>
/// <param name="set_str_flag"></param>
/// <param name="i_index"></param>
void m_dg_finc_vmpcontrol_flow(set<string>& set_str_flag, int i_index) {
	if (set_str_flag.empty() || i_index == gi_num_useful_instruction) {
		return;
	}//如果set_str_flag 没有元素了 或者到达了最前面的指令 结束递归

#if PRRINT_MARK_VMPCONFUSE
	system("cls");

	string strTemp = to_string(ga_i_index[i_index]);
	strTemp += ": ";
	strTemp += g_vec_cs_insn[ga_i_index[i_index]]->mnemonic;
	strTemp += " ";
	strTemp += g_vec_cs_insn[ga_i_index[i_index]]->op_str;
	printf("%-40s\r\n", strTemp.c_str(), 40);

	printf("from:\r\n");
	auto set = g_vecsetstr_flags[ga_i_index[i_index]];
	for (auto value : set) {
		printf("%s,", value.c_str());
	}
	printf("\r\n");

	printf("find:\r\n");
	for (auto value : set_str_flag) {
		printf("%s,", value.c_str());
	}
	printf("\r\n");
#endif
	for (auto value : set_str_flag) {
		if (m_excfind(set_str_flag, value, i_index))/*查看这层的指令*/{
			break; //因为找到一个写的就可以了  很少有使用写两个元素的指令
		}
	}

	m_dg_finc_vmpcontrol_flow(set_str_flag, i_index+1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
///打印结果

void e_print_deconfuse_result(DWORD dwIndex) {

#define	FILE(dwIndex) sprintf_s(ac_buffer,"OutPutFile\\confuse_result\\%d.txt",dwIndex)

	char ac_buffer[MAX_PATH];
	FILE(dwIndex);
	FILE* p_file = NULL;
	auto error = fopen_s(&p_file, ac_buffer, "w");
	if (error != 0)
		return;

	for (int i = 0; i < g_vecsetstr_flags.size(); ++i)
	{
		if (g_vecb_is_useful[i] != 1)
			continue;
		string strTemp = to_string(i);
		strTemp += ": ";
		strTemp += g_vec_cs_insn[i]->mnemonic;
		strTemp += " ";
		strTemp += g_vec_cs_insn[i]->op_str;
		fprintf_s(p_file, "%-40s\r\n", strTemp.c_str(), 40);

		for (auto value : g_vecsetstr_flags[i])
		{
			fprintf(p_file, "<%s>", value.c_str(), 10);
		}
		fprintf(p_file, "\r\n");
	}

	string strTemp = "end:";
	strTemp += ": ";
	strTemp += g_insn_end->mnemonic;
	strTemp += " ";
	strTemp += g_insn_end->op_str;
	fprintf_s(p_file, "%-40s\r\n", strTemp.c_str(), 40);

	fclose(p_file);
}


void  e_print_result_vmpcontrol_flow(DWORD dwIndex) {
	static FILE* p_file_useful_instruction = NULL;
#define	FILE1() sprintf_s(ac_buffer_useful_instruction,"OutPutFile\\useful_instruction\\result.txt")
#define	FILE2(dwIndex) sprintf_s(ac_buffer_vmp_control_flow,"OutPutFile\\vmp_control_flow\\%d.txt",dwIndex)
#define	FILE3(dwIndex) sprintf_s(ac_buffer_useful_instruction_index,"OutPutFile\\useful_instruction\\%d.txt",dwIndex)

	if (p_file_useful_instruction == NULL) {
		char ac_buffer_useful_instruction[MAX_PATH];
		FILE1();
		auto error = fopen_s(&p_file_useful_instruction, ac_buffer_useful_instruction, "w");
		if (error != 0) {
			MD_Throw_Runtime(fopen_s error);
		}
	}
	else {
		if (dwIndex == (DWORD)-1) {
			fclose(p_file_useful_instruction);
			p_file_useful_instruction = NULL;
			return;
		}
	}
	char ac_buffer_vmp_control_flow[MAX_PATH];
	char ac_buffer_useful_instruction_index[MAX_PATH];
	FILE* p_file = NULL;
	FILE* p_file2 = NULL;
	FILE2(dwIndex);
	FILE3(dwIndex);
	auto error = fopen_s(&p_file, ac_buffer_vmp_control_flow, "w");
	if (error != 0) {
		MD_Throw_Runtime(fopen_s error);
	}
	error = fopen_s(&p_file2, ac_buffer_useful_instruction_index, "w");
	if (error != 0) {
		MD_Throw_Runtime(fopen_s error);
	}

	for (int i = gi_num_useful_instruction-1; i!= -1; --i)
	{
		auto i1 = ga_i_index[i];
		string strTemp = to_string(i1);
		strTemp += ": ";
		strTemp += g_vec_cs_insn[i1]->mnemonic;
		strTemp += " ";
		strTemp += g_vec_cs_insn[i1]->op_str;

		if (ga_b_is_vmpcontrol_flow[i]) {

			fprintf_s(p_file, "%-40s\r\n", strTemp.c_str(), 40);
			for (auto value : g_vecsetstr_flags[i1])
			{
				fprintf(p_file, "<%s>", value.c_str(), 10);
			}
			fprintf(p_file, "\r\n");
		}
		else {
			fprintf_s(p_file_useful_instruction, "%-40s\r\n", strTemp.c_str(), 40);
			fprintf_s(p_file2, "%-40s\r\n", strTemp.c_str(), 40);

			for (auto value : g_vecsetstr_flags[i1])
			{
				fprintf(p_file2, "<%s>", value.c_str(), 10);
			}
			fprintf(p_file2, "\r\n");
		}
	}
	fprintf_s(p_file_useful_instruction,"-----------------------------------------------------------------------\r\n");

	string strTemp = "end:";
	strTemp += ": ";
	strTemp += g_insn_end->mnemonic;
	strTemp += " ";
	strTemp += g_insn_end->op_str;
	fprintf_s(p_file, "%-40s\r\n", strTemp.c_str(), 40);


	fclose(p_file);
	fclose(p_file2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////