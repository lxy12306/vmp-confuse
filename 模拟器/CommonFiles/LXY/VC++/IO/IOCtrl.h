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
		LARGE_INTEGER liSizeDst = { 0 };//目标的长度
		LARGE_INTEGER liSizeSrc = { 0 };//源的长度
		PVOID pvBufferReadWrite;
		CIOCP& m_iocp;
	};

	//异步io操作
	class AsynIOCtrl
	{
	#define BUFFSIZE (64*1024) //I/O buffer的长度
	#define MAX_PENDING_IO_REQS   4           // IO请求的最大数量
	#define MAX_THREAD_IO_DILL 4//最大处理io的线程数量
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
		/// 创建完成端口异步io将缓冲区数据写文件
		/// </summary>
		/// <param name="pszFileSrc">需要写的文件名</param>
		/// <param name="pvBuffer">缓冲区</param>
		/// <param name="liSizeSrc">缓冲区大小</param>
		/// <returns>创建线程的索引</returns>
		size_t e_FileWrite(PCTSTR pszFileSrc, PVOID pvBuffer, LARGE_INTEGER liSizeSrc);
		/// <summary>
		/// 等待IO完成
		/// </summary>
		/// <param name="">第几个io</param>
		/// <returns>是否位有效IO</returns>
		BOOL e_WaitForIoComplite(size_t);
	private:
		ULONG_PTR m_GetFreeKey()
		{
			return sm_Key++;
		}
		static DWORD WINAPI ms_ThreadWriteFile(LPVOID lpvPram);
	public:

	private:
		CIOCP m_iocp; //完成端口
		//单次失效的成员变量
		HANDLE m_ahThread[MAX_THREAD_IO_DILL];
		size_t m_nhThreadNum = 0;
		static size_t sm_Key;
	private:
		IOCtrl* m_pIoCtrl = NULL;
		
	};
}

