#pragma once
#include "../Framework.h"
#include "../util.h"
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <memory>

extern std::vector<cs_insn*> g_vec_cs_insn;
extern const char* asz_reg_names[];


namespace LXY
{

	struct m_index
	{
		int8_t i_index1 = -1;
		int8_t i_index2 = -1;
		int8_t i_index3 = -1;

		m_index() = default;
	};
	static bool operator==(m_index& lhs, m_index& rhs);
	class Regs_Find
	{
		/*��Ԫ�����Լ�using����*/
		struct  regs_level1
		{
			std::set<int> m_set;
			bool virtual e_delete(int i) {
				if (m_set.erase(i) == 0)
					return false;
				return true;
			}
			void virtual e_insert(int i) {
				m_set.emplace(i);
			}
			std::set<int> virtual e_find_left(int i) {
				return std::set<int>(m_set.begin(), m_set.lower_bound(i));
			}
			void virtual print_all(std::string str_index)
			{
				if (m_set.empty()) return;
				printf("\r\n%s:\r\n", str_index.c_str());
				for (auto value : m_set)
				{
					std::string strTemp = std::to_string(value);
					strTemp += ": ";
					strTemp += g_vec_cs_insn[value]->mnemonic;
					strTemp += " ";
					strTemp += g_vec_cs_insn[value]->op_str;
					printf("%-40s", strTemp.c_str());
				}
				printf("\r\n");
			}
			std::set<int> virtual e_ret_self() {
				return std::set<int>(m_set.begin(), m_set.end());
			}
			void virtual e_delete()
			{
				m_set.clear();
			}
		};
		struct regs_levell:regs_level1
		{
		};
		struct regs_levelh:regs_level1
		{
		};
		struct regs_level2 :regs_level1
		{
			std::set<int> m_set_x;
			void e_insert(int i) override {
				m_set_x.emplace(i);
			}
			bool e_delete(int i) override {
				if (m_set_x.erase(i) == 1)
				{
					return true;
				}
				return regs_level1::e_delete(i);
			}
			std::set<int> virtual e_find_left(int i) {
				std::set<int> set_ret(regs_level1::e_find_left(i));
				set_ret.insert(m_set_x.begin(), m_set_x.lower_bound(i));
				return set_ret;
			}
			void print_all (std::string str_index) override
			{
				std::set<int> set_temp(m_set_x);
				set_temp.insert(m_set.begin(), m_set.end());
				if (set_temp.empty()) return;
				printf("\r\n%s:\r\n", str_index.c_str());
				for (auto value : set_temp)
				{
					std::string strTemp = std::to_string(value);
					strTemp += ": ";
					strTemp += g_vec_cs_insn[value]->mnemonic;
					strTemp += " ";
					strTemp += g_vec_cs_insn[value]->op_str;
					printf("%-40s", strTemp.c_str());
				}
				printf("\r\n");
			}
			std::set<int> virtual e_ret_self() {
				return std::set<int>(m_set_x.begin(), m_set_x.end());
			}
			void virtual e_delete()
			{
				regs_level1::e_delete();
				m_set_x.clear();
			}

		};
		struct regs_levelx :regs_levell, regs_levelh
		{
			std::set<int> m_set_x;
			void e_insert(int i) override {
				m_set_x.emplace(i);
			}
			bool e_delete(int i) override {
				if (m_set_x.erase(i) == 1)
				{
					return true;
				}
				return regs_levell::e_delete(i)|| regs_levelh::e_delete(i);
			}
			std::set<int> virtual e_find_left(int i) {
				std::set<int> set_ret(regs_levell::e_find_left(i));
				std::set<int> set_temp(regs_levelh::e_find_left(i));
				set_ret.insert(m_set_x.begin(), m_set_x.lower_bound(i));
				set_ret.insert(set_temp.begin(), set_temp.end());
				return set_ret;
			}
			void print_all(std::string str_index) override
			{
				std::set<int> set_temp(m_set_x);
				set_temp.insert(regs_levell::m_set.begin(), regs_levell::m_set.end());
				set_temp.insert(regs_levelh::m_set.begin(), regs_levelh::m_set.end());
				if (set_temp.empty()) return;
				printf("\r\n%s:\r\n", str_index.c_str());
				for (auto value : set_temp)
				{
					std::string strTemp = std::to_string(value);
					strTemp += ": ";
					strTemp += g_vec_cs_insn[value]->mnemonic;
					strTemp += " ";
					strTemp += g_vec_cs_insn[value]->op_str;
					printf("%-40s", strTemp.c_str());
				}
				printf("\r\n");
			}
			std::set<int> virtual e_ret_self() {
				return std::set<int>(m_set_x.begin(), m_set_x.end());
			}
			void virtual e_delete()
			{
				regs_levell::e_delete();
				regs_levelh::e_delete();
				m_set_x.clear();
			}
		};
		struct regs_level3:regs_levelx
		{
			std::set<int> m_set_ex;
			void e_insert(int i) override {
				m_set_ex.emplace(i);
			}
			bool e_delete(int i) override {
				if (m_set_ex.erase(i) == 1)
				{
					return true;
				}
				return regs_levelx::e_delete(i);
			}
			std::set<int> virtual e_find_left(int i) {
				std::set<int> set_ret(regs_levelx::e_find_left(i));
				set_ret.insert(m_set_ex.begin(), m_set_ex.lower_bound(i));
				return set_ret;
			}

