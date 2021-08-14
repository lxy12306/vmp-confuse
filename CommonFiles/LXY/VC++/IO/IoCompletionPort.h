#pragma once
#include "F:/Code/object/CommonFiles/CmdHdr.h"
namespace LXY
{
    /// <summary>
    /// 对IO完成端口的封装
    /// </summary>
    class CIOCP {
    public:
        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="nMaxConcurrency">最大的线程数量</param>
        /// <returns>无</returns>
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
        /// 关闭IO完成端口
        /// </summary>
        /// <returns>是否成功</returns>
        BOOL Close() {
            BOOL bResult = CloseHandle(m_hIOCP);
            m_hIOCP = NULL;
            return(bResult);
        }
        /// <summary>
        /// 创建一个新的IO完成端口
        /// </summary>
        /// <param name="nMaxConcurrency">允许并发的线程最大数量</param>
        /// <returns></returns>
        BOOL Create(int nMaxConcurrency = 0) {
            m_hIOCP = CreateIoCompletionPort(
                INVALID_HANDLE_VALUE, NULL, 0, nMaxConcurrency);
            chASSERT(m_hIOCP != NULL);
            return(m_hIOCP != NULL);
        }
        /// <summary>
        /// 关联端口和设备句柄
        /// </summary>
        /// <param name="hDevice">需要关联的设备句柄</param>
        /// <param name="CompKey">关联的key</param>
        /// <returns>是否成功</returns>
        BOOL AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey) {
            BOOL fOk = (CreateIoCompletionPort(hDevice, m_hIOCP, CompKey, 0)
                == m_hIOCP);
            chASSERT(fOk);
            return(fOk);
        }
        /// <summary>
        /// 关联套接字和我们的完成端口
        /// </summary>
        /// <param name="hSocket">套接字句柄</param>
        /// <param name="CompKey">完成端口</param>
        /// <returns>是否成功</returns>
        BOOL AssociateSocket(SOCKET hSocket, ULONG_PTR CompKey) {
            return(AssociateDevice((HANDLE)hSocket, CompKey));
        }

        /// <summary>
        /// 像IO完成端口发送通知
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
        /// 获取IO完成端口的通知
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
        HANDLE m_hIOCP;//我们的IO完成端口
    };
}
