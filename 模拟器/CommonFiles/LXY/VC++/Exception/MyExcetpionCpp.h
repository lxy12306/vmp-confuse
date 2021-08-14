#pragma once
#include <stdexcept>
#include <string>
#include "..\Message\MyPrint.h"

namespace LXY
{
	//#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(#szError,__FILE__,__LINE__)    
	class m_runtime_error :public std::runtime_error
	{
		/*友元声明以及using定义*/
	private:
		/*私有数据成员*/
		uint32_t uLine;
		std::string strFile;
	public:
		/*公有数据成员*/

	public:
		/*构造函数与析构函数*/
		m_runtime_error(const char* szError, const char* filename, int line) :runtime_error(szError),
			strFile(filename), uLine(line) {
		}
		~m_runtime_error() {
		}
	public:
		/*导出函数*/
		/// <summary>
		/// 显式异常信息和行数
		/// </summary>
		virtual void e_ShowError()const 
		{
			m_PrintDebug("\r\n%s,引发异常的文件为:<%s>,行数为:<%u>\r\n\n\n", what(), strFile.c_str(), uLine);
		}
	private:
		/*其他非导出功能函数*/

	};

}