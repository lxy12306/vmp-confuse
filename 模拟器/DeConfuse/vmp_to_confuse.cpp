#include "vmp_to_confuse.h"
#include <iostream>
#include <string>
#include "..\CommonFiles\LXY\VC++\Exception\MyExcetpionCpp.h"
#include <bitset>

#define PRINT 3 //�Ƿ��ӡ��Ϣ �Թ����� 2��ʾ�ļ���Ӯ
#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(#szError,__FILE__,__LINE__)    
#define FOR_EACH_REGS(i) for(i = 0;i<(sizeof(asz_reg_names)/sizeof(asz_reg_names[0])-1);++i)
#define FOR_EACH_INSTURCTION(i) for(i = g_vecsetstr_flags.size()-1;i>=0;++i)
#define DELETE_INVALID_VISIT(i,str)	g_vecsetstr_flags[(i)].erase((str))

#define DEBUG_FILE 

using namespace std;
///////////////////////////////////////////////////////////////////////////////////////////
//ȥ��������ȫ�ֱ���

vector<int> g_vecb_is_useful;//���õ�ָ��ı��
vector<string> g_vecstr_flags;
LXY::Regs_Find g_regs_find;//ͳ�ƺ͹����ַ��������Լ���Ӧָ�����
int gi_num_useful_instruction;//���õ�ָ�������ı��



vector<set<string>> g_vecsetstr_flags;//ÿ��ָ��Ķ�Ӧ��д�Ĵ�����str����
map<string, set<int>> g_map_str_seti_flags;/*������϶�̫�� ����ʹ��map ʹ���Զ���ṹ��g_regs_find���*/
// 
///////////////////////////////////////////////////////////////////////////////////////////
//��־ȫ�ֱ���

DWORD g_dwIndex = 1; //��¼�ǵڼ���

///////////////////////////////////////////////////////////////////////////////////////////


extern const char* asz_reg_names[] =
{
	"al",
	"ah",
	"ax",
	"eax",
	"cl",
	"ch",
	"cx",
	"ecx",
	"dl",
	"dh",
	"dx",
	"edx",
	"bl",
	"bh",
	"bx",
	"ebx",
	"si",
	"esi",
	"di",
	"edi",
	"sp",
	"esp",
	"bp",
	"ebp",
	"eflag",
	"mem"
};


