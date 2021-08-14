#pragma once
#include "F:/Code/object/CommonFiles/CmdHdr.h"
namespace LXY
{
    /// <summary>
    /// ��IO��ɶ˿ڵķ�װ
    /// </summary>
    class CIOCP {
    public:
        /// <summary>
        /// ���캯��
        /// </summary>
        /// <param name="nMaxConcurrency">�����߳�����</param>
        /// <returns>��</returns>
        CIOCP(int nMaxConcurrency = -1) {
            m_hIOCP = NULL;
            if (nMaxConcurrency != -1)
                (void)Create(nMaxConcurrency);
        }

        ~CIOCP() {
            if (m_hIOCP != NULL)
                chASSERT(CloseHandle(m_hIOCP));
        }

        /// <summary>
        /// �ر�IO��ɶ˿�
        /// </summary>
        /// <returns>�Ƿ�ɹ�</returns>
        BOOL Close() {
            BOOL bResult = CloseHandle(m_hIOCP);
            m_hIOCP = NULL;
            return(bResult);
        }
        /// <summary>
        /// ����һ���µ�IO��ɶ˿�
        /// </summary>
        /// <param name="nMaxConcurrency">���������߳��������</param>
        /// <returns></returns>
        BOOL Create(int nMaxConcurrency = 0) {
            m_hIOCP = CreateIoCompletionPort(
                INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);
            chASSERT(m_hIOCP != NULL);
            return(m_hIOCP != NULL);
        }
        /// <summary>
        /// �����˿ں��豸���
        /// </summary>
        /// <param name="hDevice">��Ҫ�������豸���</param>
        /// <param name="CompKey">������key</param>
        /// <returns>�Ƿ�ɹ�</returns>
        BOOL AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey) {
            BOOL fOk = (CreateIoCompletionPort(hDevice, m_hIOCP, CompKey, 0)
                == m_hIOCP);
            chASSERT(fOk);
            return(fOk);
        }
        /// <summary>
        /// �����׽��ֺ����ǵ���ɶ˿�
        /// </summary>
        /// <param name="hSocket">�׽��־��</param>
        /// <param name="CompKey">��ɶ˿�</param>
        /// <returns>�Ƿ�ɹ�</returns>
        BOOL AssociateSocket(SOCKET hSocket, ULONG_PTR CompKey) {
            return(AssociateDevice((HANDLE)hSocket, CompKey));
        }

        /// <summary>
        /// ��IO��ɶ˿ڷ���֪ͨ
        /// </summary>
        /// <param name="CompKey"></param>
        /// <param name="dwNumBytes"></param>
        /// <param name="po"></param>
        /// <returns></returns>
        BOOL PostStatus(ULONG_PTR CompKey, DWORD dwNumBytes = 0,
            OVERLAPPED* po = NULL) {
            BOOL fOk = PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, CompKey, po);
            chASSERT(fOk);
            return(fOk);
        }
        /// <summary>
        /// ��ȡIO��ɶ˿ڵ�֪ͨ
        /// </summary>
        /// <param name="pCompKey"></param>
        /// <param name="pdwNumBytes"></param>
        /// <param name="ppo"></param>
        /// <param name="dwMilliseconds"></param>
        /// <returns></returns>
        BOOL GetStatus(ULONG_PTR* pCompKey, PDWORD pdwNumBytes,
            OVERLAPPED** ppo, DWORD dwMilliseconds = INFINITE) {
            return(GetQueuedCompletionStatus(m_hIOCP, pdwNumBytes,
                pCompKey, ppo, dwMilliseconds));
        }
    private:
        HANDLE m_hIOCP;//���ǵ�IO��ɶ˿�
    };
}
