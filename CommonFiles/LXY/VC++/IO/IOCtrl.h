#pragma once
#include "CIOReq.h"
#include "IoCompletionPort.h"
#include <vector>
namespace LXY
{
	struct IOCtrl
	{
		IOCtrl(CIOCP &iocp):m_iocp(iocp){}
		~IOCtrl()
		{
			if (hFileRead != INVALID_HANDLE_VALUE)
				CloseHandle(hFileRead);
		}

		PCTSTR pszFileDst = NULL;
		PCTSTR pszFileSrc = NULL;
		HANDLE hFileRead = INVALID_HANDLE_VALUE;
		ULONG_PTR ulpCK = 0;
		LARGE_INTEGER liSizeDst = { 0 };//Ŀ��ĳ���
		LARGE_INTEGER liSizeSrc = { 0 };//Դ�ĳ���
		PVOID pvBufferReadWrite;
		CIOCP& m_iocp;
	};

	//�첽io����
	class AsynIOCtrl
	{
	#define BUFFSIZE (64*1024) //I/O buffer�ĳ���
	#define MAX_PENDING_IO_REQS   4           // IO������������
	#define MAX_THREAD_IO_DILL 4//�����io���߳�����
	#define MAX_WAITETIME	1000*100
	public:
		AsynIOCtrl():m_iocp(CIOCP(0)){}
		~AsynIOCtrl()
		{
#if 1
			WaitForMultipleObjects(m_nhThreadNum, m_ahThread, true, INFINITE);
			for (size_t i = 0; i < m_nhThreadNum; ++i)
			{
				CloseHandle(m_ahThread[i]);
			}
#endif

		}
	public:

		/// <summary>
		/// ������ɶ˿��첽io������������д�ļ�
		/// </summary>
		/// <param name="pszFileSrc">��Ҫд���ļ���</param>
		/// <param name="pvBuffer">������</param>
		/// <param name="liSizeSrc">��������С</param>
		/// <returns>�����̵߳�����</returns>
		size_t e_FileWrite(PCTSTR pszFileSrc, PVOID pvBuffer, LARGE_INTEGER liSizeSrc);
		/// <summary>
		/// �ȴ�IO���
		/// </summary>
		/// <param name="">�ڼ���io</param>
		/// <returns>�Ƿ�λ��ЧIO</returns>
		BOOL e_WaitForIoComplite(size_t);
	private:
		ULONG_PTR m_GetFreeKey()
		{
			return sm_Key++;
		}
		static DWORD WINAPI ms_ThreadWriteFile(LPVOID lpvPram);
	public:

	private:
		CIOCP m_iocp; //��ɶ˿�
		//����ʧЧ�ĳ�Ա����
		HANDLE m_ahThread[MAX_THREAD_IO_DILL];
		size_t m_nhThreadNum = 0;
		static size_t sm_Key;
	private:
		IOCtrl* m_pIoCtrl = NULL;
		
	};
}