namespace LXY
{
	string setFlags(cs_insn* insn)
	{

		bitset<0x80> bitsetRW;
		bitsetRW.reset();
		string strCout(52, '0');

		for (auto value : insn->detail->regs_read)
		{
			if (value == 0)
				break;
			if (value <= 0x40)
				bitsetRW[(value - 1) * 2] = 1;
		}


		for (auto value : insn->detail->regs_write)
		{
			if (value == 0)
				break;
			if (value <= 0x40)
				bitsetRW[(value - 1) * 2 + 1] = 1;
		}
		if (strstr(insn->mnemonic, "cmov"))
		{
			bitsetRW[(insn->detail->x86.operands[0].reg - 1) * 2 + 1] = 1; //��һ���Ĵ���Ϊд
			bitsetRW[(insn->detail->x86.operands[1].reg - 1) * 2] = 1; //�ڶ����Ĵ���Ϊ��
		}
		else
		{
			for (auto value : insn->detail->x86.operands)
			{
				if (value.type == X86_OP_INVALID)
					break;
				if (value.type == X86_OP_REG)
				{

					if (value.access & 1) //������
						bitsetRW[(value.reg - 1) * 2] = 1;
					if (value.access & 2) //д����
						bitsetRW[(value.reg - 1) * 2 + 1] = 1;
				}
				if (value.type == X86_OP_MEM)
				{
					if (!strstr(insn->mnemonic, "lea"))
					{
						if (value.access & 1) //������
							strCout[50] = 'r';
						if (value.access & 2) //д����
							strCout[51] = 'w';
					}

					if (value.mem.base != X86_REG_INVALID)
					{
						bitsetRW[(value.mem.base - 1) * 2] = 1;
					}
					if (value.mem.index != X86_REG_INVALID)
					{
						bitsetRW[(value.mem.index - 1) * 2] = 1;
					}
				}
			}
		}

		if (strcmp(insn->mnemonic, "push") == 0)
		{
			strCout[51] = 'w';
		}

		if (strstr(insn->mnemonic, "test"))
		{
			//__asm int 3;
		}

		for (size_t i = 0; i < 0x80; i++)
		{
			switch (i)
			{
			case 2:
			{
				strCout[0] = bitsetRW.test(i) ? 'r' : '0'; //al �Ķ�
			}break;
			case 3:
			{
				strCout[1] = bitsetRW.test(i) ? 'w' : '0';//al ��д
			}break;
			case 0:
			{
				strCout[2] = bitsetRW.test(i) ? 'r' : '0'; //ah �Ķ�
			}break;
			case 1:
			{
				strCout[3] = bitsetRW.test(i) ? 'w' : '0';//ah ��д
			}break;
			case 4:
			{
				strCout[4] = bitsetRW.test(i) ? 'r' : '0'; //ax �Ķ�
			}break;
			case 5:
			{
				strCout[5] = bitsetRW.test(i) ? 'w' : '0';//ax ��д
			}break;
			case 36:
			{
				strCout[6] = bitsetRW.test(i) ? 'r' : '0';//eax �Ķ�
			}break;
			case 37:
			{
				strCout[7] = bitsetRW.test(i) ? 'w' : '0';//eax ��д
			}break;
			case 18:
			{
				strCout[8] = bitsetRW.test(i) ? 'r' : '0'; //cl �Ķ�
			}break;
			case 19:
			{
				strCout[9] = bitsetRW.test(i) ? 'w' : '0';//cl ��д
			}break;
			case 16:
			{
				strCout[10] = bitsetRW.test(i) ? 'r' : '0'; //ch �Ķ�
			}break;
			case 17:
			{
				strCout[11] = bitsetRW.test(i) ? 'w' : '0';//ch ��д
			}break;
			case 22:
			{
				strCout[12] = bitsetRW.test(i) ? 'r' : '0'; //cx �Ķ�
			}break;
			case 23:
			{
				strCout[13] = bitsetRW.test(i) ? 'w' : '0';//cx ��д
			}break;
			case 42:
			{
				strCout[14] = bitsetRW.test(i) ? 'r' : '0';//ecx �Ķ�
			}break;
			case 43:
			{
				strCout[15] = bitsetRW.test(i) ? 'w' : '0';//ecx ��д
			}break;
			case 30:
			{
				strCout[16] = bitsetRW.test(i) ? 'r' : '0'; //dl �Ķ�
			}break;
			case 31:
			{
				strCout[17] = bitsetRW.test(i) ? 'w' : '0';//dl ��д
			}break;
			case 24:
			{
				strCout[18] = bitsetRW.test(i) ? 'r' : '0'; //dh �Ķ�
			}break;
			case 25:
			{
				strCout[19] = bitsetRW.test(i) ? 'w' : '0';//dh ��д
			}break;
			case 34:
			{
				strCout[20] = bitsetRW.test(i) ? 'r' : '0'; //dx �Ķ�
			}break;
			case 35:
			{
				strCout[21] = bitsetRW.test(i) ? 'w' : '0';//dx ��д
			}break;
			case 46:
			{
				strCout[22] = bitsetRW.test(i) ? 'r' : '0';//edx �Ķ�
			}break;
			case 47:
			{
				strCout[23] = bitsetRW.test(i) ? 'w' : '0';//edx ��д
			}break;
			case 8:
			{
				strCout[24] = bitsetRW.test(i) ? 'r' : '0'; //bl �Ķ�
			}break;
			case 9:
			{
				strCout[25] = bitsetRW.test(i) ? 'w' : '0';//bl ��д
			}break;
			case 6:
			{
				strCout[26] = bitsetRW.test(i) ? 'r' : '0'; //bh �Ķ�
			}break;
			case 7:
			{
				strCout[27] = bitsetRW.test(i) ? 'w' : '0';//bh ��д
			}break;
			case 14:
			{
				strCout[28] = bitsetRW.test(i) ? 'r' : '0'; //bx �Ķ�
			}break;
			case 15:
			{
				strCout[29] = bitsetRW.test(i) ? 'w' : '0';//bx ��д
			}break;
			case 40:
			{
				strCout[30] = bitsetRW.test(i) ? 'r' : '0';//ebx �Ķ�
			}break;
			case 41:
			{
				strCout[31] = bitsetRW.test(i) ? 'w' : '0';//ebx ��д
			}break;
			case 88:
			{
				strCout[32] = bitsetRW.test(i) ? 'r' : '0'; //si �Ķ�
			}break;
			case 89:
			{
				strCout[33] = bitsetRW.test(i) ? 'w' : '0';//si ��д
			}break;
			case 56:
			{
				strCout[34] = bitsetRW.test(i) ? 'r' : '0';//esi �Ķ�
			}break;
			case 57:
			{
				strCout[35] = bitsetRW.test(i) ? 'w' : '0';//esi ��д
			}break;
			case 26:
			{
				strCout[36] = bitsetRW.test(i) ? 'r' : '0'; //di �Ķ�
			}break;
			case 27:
			{
				strCout[37] = bitsetRW.test(i) ? 'w' : '0';//di ��д
			}break;
			case 44:
			{
				strCout[38] = bitsetRW.test(i) ? 'r' : '0'; //edi �Ķ�
			}break;
			case 45:
			{
				strCout[39] = bitsetRW.test(i) ? 'w' : '0';//edi ��д
			}break;
			case 92:
			{
				strCout[40] = bitsetRW.test(i) ? 'r' : '0'; //sp �Ķ�
			}break;
			case 93:
			{
				strCout[41] = bitsetRW.test(i) ? 'w' : '0';//sp ��д
			}break;
			case 58:
			{
				strCout[42] = bitsetRW.test(i) ? 'r' : '0';//esp �Ķ�
			}break;
			case 59:
			{
				strCout[43] = bitsetRW.test(i) ? 'w' : '0';//esp ��д
			}break;
			case 10:
			{
				strCout[44] = bitsetRW.test(i) ? 'r' : '0'; //bp �Ķ�
			}break;
			case 11:
			{
				strCout[45] = bitsetRW.test(i) ? 'w' : '0';//bp ��д
			}break;
			case 38:
			{
				strCout[46] = bitsetRW.test(i) ? 'r' : '0'; //ebp �Ķ�
			}break;
			case 39:
			{
				strCout[47] = bitsetRW.test(i) ? 'w' : '0';//ebp ��д
			}break;

			case 48:
			{
				strCout[48] = bitsetRW.test(i) ? 'r' : '0';//eflag �Ķ�
			}break;
			case 49:
			{
				strCout[49] = bitsetRW.test(i) ? 'w' : '0';//eflag ��д
			}break;
			default:
				break;
			}
		}


		return strCout;
	}

	void showReadWrite(string& strCout, int iChoice = -1)
	{
		if (iChoice == -1)
		{
			for (size_t i = 0; i < sizeof(asz_reg_names) / sizeof(asz_reg_names[0]); ++i)
			{
				if (strCout[2 * i] != '0' || strCout[2 * i + 1] != '0')
					printf(" %s:<%c%c> ", asz_reg_names[i], strCout[2 * i], strCout[2 * i + 1]);
			}
			printf("\n");
		}
		else if (iChoice >= 0 && iChoice < sizeof(asz_reg_names) / sizeof(asz_reg_names[0]))
		{
			if (strCout[2 * iChoice] != '0' || strCout[2 * iChoice + 1] != '0')
				printf("            %s:<%c%c> ", asz_reg_names[iChoice], strCout[2 * iChoice], strCout[2 * iChoice + 1]);
		}
	}
	void showReadWrite2(const char* sz_show_index = NULL)
	{
		if (sz_show_index == NULL)
		{
			for (auto value : g_map_str_seti_flags)
			{
				if (!value.second.empty())
				{
					printf("\r\n");
					cout << value.first << ":\r\n";
					for (auto value2 : value.second)
					{
						string strTemp = to_string(value2);
						strTemp += ": ";
						strTemp += g_vec_cs_insn[value2]->mnemonic;
						strTemp += " ";
						strTemp += g_vec_cs_insn[value2]->op_str;
						printf("%-40s", strTemp.c_str());
					}
					printf("\r\n");
				}
			}
		}
		else
		{
			for (auto value : g_map_str_seti_flags)
			{
				if (!value.second.empty())
				{
					if (value.first.find(sz_show_index) == string::npos)
						continue;
					printf("\r\n");
					cout << value.first << ":\r\n";
					for (auto value2 : value.second)
					{
						string strTemp = to_string(value2);
						strTemp += ": ";
						strTemp += g_vec_cs_insn[value2]->mnemonic;
						strTemp += " ";
						strTemp += g_vec_cs_insn[value2]->op_str;
						printf("%-40s", strTemp.c_str());
					}
					printf("\r\n");
				}
			}
		}
	
	}
	
