#pragma once
#include <Windows.h>
#include <thread>
#include <vector>
typedef unsigned(__stdcall* PTHREAD_START) (void*);
//�Զ����̺߳�
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
	LPVOID pvStartAddr;  // �̵߳���ʼ��ַ
	DWORD dwTid;   // �߳�Id
	WCHAR wszModuleName[MAX_PATH];  // ������ģ��·��
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
		/*��Ԫ�����Լ�using����*/
	private:
		/*˽�����ݳ�Ա*/

	public:
		/*�������ݳ�Ա*/

	public:
		/*���캯������������*/
		MyThreadInfo() {
		}
		~MyThreadInfo() {
		}
	public:
		/*��������*/
		//���������е������߳� ����ṹ����
		BOOL e_QueryThreadInfo_By_Pid(__in DWORD dwPid, __out std::vector<LXY_PROCESS_THREADINFO>& vlst);
	private:
		/*�����ǵ������ܺ���*/

	};
}

