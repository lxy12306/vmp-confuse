#pragma once
#include "..\Exception\MyExcetpionCpp.h"
namespace LXY
{
//#define MD_Throw_Windows(szError) throw LXY::m_WindowsException(__FILE__,__LINE__,#szError, GetLastError())

	class m_WindowsException :public LXY::m_runtime_error
	{
		/*友元声明以及using定义*/
	private:
		/*私有数据成员*/
		std::string strFunction;
		uint32_t uErrorCode;
	public:
		/*公有数据成员*/
	public:
		/*构造函数与析构函数*/
		m_WindowsException(const char* filename, int line, const char* szError, uint32_t uErrorCode) :
			LXY::m_runtime_error("Windows错误", filename, line),
			strFunction(szError), uErrorCode(uErrorCode) {
		}
		~m_WindowsException() {
		}
	public:
		/*导出函数*/
		/// <summary>
		/// 输入所有错误信息
		/// </summary>
		void e_ShowError() const override
		{
			LXY::m_runtime_error::e_ShowError();
			m_PrintDebug("错误信息<%s>，错误码为:<%x>\r\n\n\n", strFunction.c_str(), uErrorCode);
		}
		/// <summary>
		/// 返回错误字符信息
		/// </summary>
		/// <returns></returns>
		const char* e_GetErrorInfoStr()
		{
			return strFunction.c_str();
		}
		/// <summary>
		/// 返回错误字符代码信息
		/// </summary>
		/// <returns></returns>
		uint32_t e_GetErrorInfoCode()
		{
			return uErrorCode;
		}
	private:
		/*其他非导出功能函数*/
	};
}