	/// <summary>
	/// ������ȥ�������ָ����ʾ����
	/// </summary>
	/// <param name="sz_file_name"></param>
	void show_g_vecsetstr_flags(const char* sz_file_name = "g_vecsetstr_flags.txt")
	{
		FILE* p_file = NULL;
		auto error =  fopen_s(&p_file, sz_file_name, "w");
		if (error != 0)
			return;
		for (int i = 0; i < g_vecsetstr_flags.size(); ++i)
		{
			if (g_vecsetstr_flags[i].empty())
				continue;
			string strTemp = to_string(i);
			strTemp += ": ";
			strTemp += g_vec_cs_insn[i]->mnemonic;
			strTemp += " ";
			strTemp += g_vec_cs_insn[i]->op_str;
			fprintf_s(p_file,"%-40s\r\n", strTemp.c_str(),40);

			for (auto value : g_vecsetstr_flags[i])
			{
				fprintf(p_file,"<%s>", value.c_str(),10);
			}
			fprintf(p_file, "\r\n");
		}
		fclose(p_file);
	}

	void fix_flags_map() //�ϲ�map�ļĴ���
	{
#if 1
		//eax ax al ah
		g_map_str_seti_flags["eax_r"].insert(g_map_str_seti_flags["al_r"].begin(), g_map_str_seti_flags["al_r"].end());
		g_map_str_seti_flags["eax_r"].insert(g_map_str_seti_flags["ah_r"].begin(), g_map_str_seti_flags["ah_r"].end());
		g_map_str_seti_flags["eax_r"].insert(g_map_str_seti_flags["ax_r"].begin(), g_map_str_seti_flags["ax_r"].end());
		g_map_str_seti_flags["ax_r"].insert(g_map_str_seti_flags["al_r"].begin(), g_map_str_seti_flags["al_r"].end());
		g_map_str_seti_flags["ax_r"].insert(g_map_str_seti_flags["ah_r"].begin(), g_map_str_seti_flags["ah_r"].end());

		g_map_str_seti_flags["eax_w"].insert(g_map_str_seti_flags["al_w"].begin(), g_map_str_seti_flags["al_w"].end());
		g_map_str_seti_flags["eax_w"].insert(g_map_str_seti_flags["ah_w"].begin(), g_map_str_seti_flags["ah_w"].end());
		g_map_str_seti_flags["eax_w"].insert(g_map_str_seti_flags["ax_w"].begin(), g_map_str_seti_flags["ax_w"].end());
		g_map_str_seti_flags["ax_w"].insert(g_map_str_seti_flags["al_w"].begin(), g_map_str_seti_flags["al_w"].end());
		g_map_str_seti_flags["ax_w"].insert(g_map_str_seti_flags["ah_w"].begin(), g_map_str_seti_flags["ah_w"].end());
#endif
#if 1
		//ecx cx cl ch
		g_map_str_seti_flags["ecx_r"].insert(g_map_str_seti_flags["cl_r"].begin(), g_map_str_seti_flags["cl_r"].end());
		g_map_str_seti_flags["ecx_r"].insert(g_map_str_seti_flags["ch_r"].begin(), g_map_str_seti_flags["ch_r"].end());
		g_map_str_seti_flags["ecx_r"].insert(g_map_str_seti_flags["cx_r"].begin(), g_map_str_seti_flags["cx_r"].end());
		g_map_str_seti_flags["cx_r"].insert(g_map_str_seti_flags["cl_r"].begin(), g_map_str_seti_flags["cl_r"].end());
		g_map_str_seti_flags["cx_r"].insert(g_map_str_seti_flags["ch_r"].begin(), g_map_str_seti_flags["ch_r"].end());

		g_map_str_seti_flags["ecx_w"].insert(g_map_str_seti_flags["cl_w"].begin(), g_map_str_seti_flags["cl_w"].end());
		g_map_str_seti_flags["ecx_w"].insert(g_map_str_seti_flags["ch_w"].begin(), g_map_str_seti_flags["ch_w"].end());
		g_map_str_seti_flags["ecx_w"].insert(g_map_str_seti_flags["cx_w"].begin(), g_map_str_seti_flags["cx_w"].end());
		g_map_str_seti_flags["cx_w"].insert(g_map_str_seti_flags["cl_w"].begin(), g_map_str_seti_flags["cl_w"].end());
		g_map_str_seti_flags["cx_w"].insert(g_map_str_seti_flags["ch_w"].begin(), g_map_str_seti_flags["ch_w"].end());
#endif
#if 1
		//edx dx dl dh
		g_map_str_seti_flags["edx_r"].insert(g_map_str_seti_flags["dl_r"].begin(), g_map_str_seti_flags["dl_r"].end());
		g_map_str_seti_flags["edx_r"].insert(g_map_str_seti_flags["dh_r"].begin(), g_map_str_seti_flags["dh_r"].end());
		g_map_str_seti_flags["edx_r"].insert(g_map_str_seti_flags["dx_r"].begin(), g_map_str_seti_flags["dx_r"].end());
		g_map_str_seti_flags["dx_r"].insert(g_map_str_seti_flags["dl_r"].begin(), g_map_str_seti_flags["dl_r"].end());
		g_map_str_seti_flags["dx_r"].insert(g_map_str_seti_flags["dh_r"].begin(), g_map_str_seti_flags["dh_r"].end());

		g_map_str_seti_flags["edx_w"].insert(g_map_str_seti_flags["dl_w"].begin(), g_map_str_seti_flags["dl_w"].end());
		g_map_str_seti_flags["edx_w"].insert(g_map_str_seti_flags["dh_w"].begin(), g_map_str_seti_flags["dh_w"].end());
		g_map_str_seti_flags["edx_w"].insert(g_map_str_seti_flags["dx_w"].begin(), g_map_str_seti_flags["dx_w"].end());
		g_map_str_seti_flags["dx_w"].insert(g_map_str_seti_flags["dl_w"].begin(), g_map_str_seti_flags["dl_w"].end());
		g_map_str_seti_flags["dx_w"].insert(g_map_str_seti_flags["dh_w"].begin(), g_map_str_seti_flags["dh_w"].end());
#endif
#if 1
		//ebx bx bl bh
		g_map_str_seti_flags["ebx_r"].insert(g_map_str_seti_flags["bl_r"].begin(), g_map_str_seti_flags["bl_r"].end());
		g_map_str_seti_flags["ebx_r"].insert(g_map_str_seti_flags["bh_r"].begin(), g_map_str_seti_flags["bh_r"].end());
		g_map_str_seti_flags["ebx_r"].insert(g_map_str_seti_flags["bx_r"].begin(), g_map_str_seti_flags["bx_r"].end());
		g_map_str_seti_flags["bx_r"].insert(g_map_str_seti_flags["bl_r"].begin(), g_map_str_seti_flags["bl_r"].end());
		g_map_str_seti_flags["bx_r"].insert(g_map_str_seti_flags["bh_r"].begin(), g_map_str_seti_flags["bh_r"].end());

		g_map_str_seti_flags["ebx_w"].insert(g_map_str_seti_flags["bl_w"].begin(), g_map_str_seti_flags["bl_w"].end());
		g_map_str_seti_flags["ebx_w"].insert(g_map_str_seti_flags["bh_w"].begin(), g_map_str_seti_flags["bh_w"].end());
		g_map_str_seti_flags["ebx_w"].insert(g_map_str_seti_flags["bx_w"].begin(), g_map_str_seti_flags["bx_w"].end());
		g_map_str_seti_flags["bx_w"].insert(g_map_str_seti_flags["bl_w"].begin(), g_map_str_seti_flags["bl_w"].end());
		g_map_str_seti_flags["bx_w"].insert(g_map_str_seti_flags["bh_w"].begin(), g_map_str_seti_flags["bh_w"].end());
#endif

#if 1
	g_map_str_seti_flags["esi_r"].insert(g_map_str_seti_flags["si_r"].begin(), g_map_str_seti_flags["si_r"].end());
	g_map_str_seti_flags["esi_w"].insert(g_map_str_seti_flags["si_w"].begin(), g_map_str_seti_flags["si_w"].end());

	g_map_str_seti_flags["edi_r"].insert(g_map_str_seti_flags["di_r"].begin(), g_map_str_seti_flags["di_r"].end());
	g_map_str_seti_flags["edi_w"].insert(g_map_str_seti_flags["di_w"].begin(), g_map_str_seti_flags["di_w"].end());

	g_map_str_seti_flags["esp_r"].insert(g_map_str_seti_flags["sp_r"].begin(), g_map_str_seti_flags["sp_r"].end());
	g_map_str_seti_flags["esp_w"].insert(g_map_str_seti_flags["sp_w"].begin(), g_map_str_seti_flags["sp_w"].end());

	g_map_str_seti_flags["ebp_r"].insert(g_map_str_seti_flags["bp_r"].begin(), g_map_str_seti_flags["bp_r"].end());
	g_map_str_seti_flags["ebp_w"].insert(g_map_str_seti_flags["bp_w"].begin(), g_map_str_seti_flags["bp_w"].end());
#endif
	}

