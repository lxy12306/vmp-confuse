#include "IOCtrl.h"
#include "F:/Code/object/Exception/MyExceptionWindows.h"
namespace LXY
{
#define MD_Throw_Windows(szError) throw LXY::m_WindowsException(__FILE__,__LINE__,#szError, GetLastError())
#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(#szError,__FILE__,__LINE__)    
	size_t AsynIOCtrl::sm_Key = 0;
	BOOL AsynIOCtrl::e_WaitForIoComplite(size_t n)
	{
		if (n >= MAX_THREAD_IO_DILL)
			return false;
		if (m_ahThread[n] == NULL)
			return false;
		WaitForSingleObject(m_ahThread[n], INFINITE);
		CloseHandle(m_ahThread[n]);
		m_ahThread[n] = NULL;
		return true;
	}
	size_t AsynIOCtrl::e_FileWrite(PCTSTR pszFileDst, PVOID pvBuffer, LARGE_INTEGER liSizeSrc)
	{
		size_t nRet = -1;
		size_t nInsert = 0;
		if (m_nhThreadNum == MAX_THREAD_IO_DILL)
		{
			for (nInsert  = 0; nInsert < MAX_THREAD_IO_DILL; ++nInsert)
			{
				if (m_ahThread[nInsert] == NULL)
				{
					break;
				}
			}
			//确认没有空闲的位置
			if (nInsert == MAX_THREAD_IO_DILL)
			{
				nInsert = WaitForMultipleObjects(m_nhThreadNum, m_ahThread, false, INFINITE);
				CloseHandle(m_ahThread[nInsert]);
			}
			//如果确实没有空白位置 等待某一个任务结束 再开始下一个任务
		}
		/*以上判断线程是否已经满了*/
		else
		{
			nInsert = m_nhThreadNum;
			m_nhThreadNum++;
		}
		nRet = nInsert;
		HANDLE hThread = NULL;

		m_pIoCtrl = new IOCtrl(this->m_iocp);
		if(m_pIoCtrl!=NULL) 
		{

			m_pIoCtrl->liSizeDst.QuadPart = m_chROUNDUP(liSizeSrc.QuadPart, BUFFSIZE);
			m_pIoCtrl->liSizeSrc = liSizeSrc;
			m_pIoCtrl->pvBufferReadWrite = pvBuffer;
			m_pIoCtrl->pszFileDst = pszFileDst;
			m_pIoCtrl->ulpCK = m_GetFreeKey();
			hThread = CreateThread(NULL, 0, ms_ThreadWriteFile, m_pIoCtrl, 0, 0);
			if (hThread == NULL)
			{
				delete m_pIoCtrl;
				m_nhThreadNum--;
				MD_Throw_Windows(CreateThread(NULL, 0, ms_ThreadWriteFile, m_pIoCtrl, 0, 0));
			}
		}
		else
		{
			MD_Throw_Runtime(new IOCtrl(this->m_iocp));
		}
		m_ahThread[nInsert] = hThread;
		m_pIoCtrl = NULL;
		return nRet; //返回插入的位置
	}
	DWORD WINAPI AsynIOCtrl::ms_ThreadWriteFile(LPVOID lpvPram)
	{
		DWORD dwRet = EXIT_SUCCESS;
		IOCtrl* ioc = (IOCtrl*)lpvPram;

		LARGE_INTEGER liNextReadOffset = { 0 };//记录下一个偏移
		LARGE_INTEGER liFileSizeDst = ioc->liSizeDst;//文件大小
		LARGE_INTEGER liBufferSizeSrc = ioc->liSizeSrc;//输入的缓冲区大小

		PVOID pvBufferRead = ioc->pvBufferReadWrite;//输入缓冲区
		HANDLE m_hFileToWrite = INVALID_HANDLE_VALUE;//写入的文件句柄

		CIOReq ior[MAX_PENDING_IO_REQS];//初始IO请求
		ULONG_PTR ulpCK = ioc->ulpCK;//完成端口Key
		int iWritesInProgress = 0;//异步请求数量

		m_hFileToWrite = CreateFile(ioc->pszFileDst, GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS,
			FILE_FLAG_OVERLAPPED, NULL);

		if (m_hFileToWrite == INVALID_HANDLE_VALUE)
		{
			chASSERT(m_hFileToWrite != INVALID_HANDLE_VALUE);
			dwRet = EXIT_FAILURE;
			goto leave;
		}
		/*创建异步写文件文件*/

		SetFilePointerEx(m_hFileToWrite, liFileSizeDst, NULL, FILE_BEGIN);
		SetEndOfFile(m_hFileToWrite);
		/*设置文件大小*/

		ioc->m_iocp.AssociateDevice(m_hFileToWrite, ulpCK);
		/*向IO完成端口添加设备*/


		for (int nIOReq = 0; nIOReq < _countof(ior); nIOReq++) {

			//向完成端口添加写通知
			++iWritesInProgress;
			ioc->m_iocp.PostStatus(ulpCK, 0, &ior[nIOReq]);
		}
		while (iWritesInProgress > 0)
		{
			ULONG_PTR dlpCompletionKey;
			DWORD dwNumBytes;
			CIOReq* pior;

			ioc->m_iocp.GetStatus(&dlpCompletionKey, &dwNumBytes, (OVERLAPPED**)&pior, MAX_WAITETIME);
			if (GetLastError() == ERROR_TIMEOUT)
			{
				chASSERT(GetLastError() != ERROR_TIMEOUT);
				dwRet = EXIT_FAILURE;
				goto leave;
			}
			if (dlpCompletionKey == ulpCK)//本任务
			{

				if (liNextReadOffset.QuadPart < liFileSizeDst.QuadPart) //没有结束 继续添加异步请求
				{

					//没有到文件结束 设置读取的缓冲区
					pior->e_SetBuffer(pvBufferRead, SIZE_T(liBufferSizeSrc.QuadPart > BUFFSIZE ? BUFFSIZE : liBufferSizeSrc.QuadPart));
					//发送写io
					pior->e_Write(m_hFileToWrite, &liNextReadOffset);
					//重设缓冲区区域
					pvBufferRead = PVOID((PBYTE)pvBufferRead + BUFFSIZE);
					//重设已经写了多少了
					liNextReadOffset.QuadPart += BUFFSIZE;
					liBufferSizeSrc.QuadPart -= BUFFSIZE;
				}
				else
				{
					//准备退出循环了 已经到了文件尾 结束请求
					iWritesInProgress--;
				}
			}
			else //完成端口完成的其他的文件操作
			{
				ioc->m_iocp.PostStatus(dlpCompletionKey, dwNumBytes, pior);//重新抛给完成端口
			}
		}

		leave:
		{
			//清理资源
			CloseHandle(m_hFileToWrite);
			delete ioc;
		}
	

		return dwRet;
	}
}
