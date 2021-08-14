#pragma once

#include <vector>
#include <string>
#include "../memory/LXY_MemoryOperation.h"
#include "../Framework.h"

namespace LXY {
	struct SimulatorStatus
	{
		std::string str_status;
		std::vector<Seg_Map> vec_seg_to_save;
		Regs regs_to_save;
		~SimulatorStatus();
		SimulatorStatus() :vec_seg_to_save(std::vector<Seg_Map>()), str_status("��״̬"){

		}
		SimulatorStatus(const SimulatorStatus& old);
		void e_save_current();
		void e_swap_current_noret();
		//�Ƚϵ�ǰ״̬��ȫ��״̬�Ƿ���ͬ
		bool e_cmp_status();
		SimulatorStatus* e_swap_current_ret();

	};
}
