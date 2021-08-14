#include "vmp_to_confirm_correctness_of_deobfuscation.h"
#include "../CommonFiles/LXY/VC++/Exception/MyExcetpionCpp.h"
#include "../memory/LXY_malloc.h"
#include "../util.h"
using namespace std;
#define ERROR_FILE_NAME NULL
#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(#szError,__FILE__,__LINE__)    
extern vector<Seg_Map>  g_vecSegs;//�ڴ�״̬
extern Regs g_Regs; //�Ĵ���

//ȫ�ּĴ���д�뵽ģ������
void write_regs();
//ȫ�ּĴ���д�뵽ģ������
void read_regs();

namespace LXY
{
	bool operator==(Regs& lhs, Regs& rhs)
	{
		bool b_ret = true;
		m_PrintFile(ERROR_FILE_NAME, "�Ĵ����Ƚϣ�");
		if (lhs.regs.r_eax != rhs.regs.r_eax) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "eax��ͬ ");
		}
		if (lhs.regs.r_edx != rhs.regs.r_edx) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "edx��ͬ ");
		}
		if (lhs.regs.r_ecx != rhs.regs.r_ecx) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "ecx��ͬ ");
		}
		if (lhs.regs.r_ebx != rhs.regs.r_ebx) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "ebx��ͬ ");
		}
		if (lhs.regs.r_esi != rhs.regs.r_esi) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "esi��ͬ ");
		}
		if (lhs.regs.r_edi != rhs.regs.r_edi) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "edi��ͬ ");
		}
		if (lhs.regs.r_esp != rhs.regs.r_esp) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "esp��ͬ ");
		}
		if (lhs.regs.r_ebp != rhs.regs.r_ebp) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "ebp��ͬ ");
		}
		if (lhs.regs.r_eip != rhs.regs.r_eip) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "eip��ͬ ");
		}
		if (lhs.regs.r_efl != rhs.regs.r_efl) {
			b_ret = false;
			m_PrintFile(ERROR_FILE_NAME, "eflag��ͬ\n");
		}
		if (b_ret) {
			m_PrintFile(ERROR_FILE_NAME, "�Ĵ����Ƚ���ͬ\n");
		}

		return b_ret;
	}
	bool operator==(Seg_Map& lhs, Seg_Map& rhs)
	{
		bool b_ret = true;
		char a_temp[0x10] = { 0,0,0,0,0,0,0,0,0,0 };
		if (lhs.dwBase != rhs.dwBase)
		{
			m_PrintFile(ERROR_FILE_NAME, "�ڴ��ַ��һ��,�Ƚ�û������\r\n");
			return false;
		}
		if (lhs.uSize != rhs.uSize)
		{
			m_PrintFile(ERROR_FILE_NAME, "�ڴ��С��һ��,�޷����бȽ�\r\n");
			return false;
		}

		unsigned char* p_lhs = lhs.bBuffer, *p_rhs = rhs.bBuffer;
		auto p_temp = lxy_memcmp(p_lhs, p_rhs, lhs.uSize);
		auto p_temp2 = p_temp.get();

		//�������ȵ��ڴ�λ��
		for (int i = 0; i < rhs.uSize; i++)
		{
			if (*p_temp2 == 0)
			{
				b_ret = false;
				m_PrintFile(ERROR_FILE_NAME, "�ڴ�:<%#10x>��һ��...", lhs.dwBase+i);
			}
			++p_temp2;
		}
		
		//����ڴ���� �������ʾ
		if (b_ret == true)
		{
			m_PrintFile(ERROR_FILE_NAME, "�ڴ�:<%#10x> size:<%#10x> ״̬һ��...", lhs.dwBase,lhs.uSize );
		}
		m_PrintFile(ERROR_FILE_NAME, "\n");
		
		return b_ret;
	}

	void SimulatorStatus::e_save_current() {

		//���֮ǰ�����
		for (auto seg : vec_seg_to_save)
		{
			lxy_free(seg.bBuffer);
		}
		vec_seg_to_save.clear();

		//����ģ����״̬��ȫ��״̬��
		read_regs();
		read_mem();

		for (auto seg : g_vecSegs)
		{
			Seg_Map new_seg(seg);

			if ((new_seg.bBuffer = (unsigned char*)lxy_malloc(new_seg.uSize)) == NULL)
			{
				MD_Throw_Runtime(��ʼ�������ڴ�ʧ��);
			}
			lxy_memcpy(new_seg.bBuffer, new_seg.uSize, seg.bBuffer, seg.uSize);
			//��Ҫ����һ���ڴ� �������վ�
			vec_seg_to_save.emplace_back(new_seg);
		}
		lxy_memcpy(&regs_to_save, sizeof(Regs), &g_Regs, sizeof(Regs));
		str_status = "������ģ����״̬��save_begin��";
	}
	void SimulatorStatus::e_swap_current_noret()
	{
		SimulatorStatus* p_old = new SimulatorStatus(*this); //����֮ǰ��SimulatorStatus��״̬

		//����ģ����״̬��ȫ��״̬��
		read_regs();
		read_mem();

		vec_seg_to_save = g_vecSegs;
		regs_to_save = g_Regs;
		//�����ͷ��ڴ� ��ʱ�ڴ汻p_old����

		g_vecSegs = p_old->vec_seg_to_save;
		g_Regs = p_old->regs_to_save;

		for (int i = 0; i < g_vecSegs.size(); ++i)
		{
			unsigned u_size = p_old->vec_seg_to_save[i].uSize;
			if ((g_vecSegs[i].bBuffer = (unsigned char*)lxy_malloc(u_size)) == NULL)
			{
				MD_Throw_Runtime(��ʼ�������ڴ�ʧ��);
			}
			lxy_memcpy(g_vecSegs[i].bBuffer, u_size, p_old->vec_seg_to_save[i].bBuffer, u_size);
			//��Ҫ����һ���ڴ� �������վ�

			// ��ӳ��ִ���ڴ�
			g_Error = uc_mem_write(g_uc, g_vecSegs[i].dwBase, g_vecSegs[i].bBuffer, g_vecSegs[i].uSize);
		}
		write_regs();
		delete p_old;

		str_status = "������ģ����״̬��swap��,ģ����״̬�ѱ��ı�";
	}

	SimulatorStatus* SimulatorStatus::e_swap_current_ret() {

		SimulatorStatus* p_old = new SimulatorStatus(*this); //����֮ǰ��SimulatorStatus��״̬

		//����ģ����״̬��ȫ��״̬��
		read_regs();
		read_mem();

		vec_seg_to_save = g_vecSegs;
		regs_to_save = g_Regs;
		//�����ͷ��ڴ� ��ʱ�ڴ汻p_old����

		g_vecSegs = p_old->vec_seg_to_save;
		g_Regs = p_old->regs_to_save;

		for (int i = 0; i < g_vecSegs.size(); ++i)
		{
			unsigned u_size = p_old->vec_seg_to_save[i].uSize;
			
			if ((g_vecSegs[i].bBuffer = (unsigned char*)lxy_malloc(u_size)) == NULL)
			{
				MD_Throw_Runtime(��ʼ�������ڴ�ʧ��);
			}
			lxy_memcpy(g_vecSegs[i].bBuffer, u_size, p_old->vec_seg_to_save[i].bBuffer, u_size);
			//��Ҫ����һ���ڴ� �������վ�

			// ��ӳ��ִ���ڴ�
			g_Error = uc_mem_write(g_uc, g_vecSegs[i].dwBase, g_vecSegs[i].bBuffer, g_vecSegs[i].uSize);
		}
		write_regs();
		str_status = "������ģ����״̬��save_begin���Ǳ�������";
		return p_old;
	}
	SimulatorStatus::SimulatorStatus(const SimulatorStatus& old):vec_seg_to_save(vector<Seg_Map>(old.vec_seg_to_save))
	{
		lxy_memcpy(&regs_to_save, sizeof(Regs), (void*)&old.regs_to_save, sizeof(Regs));
		str_status = "������ģ����״̬��swap��,ģ����״̬�ѱ��ı�";
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