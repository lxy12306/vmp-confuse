#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <debugapi.h>
#include <utility>

namespace LXY
{
	/// <summary>
	/// 调试器打印函数
	/// </summary>
	/// <param name="_Format">格式化字符串</param>
	/// <param name=""></param>
	/// <returns>打印的长度</returns>
	inline size_t m_PrintDebug(const char* const _Format,...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //预判需要的长度

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//拼接字符串
		va_end(vlArgs);
		OutputDebugStringA(szBuffer);
		delete[] szBuffer;

		return nLen;
	}
	/// <summary>
	/// 打印到MessageBox
	/// </summary>
	/// <param name="_Format">格式化字符串</param>
	/// <param name=""></param>
	/// <returns>打印的字符</returns>
	inline size_t m_PrintMessageBox(const char* const _Format, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //预判需要的长度

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//拼接字符串
		va_end(vlArgs);
		MessageBoxA(NULL, szBuffer, "m_MessageBoxPrint", MB_OK);
		delete[] szBuffer;

		return nLen;
	}
	inline size_t m_PrintMessageBox2(const char* szOption,const char* const _Format, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //预判需要的长度

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//拼接字符串
		va_end(vlArgs);
		MessageBoxA(NULL, szBuffer, szOption, MB_OK);
		delete[] szBuffer;

		return nLen;
	}

	//返回选择和长度
	inline std::pair<size_t, DWORD> m_PrintMessageBox3(const char* szOption,unsigned uChoice, const char* const _Format, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //预判需要的长度

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//拼接字符串
		va_end(vlArgs);
		DWORD dwRet = MessageBoxA(NULL, szBuffer, szOption, uChoice);
		delete[] szBuffer;

		return {nLen,dwRet};
	}

	/// <summary>
	/// 打印信息到文件中
	/// </summary>
	/// <param name="szFileName"></param>
	/// <param name="_Format"></param>
	/// <param name=""></param>
	/// <returns></returns>
	inline size_t m_PrintFile(const char* szFileName, const char* const _Format, ...)
	{
		FILE* pFile;
		if (szFileName == NULL)
			pFile = stdout;
		else
		{
			auto error = fopen_s(&pFile, szFileName, "a");
			if (error != 0)//失败了
			{
				DebugBreak();
			}
		}
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //预判需要的长度

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//拼接字符串
		fprintf_s(pFile, szBuffer);

		va_end(vlArgs);
		delete[] szBuffer;
		if(szFileName != NULL)
			fclose(pFile);
		return nLen;
	}
}