	void analysisReadWrite(int i_index)
	{
		char ac_buffer[0x10];
		for (size_t i = 0; i < sizeof(asz_reg_names) / sizeof(asz_reg_names[0]); ++i)
		{
			if (g_vecstr_flags[i_index][2 * i] != '0')
			{
				sprintf_s(ac_buffer, "%s_r", asz_reg_names[i]);
				g_vecsetstr_flags[i_index].emplace(ac_buffer);
				g_map_str_seti_flags[ac_buffer].emplace(i_index);
			}
			if (g_vecstr_flags[i_index][2 * i + 1] != '0')
			{
				sprintf_s(ac_buffer, "%s_w", asz_reg_names[i]);
				g_vecsetstr_flags[i_index].emplace(ac_buffer);
				g_map_str_seti_flags[ac_buffer].emplace(i_index);
			}
		}
		printf("\n");
	}
	void analysisReadWrite2(int i_index)
	{
		char ac_buffer[0x10];
		for (size_t i = 0; i < sizeof(asz_reg_names) / sizeof(asz_reg_names[0]); ++i)
		{
			if (g_vecstr_flags[i_index][2 * i] != '0')
			{
				sprintf_s(ac_buffer, "%s_r", asz_reg_names[i]);
				g_vecsetstr_flags[i_index].emplace(ac_buffer);
				g_regs_find.e_insert(ac_buffer, i_index);
			}
			if (g_vecstr_flags[i_index][2 * i + 1] != '0')
			{
				sprintf_s(ac_buffer, "%s_w", asz_reg_names[i]);
				g_vecsetstr_flags[i_index].emplace(ac_buffer);
				g_regs_find.e_insert(ac_buffer, i_index);
			}
		}
	}
	void m_setFlags_and_Analysis()
	{
		g_vecstr_flags.resize(g_vec_cs_insn.size());
		for (int i = 0; i < g_vec_cs_insn.size(); ++i)
		{
			if (g_vecb_is_useful[i] == 1)
			{

				g_vecstr_flags[i] = std::move(setFlags(g_vec_cs_insn[i]));
				analysisReadWrite(i); //��������
#if 1
				string strTemp = g_vec_cs_insn[i]->mnemonic;
				strTemp += " ";
				strTemp += g_vec_cs_insn[i]->op_str;
				printf("%-40s", strTemp.c_str());
				showReadWrite(g_vecstr_flags[i]);
#endif//��Ӯ��д��־λ
			}
		}//ȷ����־
		fix_flags_map();
		showReadWrite2("a");
	}
	void m_setFlags_and_Analysis2()
	{
		g_vecstr_flags.resize(g_vec_cs_insn.size());
		for (int i = 0; i < g_vec_cs_insn.size(); ++i)
		{
			if (g_vecb_is_useful[i] ==1)
			{

				g_vecstr_flags[i] = std::move(setFlags(g_vec_cs_insn[i]));
				analysisReadWrite2(i); //��������
#if 1
				string strTemp = g_vec_cs_insn[i]->mnemonic;
				strTemp += " ";
				strTemp += g_vec_cs_insn[i]->op_str;
				printf("%-40s", strTemp.c_str());
				showReadWrite(g_vecstr_flags[i]);
#endif//��Ӯ��д��־λ
			}
		}//ȷ����־


	}

