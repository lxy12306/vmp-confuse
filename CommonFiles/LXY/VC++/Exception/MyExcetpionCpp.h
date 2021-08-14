#pragma once
#include <stdexcept>
#include <string>
#include "..\Message\MyPrint.h"

namespace LXY
{
	//#define MD_Throw_Runtime(szError) throw LXY::m_runtime_error(#szError,__FILE__,__LINE__)    
	class m_runtime_error :public std::runtime_error
	{
		/*��Ԫ�����Լ�using����*/
	private:
		/*˽�����ݳ�Ա*/
		uint32_t uLine;
		std::string strFile;
	public:
		/*�������ݳ�Ա*/

	public:
		/*���캯������������*/
		m_runtime_error(const char* szError, const char* filename, int line) :runtime_error(szError),
			strFile(filename), uLine(line) {
		}
		~m_runtime_error() {
		}
	public:
		/*��������*/
		/// <summary>
		/// ��ʽ�쳣��Ϣ������
		/// </summary>
		virtual void e_ShowError()const 
		{
			m_PrintDebug("\r\n%s,�����쳣���ļ�Ϊ:<%s>,����Ϊ:<%u>\r\n\n\n", what(), strFile.c_str(), uLine);
		}
	private:
		/*�����ǵ������ܺ���*/

	};

}