#pragma once
#include "..\Exception\MyExcetpionCpp.h"
namespace LXY
{
//#define MD_Throw_Windows(szError) throw LXY::m_WindowsException(__FILE__,__LINE__,#szError, GetLastError())

	class m_WindowsException :public LXY::m_runtime_error
	{
		/*��Ԫ�����Լ�using����*/
	private:
		/*˽�����ݳ�Ա*/
		std::string strFunction;
		uint32_t uErrorCode;
	public:
		/*�������ݳ�Ա*/
	public:
		/*���캯������������*/
		m_WindowsException(const char* filename, int line, const char* szError, uint32_t uErrorCode) :
			LXY::m_runtime_error("Windows����", filename, line),
			strFunction(szError), uErrorCode(uErrorCode) {
		}
		~m_WindowsException() {
		}
	public:
		/*��������*/
		/// <summary>
		/// �������д�����Ϣ
		/// </summary>
		void e_ShowError() const override
		{
			LXY::m_runtime_error::e_ShowError();
			m_PrintDebug("������Ϣ<%s>��������Ϊ:<%x>\r\n\n\n", strFunction.c_str(), uErrorCode);
		}
		/// <summary>
		/// ���ش����ַ���Ϣ
		/// </summary>
		/// <returns></returns>
		const char* e_GetErrorInfoStr()
		{
			return strFunction.c_str();
		}
		/// <summary>
		/// ���ش����ַ�������Ϣ
		/// </summary>
		/// <returns></returns>
		uint32_t e_GetErrorInfoCode()
		{
			return uErrorCode;
		}
	private:
		/*�����ǵ������ܺ���*/
	};
}