	/// <summary>
	/// ���д��дָ�����ض�д����
	/// </summary>
	void clear_invalid_instruction()
	{
		int i_index_max = g_vecb_is_useful.size();
		int i = 0;

		FOR_EACH_REGS(i)
		{
			string str_reg = asz_reg_names[i];
			if (str_reg == "di")
			{
#if _DEBUG
				system("cls");
				g_regs_find.e_printf(str_reg);
#endif^
				//__asm int 3;

			}
			string str_reg_r = str_reg + "_r";
			auto str_reg_w = str_reg + "_w";

			auto set_r = g_regs_find.e_find_left(str_reg_r, i_index_max);
			auto set_w = g_regs_find.e_find_left(str_reg_w, i_index_max);
			auto set_find = g_regs_find.e_ret_self(str_reg_w);
			while (true)
			{
				auto iter_end = set_find.end();
				if (iter_end == set_find.begin())
					break;//Ѱ������Ϊ�� �˳�ѭ��
				--iter_end;//��ȡset�����һ��Ԫ��
				auto iter_r = set_r.lower_bound(*iter_end);
				if (iter_r == set_r.end() )
				{
					if (set_w.upper_bound(*iter_end) != set_w.end())
					{
						g_regs_find.e_delete(str_reg_w, *iter_end); //û�ж�ȡָ�� ���Ƿ����һ��дָ�� ��Ҫ����eflag�Ĵ���
					}
					set_find.erase(iter_end);
					continue;
				}
				if (*iter_r != *iter_end)
				{
					decltype(iter_end) iter_w;
					if (iter_r == set_r.begin())
					{
						//ǰ���Ѿ�û�ж�ȡ���� ��ɾ��ǰ������ж�ȡ����
						iter_w = set_w.begin(); 
						if (*iter_w == *iter_end)
						{
							set_find.erase(iter_end);
							continue; //�����ʱw���Ѿ�û����Ԫ��  ��Ӧ��ɾ���˶�ȡ ������һ��ѭ����
						}
					}
					else {
						--iter_r;
						iter_w = set_w.lower_bound(*iter_r);
					}

					auto iter_w_end = set_w.find(*iter_end);
					if (iter_w == iter_w_end)
					{
						//��ָ��֮��û��ǰдָ��
						set_find.erase(iter_end);
						continue;
					}

					for(auto iter = iter_w; iter != iter_w_end; ++iter)
					{
						if (g_regs_find.e_delete(str_reg_w, *iter))
						{
							set_find.erase(*iter);
						}
						if (g_regs_find.e_delete(str_reg_r, *iter))
						{
							set_r.erase(iter_r);//�������ǰ��ָ����Ч ��ɾ���ö�ȡָ��
						}
#if _DEBUG
						g_regs_find.e_printf(str_reg);
#endif
					}
					//ɾ��ǰ�����Чд����
#if 0
					if (g_regs_find.e_delete(str_reg_w, *iter_end))
					{
						g_regs_find.e_printf(str_reg);
						DELETE_INVALID_VISIT(*iter_end, str_reg_w);
					}
					if (g_regs_find.e_delete(str_reg_r, *iter_end))
					{
						g_regs_find.e_printf(str_reg);
						DELETE_INVALID_VISIT(*iter_end, str_reg_w);
					}
					//ɾ�����ε���Ч������
#endif
					set_w.erase(iter_w, iter_w_end);//�������ǰдָ����Ч ��ɾ����Щдָ��
				}
				else {
					if (str_reg == "eflag")
					{
						g_regs_find.e_delete(str_reg_w, *iter_end); //û�ж�ȡָ�� ���Ƿ����һ��дָ�� ��Ҫ����eflag�Ĵ���
					}
					set_find.erase(iter_end);
				}
			}
#if _DEBUG
			g_regs_find.e_printf(str_reg);
#endif
		}
	}
	/// <summary>
	/// ���ֻ��ָ�� �������Ƿ���Ҫ����ȥ����
	/// </summary>
	/// <returns>bool true��ʾ��Ҫ����ȥ����</returns>
	bool m_fix_useful()
	{	
		int i_temp = gi_num_useful_instruction;

		for (int i = 0; i < g_vecb_is_useful.size(); ++i)
		{
			if (g_vecb_is_useful[i] == 1)
			{
				if (g_vecsetstr_flags[i].empty())
				{
					g_vecb_is_useful[i] = 0;
					gi_num_useful_instruction--;
				}
				else {
					bool b_useful = false;
					for (auto value : g_vecsetstr_flags[i])
					{
						if (value.find("_w") != string::npos)
						{
							b_useful = true;
							break; //��д������ ��ָ���������
						}
					}
					if (!b_useful)
					{
						g_vecsetstr_flags[i].clear();//�������
						g_vecb_is_useful[i] = 0; //ֻ��ָ����Ч
						gi_num_useful_instruction--; //����ָ��������1
					}
				}
			}
		}
		if (i_temp != gi_num_useful_instruction)
		{
			return true;
		}

		return false; //����ֵ�����жϻ��費��Ҫ����ȥ����
	}
	/// <summary>
	/// �ؽ�m_rebuild_g_regs_find ������ȥ��ֻ��ָ��֮���ָ�
	/// </summary>
	void m_rebuild_g_regs_find()
	{
		g_regs_find.e_clear();
		for (int i = 0; i < g_vec_cs_insn.size(); ++i)
		{
			if (g_vecb_is_useful[i] == 1)
			{
				for (auto value : g_vecsetstr_flags[i])
				{
					g_regs_find.e_insert(value, i);//���޲����дָ��
				}
			}
		}
	}

