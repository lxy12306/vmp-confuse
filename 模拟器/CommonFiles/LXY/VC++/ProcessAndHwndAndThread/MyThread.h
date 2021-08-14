#pragma once
#include <Windows.h>
#include <thread>
#include <vector>
typedef unsigned(__stdcall* PTHREAD_START) (void*);
//自定义线程宏
#define lxy_BEGINTHREADEX(psa, cbStackSize, pfnStartAddr, \
   pvParam, dwCreateFlags, pdwThreadId)                 \
      ((HANDLE)_beginthreadex(                          \
         (void *)        (psa),                         \
         (unsigned)      (cbStackSize),                 \
         (PTHREAD_START) (pfnStartAddr),                \
         (void *)        (pvParam),                     \
         (unsigned)      (dwCreateFlags),               \
         (unsigned *)    (pdwThreadId)))

typedef struct _LXY_PROCESS_THREADINFO
{
	LPVOID pvStartAddr;  // 线程的起始地址
	DWORD dwTid;   // 线程Id
	WCHAR wszModuleName[MAX_PATH];  // 所属的模块路径
}LXY_PROCESS_THREADINFO;

typedef enum _THREADINFOCLASS {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	MaxThreadInfoClass
} THREADINFOCLASS;

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;

typedef CLIENT_ID* PCLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION { // Information Class 0
	LONG     ExitStatus;
	PVOID    TebBaseAddress;
	CLIENT_ID ClientId;
	LONG AffinityMask;
	LONG Priority;
	LONG BasePriority;
} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

typedef LONG(__stdcall* ZwQueryInformationThread) (
	IN HANDLE ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	OUT PVOID ThreadInformation,
	IN ULONG ThreadInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

namespace LXY
{
	class MyThreadInfo
	{
		/*友元声明以及using定义*/
	private:
		/*私有数据成员*/

	public:
		/*公有数据成员*/

	public:
		/*构造函数与析构函数*/
		MyThreadInfo() {
		}
		~MyThreadInfo() {
		}
	public:
		/*导出函数*/
		//遍历进程中的所有线程 存入结构体中
		BOOL e_QueryThreadInfo_By_Pid(__in DWORD dwPid, __out std::vector<LXY_PROCESS_THREADINFO>& vlst);
	private:
		/*其他非导出功能函数*/

	};
}

