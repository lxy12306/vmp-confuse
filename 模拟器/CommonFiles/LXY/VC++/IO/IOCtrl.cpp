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
			//ȷ��û�п��е�λ��
			if (nInsert == MAX_THREAD_IO_DILL)
			{
				nInsert = WaitForMultipleObjects(m_nhThreadNum, m_ahThread, false, INFINITE);
				CloseHandle(m_ahThread[nInsert]);
			}
			//���ȷʵû�пհ�λ�� �ȴ�ĳһ��������� �ٿ�ʼ��һ������
		}
		/*�����ж��߳��Ƿ��Ѿ�����*/
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
		return nRet; //���ز����λ��
	}
	DWORD WINAPI AsynIOCtrl::ms_ThreadWriteFile(LPVOID lpvPram)
	{
		DWORD dwRet = EXIT_SUCCESS;
		IOCtrl* ioc = (IOCtrl*)lpvPram;

		LARGE_INTEGER liNextReadOffset = { 0 };//��¼��һ��ƫ��
		LARGE_INTEGER liFileSizeDst = ioc->liSizeDst;//�ļ���С
		LARGE_INTEGER liBufferSizeSrc = ioc->liSizeSrc;//����Ļ�������С

		PVOID pvBufferRead = ioc->pvBufferReadWrite;//���뻺����
		HANDLE m_hFileToWrite = INVALID_HANDLE_VALUE;//д����ļ����

		CIOReq ior[MAX_PENDING_IO_REQS];//��ʼIO����
		ULONG_PTR ulpCK = ioc->ulpCK;//��ɶ˿�Key
		int iWritesInProgress = 0;//�첽��������

		m_hFileToWrite = CreateFile(ioc->pszFileDst, GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS,
			FILE_FLAG_OVERLAPPED, NULL);

		if (m_hFileToWrite == INVALID_HANDLE_VALUE)
		{
			chASSERT(m_hFileToWrite != INVALID_HANDLE_VALUE);
			dwRet = EXIT_FAILURE;
			goto leave;
		}
		/*�����첽д�ļ��ļ�*/

		SetFilePointerEx(m_hFileToWrite, liFileSizeDst, NULL, FILE_BEGIN);
		SetEndOfFile(m_hFileToWrite);
		/*�����ļ���С*/

		ioc->m_iocp.AssociateDevice(m_hFileToWrite, ulpCK);
		/*��IO��ɶ˿�����豸*/


		for (int nIOReq = 0; nIOReq < _countof(ior); nIOReq++) {

			//����ɶ˿����д֪ͨ
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
			if (dlpCompletionKey == ulpCK)//������
			{

				if (liNextReadOffset.QuadPart < liFileSizeDst.QuadPart) //û�н��� ��������첽����
				{

					//û�е��ļ����� ���ö�ȡ�Ļ�����
					pior->e_SetBuffer(pvBufferRead, SIZE_T(liBufferSizeSrc.QuadPart > BUFFSIZE ? BUFFSIZE : liBufferSizeSrc.QuadPart));
					//����дio
					pior->e_Write(m_hFileToWrite, &liNextReadOffset);
					//���軺��������
					pvBufferRead = PVOID((PBYTE)pvBufferRead + BUFFSIZE);
					//�����Ѿ�д�˶�����
					liNextReadOffset.QuadPart += BUFFSIZE;
					liBufferSizeSrc.QuadPart -= BUFFSIZE;
				}
				else
				{
					//׼���˳�ѭ���� �Ѿ������ļ�β ��������
					iWritesInProgress--;
				}
			}
			else //��ɶ˿���ɵ��������ļ�����
			{
				ioc->m_iocp.PostStatus(dlpCompletionKey, dwNumBytes, pior);//�����׸���ɶ˿�
			}
		}

		leave:
		{
			//������Դ
			CloseHandle(m_hFileToWrite);
			delete ioc;
		}
	

		return dwRet;
	}
}
