#include "vmp_to_confirm_correctness_of_deobfuscation.h"
#include "../CommonFiles/LXY/VC++/Exception/MyExcetpionCpp.h"
#include "../memory/LXY_malloc.h"
#include "../util.h"
using namespace std;
#define ERROR_FILE_NAME NULL
#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(#szError,__FILE__,__LINE__)    
extern vector<Seg_Map>  g_vecSegs;//内存状态
extern Regs g_Regs; //寄存器

//全局寄存器写入到模拟器中
void write_regs();
//全局寄存器写入到模拟器中
void read_regs();

namespace LXY
{
	bool operator==(Regs& lhs, Regs& rhs)
	{
		bool b_ret = true;
		m_PrintFile(ERROR_FILE_NAME, "寄存器比较：");
		if (lhs.regs.r_eax != rhs.regs.r_eax) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "eax不同 ");
		}
		if (lhs.regs.r_edx != rhs.regs.r_edx) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "edx不同 ");
		}
		if (lhs.regs.r_ecx != rhs.regs.r_ecx) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "ecx不同 ");
		}
		if (lhs.regs.r_ebx != rhs.regs.r_ebx) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "ebx不同 ");
		}
		if (lhs.regs.r_esi != rhs.regs.r_esi) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "esi不同 ");
		}
		if (lhs.regs.r_edi != rhs.regs.r_edi) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "edi不同 ");
		}
		if (lhs.regs.r_esp != rhs.regs.r_esp) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "esp不同 ");
		}
		if (lhs.regs.r_ebp != rhs.regs.r_ebp) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "ebp不同 ");
		}
		if (lhs.regs.r_eip != rhs.regs.r_eip) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "eip不同 ");
		}
		if (lhs.regs.r_efl != rhs.regs.r_efl) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "eflag不同\n");
		}
		if (b_ret) {
			m_PrintFile(ERROR_FILE_NAME, "寄存器比较相同\n");
		}

		return b_ret;
	}
	bool operator==(Seg_Map& lhs, Seg_Map& rhs)
	{
		bool b_ret = true;
		char a_temp[0x10] = { 0,0,0,0,0,0,0,0,0,0 };
		if (lhs.dwBase != rhs.dwBase)
		{
			m_PrintFile(ERROR_FILE_NAME, "内存基址不一致,比较没有意义\r\n");
			return false;
		}
		if (lhs.uSize != rhs.uSize)
		{
			m_PrintFile(ERROR_FILE_NAME, "内存大小不一致,无法进行比较\r\n");
			return false;
		}

		unsigned char* p_lhs = lhs.bBuffer, *p_rhs = rhs.bBuffer;
		auto p_temp = lxy_memcmp(p_lhs, p_rhs, lhs.uSize);
		auto p_temp2 = p_temp.get();

		//输出不相等的内存位置
		for (int i = 0; i < rhs.uSize; i++)
		{
			if (*p_temp2 == 0)
			{
				b_ret = false;
				m_PrintFile(ERROR_FILE_NAME, "内存:<%#10x>不一致...", lhs.dwBase+i);
			}
			++p_temp2;
		}
		
		//如果内存相等 则输出提示
		if (b_ret == true)
		{
			m_PrintFile(ERROR_FILE_NAME, "内存:<%#10x> size:<%#10x> 状态一致...", lhs.dwBase,lhs.uSize );
		}
		m_PrintFile(ERROR_FILE_NAME, "\n");
		
		return b_ret;
	}

	void SimulatorStatus::e_save_current() {

		//清空之前保存的
		for (auto seg : vec_seg_to_save)
		{
			lxy_free(seg.bBuffer);
		}
		vec_seg_to_save.clear();

		//更新模拟器状态到全局状态中
		read_regs();
		read_mem();

		for (auto seg : g_vecSegs)
		{
			Seg_Map new_seg(seg);

			if ((new_seg.bBuffer = (unsigned char*)lxy_malloc(new_seg.uSize)) == NULL)
			{
				MD_Throw_Runtime(初始化复制内存失败);
			}
			lxy_memcpy(new_seg.bBuffer, new_seg.uSize, seg.bBuffer, seg.uSize);
			//需要复制一份内存 其他的照旧
			vec_seg_to_save.emplace_back(new_seg);
		}
		lxy_memcpy(&regs_to_save, sizeof(Regs), &g_Regs, sizeof(Regs));
		str_status = "保存了模拟器状态（save_begin）";
	}
	void SimulatorStatus::e_swap_current_noret()
	{
		SimulatorStatus* p_old = new SimulatorStatus(*this); //保存之前的SimulatorStatus的状态

		//更新模拟器状态到全局状态中
		read_regs();
		read_mem();

		vec_seg_to_save = g_vecSegs;
		regs_to_save = g_Regs;
		//不是释放内存 此时内存被p_old保存

		g_vecSegs = p_old->vec_seg_to_save;
		g_Regs = p_old->regs_to_save;

		for (int i = 0; i < g_vecSegs.size(); ++i)
		{
			unsigned u_size = p_old->vec_seg_to_save[i].uSize;
			if ((g_vecSegs[i].bBuffer = (unsigned char*)lxy_malloc(u_size)) == NULL)
			{
				MD_Throw_Runtime(初始化复制内存失败);
			}
			lxy_memcpy(g_vecSegs[i].bBuffer, u_size, p_old->vec_seg_to_save[i].bBuffer, u_size);
			//需要复制一份内存 其他的照旧

			// 重映射执行内存
			g_Error = uc_mem_write(g_uc, g_vecSegs[i].dwBase, g_vecSegs[i].bBuffer, g_vecSegs[i].uSize);
		}
		write_regs();
		delete p_old;

		str_status = "保存了模拟器状态（swap）,模拟器状态已被改变";
	}

	SimulatorStatus* SimulatorStatus::e_swap_current_ret() {

		SimulatorStatus* p_old = new SimulatorStatus(*this); //保存之前的SimulatorStatus的状态

		//更新模拟器状态到全局状态中
		read_regs();
		read_mem();

		vec_seg_to_save = g_vecSegs;
		regs_to_save = g_Regs;
		//不是释放内存 此时内存被p_old保存

		g_vecSegs = p_old->vec_seg_to_save;
		g_Regs = p_old->regs_to_save;

		for (int i = 0; i < g_vecSegs.size(); ++i)
		{
			unsigned u_size = p_old->vec_seg_to_save[i].uSize;
			
			if ((g_vecSegs[i].bBuffer = (unsigned char*)lxy_malloc(u_size)) == NULL)
			{
				MD_Throw_Runtime(初始化复制内存失败);
			}
			lxy_memcpy(g_vecSegs[i].bBuffer, u_size, p_old->vec_seg_to_save[i].bBuffer, u_size);
			//需要复制一份内存 其他的照旧

			// 重映射执行内存
			g_Error = uc_mem_write(g_uc, g_vecSegs[i].dwBase, g_vecSegs[i].bBuffer, g_vecSegs[i].uSize);
		}
		write_regs();
		str_status = "保存了模拟器状态（save_begin）是被拷贝的";
		return p_old;
	}
	SimulatorStatus::SimulatorStatus(const SimulatorStatus& old):vec_seg_to_save(vector<Seg_Map>(old.vec_seg_to_save))
	{
		lxy_memcpy(&regs_to_save, sizeof(Regs), (void*)&old.regs_to_save, sizeof(Regs));
		str_status = "保存了模拟器状态（swap）,模拟器状态已被改变";
	}
	SimulatorStatus::~SimulatorStatus()
	{
		for (auto seg : vec_seg_to_save)
		{
			lxy_free(seg.bBuffer);
		}
		vec_seg_to_save.clear();
	}

	bool SimulatorStatus::e_cmp_status() {

		read_regs();
		read_mem();
		bool b_ret = regs_to_save == g_Regs;
		int c = g_vecSegs.size();
		for (int i = 0; i < g_vecSegs.size(); ++i)
		{
			b_ret &= g_vecSegs[i] == vec_seg_to_save[i];
		}

		return b_ret;
	}
}