#pragma once
#include <Windows.h>
namespace LXY
{
	//ÿһ��I/0��������Ҫһ��OVERLAPPED�ṹ�廹��һ�����ݻ�����
	class CIOReq : public OVERLAPPED
	{
	public:
		CIOReq() :m_nBuffSize(0), m_pvData(NULL), m_bOwener(TRUE)
		{
			Internal = InternalHigh = 0;//Internal����I/O�Ĵ����� InternalHigh ����I/O���ֽ���
			Offset = OffsetHigh = 0;//I/Oƫ��
			hEvent = NULL;//�ɵȴ��¼�����
		}
		~CIOReq()
		{
			if (m_bOwener)//��ӵ���� ���ͷſռ�
				if (m_pvData != NULL)
					VirtualFree(m_pvData, 0, MEM_RELEASE);//�ͷ�����Ŀռ�
		}
		/// <summary>
		/// ����buffer
		/// </summary>
		/// <param name="pvData">�����Ļ�������ַ</param>
		/// <param name="nBufferSize">�����Ļ���������</param>
		VOID e_SetBuffer(PVOID pvData, SIZE_T nBufferSize)
		{
			if (m_pvData != (PVOID)0xcccccccc&&m_bOwener)//��ӵ���� ���ͷſռ�
				if (m_pvData != NULL)
					VirtualFree(m_pvData, 0, MEM_RELEASE);//�ͷ��Ѿ�����Ŀռ�
			m_nBuffSize = nBufferSize;
			m_pvData = pvData;
			m_bOwener = false;
		}
		/// <summary>
		/// �����ڴ�ĳ���
		/// </summary>
		/// <param name="nBufferSize">��Ҫ���ļ�����</param>
		/// <param name="pdwError">���صĴ�����</param>
		/// <returns>�����Ƿ�ɹ�</returns>
		BOOL e_AllocBuffer(SIZE_T nBufferSize,PDWORD pdwError = NULL)
		{
			m_nBuffSize = nBufferSize;
			m_pvData = VirtualAlloc(NULL, m_nBuffSize, MEM_COMMIT, PAGE_READWRITE);
			if (pdwError != NULL)
				*pdwError = GetLastError();
			m_bOwener = true;
			return (m_pvData != NULL);
		}
		/// <summary>
		/// �첽��ȡ
		/// </summary>
		/// <param name="hDevice">�첽��ȡ���豸���</param>
		/// <param name="pliOffset">��ȡƫ��</param>
		/// <returns>�Ƿ�ɹ�����ȡ��������첽IO����</returns>
		BOOL e_Read(HANDLE hDevice, PLARGE_INTEGER pliOffset = NULL)
		{
			if (pliOffset != NULL)
			{
				Offset = pliOffset->LowPart;
				OffsetHigh = pliOffset->HighPart;
				
			}
			if (!::ReadFile(hDevice, m_pvData, m_nBuffSize, NULL, this))
			{
				if (GetLastError() == ERROR_IO_PENDING)
					return true;
				else
					return false;
			}
			else
				return false;
		}
		/// <summary>
		/// �첽д��
		/// </summary>
		/// <param name="hDevice">�첽д����豸���</param>
		/// <param name="pliOffset">д��ƫ��</param>
		/// <returns>д���Ƿ�ɹ�</returns>
		BOOL e_Write(HANDLE hDevice, PLARGE_INTEGER pliOffset = NULL)
		{
			if (pliOffset != NULL)
			{
				Offset = pliOffset->LowPart;
				OffsetHigh = pliOffset->HighPart;
				//Internal = InternalHigh = 0;//Internal����I/O�Ĵ����� InternalHigh ����I/O���ֽ���
				//hEvent = 0;
			}
			if (!::WriteFile(hDevice, m_pvData, m_nBuffSize, NULL, this))
			{
				if (GetLastError() == ERROR_IO_PENDING)
					return true;
				else
					return false;
			}
			else
				return false;
		}

		;
	public:
		SIZE_T m_nBuffSize;//�ļ�����
		PVOID m_pvData;//������
		BOOL m_bOwener;//�Ƿ��ǻ�������������
	};
}