	/// <summary>
	/// ���һЩ�����ָ��
	/// </summary>
	void m_clean_special_ins();
	void m_clean_special_ins_when_end();
	void e_quhunxiao()
	{
		m_setFlags_and_Analysis2(); //��ʼ����־λ Ϊ��һ��������׼��
		//m_clean_special_ins();
#if PRINT&1
		system("cls");
#endif

#if PRINT&2
		system("cls");
		show_g_vecsetstr_flags();//��ӡ
#endif

		while (true)
		{
			clear_invalid_instruction();//���д��д ��ǰд����

			g_regs_find.e_printf_to_flags(g_vecsetstr_flags);//�ؽ�g_vecsetstr_flags ����д��д
			if (!m_fix_useful())
			{
				break;
				//���û������ ˵���Ѿ����ȥ����
			}//ȥ��g_vecsetstr_flags��ֻ��ָ��
#if PRINT&2
			show_g_vecsetstr_flags("g_vecsetstr_flags_change.txt");
#endif
			m_rebuild_g_regs_find();//�ؽ�g_regs_find �Թ���һ��ȥ����

		}


		m_clean_special_ins_when_end();
		//g_regs_find.e_clear(); //��ձ����g_regs_find Ϊ��һ����׼�� /*�޸ĵ���ʼ��ʱ����*/
	}

	int begin_analysis()
	{
		gi_num_useful_instruction = g_vec_cs_insn.size();
		g_vecb_is_useful.clear();
		g_vecb_is_useful.resize(gi_num_useful_instruction, 1);
		g_vecsetstr_flags.clear();
		g_vecsetstr_flags.resize(gi_num_useful_instruction);
		g_vecstr_flags.clear();
		g_regs_find.e_clear();
		//g_map_str_seti_flags.clear();����
		//��ʼ���ṹ��
		for (int i = 0;i< g_vec_cs_insn.size();++i)
		{
			if (is_special_instructions_join(g_vec_cs_insn[i]))
			{
				g_vecb_is_useful[i] = 2;
				--gi_num_useful_instruction;
			}
		}//jmpֱ����תָ��ȥ��
		e_quhunxiao();
		return 0;
	}
}

namespace LXY {

	static bool operator==(m_index& lhs, m_index& rhs)
	{
		return (lhs.i_index1 == rhs.i_index1) && (lhs.i_index2 == rhs.i_index2) && (lhs.i_index3 == rhs.i_index3);
	}