			void print_all(std::string str_index) override
			{
				std::set<int> set_temp(m_set_ex);
				set_temp.insert(m_set_x.begin(), m_set_x.end());
				set_temp.insert(regs_levell::m_set.begin(), regs_levell::m_set.end());
				set_temp.insert(regs_levelh::m_set.begin(), regs_levelh::m_set.end());
				if (set_temp.empty()) return;
				printf("\r\n%s:\r\n", str_index.c_str());
				for (auto value : set_temp)
				{
					std::string strTemp = std::to_string(value);
					strTemp += ": ";
					strTemp += g_vec_cs_insn[value]->mnemonic;
					strTemp += " ";
					strTemp += g_vec_cs_insn[value]->op_str;
					printf("%-40s", strTemp.c_str());
				}
				printf("\r\n");
			}
			std::set<int> virtual e_ret_self() {
				return std::set<int>(m_set_ex.begin(), m_set_ex.end());
			}
			void virtual e_delete()
			{
				regs_levelx::e_delete();
				m_set_ex.clear();
			}
		};
	private:
		/*˽�����ݳ�Ա*/
		std::map<std::string, m_index> m_map_str_index;
		std::vector<void *> m_vec_regs;
	public:
		/*�������ݳ�Ա*/

	public:
		/*���캯������������*/
		Regs_Find() {
			init_index();
			init_regs();
		}
		~Regs_Find() {
		}
	public:
		/*��������*/
		void e_insert(std::string str_index, int i);
		std::set<int> e_find_left(std::string str_index, int i);
		std::set<int> e_ret_self(std::string str_index);
		bool e_delete(std::string str_index, int i_value);
		void e_printf(std::string str_index);
		void e_printf_to_flags(std::vector<std::set<std::string>>& vecsetstr_flags);
		void e_clear();
	private:
		/*�����ǵ������ܺ���*/
		//��ʼ�� m_map_str_index �ַ������ʵ�����
		int8_t init_index();
		//��ʼ�������Ĵ���ָ���
		int init_regs();
		//ͨ���Ĵ�����дstring��־ �ҵ�index��־
		m_index m_find_index(std::string str_find);

	};
}

namespace LXY
{
	/// <summary>
	/// �ж�ָ���Ƿ�Ϊ����ָ�� ret call jmp(�Ĵ���) jcc
	/// </summary>
	/// <param name="insn"></param>
	/// <returns>true ��ʾ�ǿ���ָ��</returns>
	inline bool is_special_instructions_begin(cs_insn* insn)
	{
		return ( (!strcmp(insn->mnemonic, "ret"))/*��retָ��*/ 
			|| (!strcmp(insn->mnemonic, "call")) /*callָ��*/
			||(!strcmp(insn->mnemonic, "jmp") && insn->detail->x86.operands[0].type != X86_OP_IMM)/*��jmp�Ĵ���ָ��*/
			/* || ((insn->mnemonic[0] == 'j') && (insn->detail->regs_read_count == 1) && (insn->detail->regs_read[0] == X86_REG_EFLAGS))*//*jcc����������ʽ��дeflag�Ĵ���*/
			)
			;
	}
	inline bool is_special_instructions_join(cs_insn* insn)
	{

		return (!strcmp(insn->mnemonic, "jmp") && insn->detail->x86.operands[0].type == X86_OP_IMM)/*��jmpֱ����תָ��*/ 
			 || ((insn->mnemonic[0] == 'j') && (insn->detail->regs_read_count == 1) && (insn->detail->regs_read[0] == X86_REG_EFLAGS))/*jcc����������ʽ��дeflag�Ĵ���*/;
	}

	int begin_analysis();

	inline void e_clear()
	{
		for (auto insn: g_vec_cs_insn)
		{
			instruction_free(insn);
		}
		g_vec_cs_insn.clear();

	}
}


