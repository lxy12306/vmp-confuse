#pragma once
#include <Windows.h>
namespace LXY
{
	//每一个I/0的请求都需要一个OVERLAPPED结构体还有一个数据缓冲区
	class CIOReq : public OVERLAPPED
	{
	public:
		CIOReq() :m_nBuffSize(0), m_pvData(NULL), m_bOwener(TRUE)
		{
			Internal = InternalHigh = 0;//Internal代表I/O的错误码 InternalHigh 代表I/O的字节数
			Offset = OffsetHigh = 0;//I/O偏移
			hEvent = NULL;//可等待事件对象
		}
		~CIOReq()
		{
			if (m_bOwener)//是拥有者 才释放空间
				if (m_pvData != NULL)
					VirtualFree(m_pvData, 0, MEM_RELEASE);//释放申请的空间
		}
		/// <summary>
		/// 设置buffer
		/// </summary>
		/// <param name="pvData">外来的缓冲区地址</param>
		/// <param name="nBufferSize">外来的缓冲区长度</param>
		VOID e_SetBuffer(PVOID pvData, SIZE_T nBufferSize)
		{
			if (m_pvData != (PVOID)0xcccccccc&&m_bOwener)//是拥有者 才释放空间
				if (m_pvData != NULL)
					VirtualFree(m_pvData, 0, MEM_RELEASE);//释放已经申请的空间
			m_nBuffSize = nBufferSize;
			m_pvData = pvData;
			m_bOwener = false;
		}
		/// <summary>
		/// 申请内存的长度
		/// </summary>
		/// <param name="nBufferSize">需要的文件长度</param>
		/// <param name="pdwError">返回的错误码</param>
		/// <returns>返回是否成功</returns>
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
		/// 异步读取
		/// </summary>
		/// <param name="hDevice">异步读取的设备句柄</param>
		/// <param name="pliOffset">读取偏移</param>
		/// <returns>是否成功将读取请求加入异步IO队列</returns>
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
		/// 异步写入
		/// </summary>
		/// <param name="hDevice">异步写入的设备句柄</param>
		/// <param name="pliOffset">写入偏移</param>
		/// <returns>写入是否成功</returns>
		BOOL e_Write(HANDLE hDevice, PLARGE_INTEGER pliOffset = NULL)
		{
			if (pliOffset != NULL)
			{
				Offset = pliOffset->LowPart;
				OffsetHigh = pliOffset->HighPart;
				//Internal = InternalHigh = 0;//Internal代表I/O的错误码 InternalHigh 代表I/O的字节数
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
		SIZE_T m_nBuffSize;//文件长度
		PVOID m_pvData;//缓冲区
		BOOL m_bOwener;//是否是缓冲区的所有者
	};
}