	int8_t Regs_Find::init_index()
	{
		int8_t i = 0;
#if 1
		m_map_str_index["eax_r"] = { i,0,-1 };
		m_map_str_index["ax_r"] = { i,1,-1 };
		m_map_str_index["al_r"] = { i,1,0 };
		m_map_str_index["ah_r"] = { i,1,1 };
		++i;
									  
		m_map_str_index["eax_w"] =	{ i,0,-1 };
		m_map_str_index["ax_w"] =	{ i,1,-1 };
		m_map_str_index["al_w"] =	{ i,1,0 };
		m_map_str_index["ah_w"] =	{ i,1,1 };
		++i;
#endif //eax						  
									  
#if 1								  
		m_map_str_index["edx_r"] =	{ i,0,-1 };
		m_map_str_index["dx_r"] =	{ i,1,-1 };
		m_map_str_index["dl_r"] =	{ i,1,0 };
		m_map_str_index["dh_r"] =	{ i,1,1 };
		++i;
									  
		m_map_str_index["edx_w"] =	{ i,0,-1 };
		m_map_str_index["dx_w"] =	{ i,1,-1 };
		m_map_str_index["dl_w"] =	{ i,1,0 };
		m_map_str_index["dh_w"] =	{ i,1,1 };
		++i;
#endif //edx						  
									  
#if 1								  
		m_map_str_index["ecx_r"] =	{ i,0,-1 };
		m_map_str_index["cx_r"] =	{ i,1,-1 };
		m_map_str_index["cl_r"] =	{ i,1,0 };
		m_map_str_index["ch_r"] =	{ i,1,1 };
		++i;
									  
		m_map_str_index["ecx_w"] =	{ i,0,-1 };
		m_map_str_index["cx_w"] =	{ i,1,-1 };
		m_map_str_index["cl_w"] =	{ i,1,0 };
		m_map_str_index["ch_w"] =	{ i,1,1 };
		++i;
#endif //ecx						  
									  
#if 1								  
		m_map_str_index["ebx_r"] =	{ i,0,-1 };
		m_map_str_index["bx_r"] =	{ i,1,-1 };
		m_map_str_index["bl_r"] =	{ i,1,0 };
		m_map_str_index["bh_r"] =	{ i,1,1 };
		++i;
									  
		m_map_str_index["ebx_w"] =	{ i,0,-1 };
		m_map_str_index["bx_w"] =	{ i,1,-1 };
		m_map_str_index["bl_w"] =	{ i,1,0 };
		m_map_str_index["bh_w"] =	{ i,1,1 };
		++i;
#endif //ebx						  
									  
#if 1								  
		m_map_str_index["esi_r"] =	{ i,0,-1 };
		m_map_str_index["si_r"] =	{ i,1,-1 };
		++i;
									  
		m_map_str_index["esi_w"] =	{ i,0,-1 };
		m_map_str_index["si_w"] =	{ i,1,-1 };
		++i;
#endif //esi						  
									  
#if 1								  
		m_map_str_index["edi_r"] =	{ i,0,-1 };
		m_map_str_index["di_r"] =	{ i,1,-1 };
		++i;
									  
		m_map_str_index["edi_w"] =	{ i,0,-1 };
		m_map_str_index["di_w"] =	{ i,1,-1 };
		++i;
#endif //edi						  
									  
#if 1								  
		m_map_str_index["esp_r"] =	{ i,0,-1 };
		m_map_str_index["sp_r"] =	{ i,1,-1 };
		++i;

		m_map_str_index["esp_w"] =	{ i,0,-1 };
		m_map_str_index["sp_w"] =	{ i,1,-1 };
		++i;
#endif //esp						  
									  
#if 1								  
		m_map_str_index["ebp_r"] =	{ i,0,-1 };
		m_map_str_index["bp_r"] =	{ i,1,-1 };
		++i;
									  
		m_map_str_index["ebp_w"] =	{ i,0,-1 };
		m_map_str_index["bp_w"] =	{ i,1,-1 };
		++i;
#endif //ebp
#if 1
	m_map_str_index["eflag_r"] =	{ i,-1,-1 };
		++i;
	m_map_str_index["eflag_w"] =	{ i,-1,-1 };
		++i;
#endif //eflag
#if 1
		m_map_str_index["mem_r"] = { i,-1,-1 };
		++i;
		m_map_str_index["mem_w"] = { i,-1,-1 };
		++i;
#endif //eflag
		return i;
	}
	int Regs_Find::init_regs()
	{
		for (int i = 8; i > 0; --i)
		{
			regs_level3* p= new regs_level3();
			m_vec_regs.push_back(p);
		}
		//eax edx ecx ebx�Ķ�д

		for (int i = 8; i > 0; --i)
		{
			regs_level2* p = new regs_level2();
			m_vec_regs.push_back(p);
		}
		//esi edi esp edp�Ķ�д

		for (int i = 8; i > 0; --i)
		{
			regs_level1* p = new regs_level1();
			m_vec_regs.push_back(p);
		}
		//eflag mem�Ķ�д

		return 0;
	}
	m_index Regs_Find::m_find_index(string str_find)
	{
		auto iter = m_map_str_index.find(str_find);
		if (iter != m_map_str_index.end())
		{
			return iter->second;
		}
		else
		{
			MD_Throw_Runtime(m_find_indexʧ�ܣ�û���ҵ���Ӧֵ);
		}
	}
	void Regs_Find::e_clear()
	{
		for (int i = 0; i < m_vec_regs.size(); ++i)
		{
			if (i < 8)
			{
				regs_level3* p = (regs_level3*)m_vec_regs[i];
				p->e_delete();
			}
			else if (i < 16)
			{
				regs_level2* p = (regs_level2*)m_vec_regs[i];
				p->e_delete();
			}
			else
			{
				regs_level1* p = (regs_level1*)m_vec_regs[i];
				p->e_delete();
			}
		}
	
	}
	bool Regs_Find::e_delete(std::string str_index, int i_value)
	{
		m_index index;
		try {
			index = m_find_index(str_index);
		}
		catch (LXY::m_runtime_error& e)
		{
			e.e_ShowError();
		}
		if (index.i_index1 < 8)
		{
			regs_level3* p = (regs_level3*)m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				return p->e_delete(i_value);
			else if (index.i_index3 == -1)
			{
				return p->regs_levelx::e_delete(i_value);
			}
			else if (index.i_index3 == 0)
			{
				return p->regs_levelx::regs_levell::e_delete(i_value);
			}
			else if (index.i_index3 == 1)
			{
				return p->regs_levelx::regs_levelh::e_delete(i_value);
			}
		}
		else if (index.i_index1 < 16)
		{
			regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				return p->e_delete(i_value);
			else
			{
				return p->regs_level1::e_delete(i_value);
			}
		}
		else
		{
			regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
			return p->e_delete(i_value);
		}
		return false;
	}
	void Regs_Find::e_insert(std::string str_index,int i)
	{
		m_index index;
		try {
			index = m_find_index(str_index);
		}
		catch (LXY::m_runtime_error &e)
		{
			e.e_ShowError();
		}
		if (index.i_index1 < 8)
		{
			regs_level3* p = (regs_level3*) m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				p->e_insert(i);
			else if (index.i_index3 == -1)
			{
				p->regs_levelx::e_insert(i);
			}
			else if (index.i_index3 == 0)
			{
				p->regs_levelx::regs_levell::e_insert(i);
			}
			else if (index.i_index3 == 1)
			{
				p->regs_levelx::regs_levelh::e_insert(i);
			}
		}
		else if (index.i_index1 < 16)
		{
			regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				p->e_insert(i);
			else
			{
				p->regs_level1::e_insert(i);
			}
		}
		else
		{
			regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
			p->e_insert(i);
		}
	}
	void Regs_Find::e_printf(std::string str_index)
	{
		m_index index;
		for (int i = 0; i < sizeof(asz_reg_names) / sizeof(asz_reg_names[0]); ++i)
		{
			string str_index_temp = asz_reg_names[i];
			if (str_index_temp.find(str_index) != string::npos)
			{
#if 1
				string strtemp = str_index_temp + "_r";
				try {
					index = m_find_index(strtemp);
				}
				catch (LXY::m_runtime_error& e)
				{
					e.e_ShowError();
					return;
				}
				if (index.i_index1 < 8)
				{
					regs_level3* p = (regs_level3*)m_vec_regs[index.i_index1];
					if (index.i_index2 == 0)
						p->print_all(strtemp);
					else if (index.i_index3 == -1)
					{
						p->regs_levelx::print_all(strtemp);
					}
					else if (index.i_index3 == 0)
					{
						p->regs_levelx::regs_levell::print_all(strtemp);
					}
					else if (index.i_index3 == 1)
					{
						p->regs_levelx::regs_levelh::print_all(strtemp);
					}
				}
				else if (index.i_index1 < 16)
				{
					regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
					if (index.i_index2 == 0)
						p->print_all(strtemp);
					else
					{
						p->regs_level1::print_all(strtemp);
					}
				}
				else
				{
					regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
					p->print_all(strtemp);
				}
#endif
#if 1
				string strtemp1 = str_index_temp + "_w";
				try {
					index = m_find_index(strtemp1);
				}
				catch (LXY::m_runtime_error& e)
				{
					e.e_ShowError();
					return;
				}
				if (index.i_index1 < 8)
				{
					regs_level3* p = (regs_level3*)m_vec_regs[index.i_index1];
					if (index.i_index2 == 0)
						p->print_all(strtemp1);
					else if (index.i_index3 == -1)
					{
						p->regs_levelx::print_all(strtemp1);
					}
					else if (index.i_index3 == 0)
					{
						p->regs_levelx::regs_levell::print_all(strtemp1);
					}
					else if (index.i_index3 == 1)
					{
						p->regs_levelx::regs_levelh::print_all(strtemp1);
					}
				}
				else if (index.i_index1 < 16)
				{
					regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
					if (index.i_index2 == 0)
						p->print_all(strtemp1);
					else
					{
						p->regs_level1::print_all(strtemp1);
					}
				}
				else
				{
					regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
					p->print_all(strtemp1);
				}
#endif
			}
		}
	
	}
	static inline void set_to_vecsetset(string& str, std::vector<std::set<std::string>>& vecsetstr_flags, set<int> set)
	{
		for (auto value : set)
		{
			vecsetstr_flags[value].insert(str);
		}
	}
	//�ع�g_vecsetstr_flags
	void Regs_Find::e_printf_to_flags(std::vector<std::set<std::string>>& vecsetstr_flags)
	{
		for (auto &set : vecsetstr_flags)
			set.clear();
		m_index index;
		for (int i = 0; i < sizeof(asz_reg_names) / sizeof(asz_reg_names[0]); ++i)
		{
			string str_index_temp = asz_reg_names[i];
#if 1
			string strtemp = str_index_temp + "_r";
			try {
				index = m_find_index(strtemp);
			}
			catch (LXY::m_runtime_error& e)
			{
				e.e_ShowError();
				return;
			}
			if (index.i_index1 < 8)
			{
				regs_level3* p = (regs_level3*)m_vec_regs[index.i_index1];
				if (index.i_index2 == 0)
					set_to_vecsetset(strtemp, vecsetstr_flags, p->e_ret_self());
				else if (index.i_index3 == -1)
				{
					set_to_vecsetset(strtemp, vecsetstr_flags, p->regs_levelx::e_ret_self());
				}
				else if (index.i_index3 == 0)
				{
					set_to_vecsetset(strtemp, vecsetstr_flags, p->regs_levelx::regs_levell::e_ret_self());
				}
				else if (index.i_index3 == 1)
				{
					set_to_vecsetset(strtemp, vecsetstr_flags, p->regs_levelx::regs_levelh::e_ret_self());
				}
			}
			else if (index.i_index1 < 16)
			{
				regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
				if (index.i_index2 == 0)
					set_to_vecsetset(strtemp, vecsetstr_flags, p->e_ret_self());
				else
				{
					set_to_vecsetset(strtemp, vecsetstr_flags, p->regs_level1::e_ret_self());
				}
			}
			else
			{
				regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
				set_to_vecsetset(strtemp, vecsetstr_flags, p->e_ret_self());
			}
#endif
#if 1
			string strtemp1 = str_index_temp + "_w";
			try {
				index = m_find_index(strtemp1);
			}
			catch (LXY::m_runtime_error& e)
			{
				e.e_ShowError();
				return;
			}
			if (index.i_index1 < 8)
			{
				regs_level3* p = (regs_level3*)m_vec_regs[index.i_index1];
				if (index.i_index2 == 0)
					set_to_vecsetset(strtemp1, vecsetstr_flags, p->e_ret_self());
				else if (index.i_index3 == -1)
				{
					set_to_vecsetset(strtemp1, vecsetstr_flags, p->regs_levelx::e_ret_self());
				}
				else if (index.i_index3 == 0)
				{
					set_to_vecsetset(strtemp1, vecsetstr_flags, p->regs_levelx::regs_levell::e_ret_self());
				}
				else if (index.i_index3 == 1)
				{
					set_to_vecsetset(strtemp1, vecsetstr_flags, p->regs_levelx::regs_levelh::e_ret_self());
				}
			}
			else if (index.i_index1 < 16)
			{
				regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
				if (index.i_index2 == 0)
					set_to_vecsetset(strtemp1, vecsetstr_flags, p->e_ret_self());
				else
				{
					set_to_vecsetset(strtemp1, vecsetstr_flags, p->regs_level1::e_ret_self());
				}
			}
			else
			{
				regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
				set_to_vecsetset(strtemp1, vecsetstr_flags, p->e_ret_self());
			}
#endif
		}
	}
#if 1
	set<int> Regs_Find::e_find_left(std::string str_index, int i)
	{
		m_index index;
		set<int> set_ret;
		try {
			index = m_find_index(str_index);
		}
		catch (LXY::m_runtime_error& e)
		{
			e.e_ShowError();
		}
		if (index.i_index1 < 8)
		{
			regs_level3* p = (regs_level3*)m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				set_ret = p->e_find_left(i);
			else if (index.i_index3 == -1)
			{
				set_ret = p->regs_levelx::e_find_left(i);
			}
			else if (index.i_index3 == 0)
			{
				set_ret = p->regs_levelx::regs_levell::e_find_left(i);
			}
			else if (index.i_index3 == 1)
			{
				set_ret = p->regs_levelx::regs_levelh::e_find_left(i);
			}
		}
		else if (index.i_index1 < 16)
		{
			regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				set_ret = p->e_find_left(i);
			else
			{
				set_ret = p->regs_level1::e_find_left(i);
			}
		}
		else
		{
			regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
			set_ret = p->e_find_left(i);
		}
		return set_ret;
	}
#endif

#if 1
	set<int> Regs_Find::e_ret_self(std::string str_index)
	{
		m_index index;
		set<int> set_ret;
		try {
			index = m_find_index(str_index);
		}
		catch (LXY::m_runtime_error& e)
		{
			e.e_ShowError();
		}
		if (index.i_index1 < 8)
		{
			regs_level3* p = (regs_level3*)m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				set_ret = p->e_ret_self();
			else if (index.i_index3 == -1)
			{
				set_ret = p->regs_levelx::e_ret_self();
			}
			else if (index.i_index3 == 0)
			{
				set_ret = p->regs_levelx::regs_levell::e_ret_self();
			}
			else if (index.i_index3 == 1)
			{
				set_ret = p->regs_levelx::regs_levelh::e_ret_self();
			}
		}
		else if (index.i_index1 < 16)
		{
			regs_level2* p = (regs_level2*)m_vec_regs[index.i_index1];
			if (index.i_index2 == 0)
				set_ret = p->e_ret_self();
			else
			{
				set_ret = p->regs_level1::e_ret_self();
			}
		}
		else
		{
			regs_level1* p = (regs_level1*)m_vec_regs[index.i_index1];
			set_ret = p->e_ret_self();
		}
		return set_ret;
	}
#endif
}