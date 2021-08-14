#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <debugapi.h>
#include <utility>

namespace LXY
{
	/// <summary>
	/// ��������ӡ����
	/// </summary>
	/// <param name="_Format">��ʽ���ַ���</param>
	/// <param name=""></param>
	/// <returns>��ӡ�ĳ���</returns>
	inline size_t m_PrintDebug(const char* const _Format,...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //Ԥ����Ҫ�ĳ���

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//ƴ���ַ���
		va_end(vlArgs);
		OutputDebugStringA(szBuffer);
		delete[] szBuffer;

		return nLen;
	}
	/// <summary>
	/// ��ӡ��MessageBox
	/// </summary>
	/// <param name="_Format">��ʽ���ַ���</param>
	/// <param name=""></param>
	/// <returns>��ӡ���ַ�</returns>
	inline size_t m_PrintMessageBox(const char* const _Format, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //Ԥ����Ҫ�ĳ���

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//ƴ���ַ���
		va_end(vlArgs);
		MessageBoxA(NULL, szBuffer, "m_MessageBoxPrint", MB_OK);
		delete[] szBuffer;

		return nLen;
	}
	inline size_t m_PrintMessageBox2(const char* szOption,const char* const _Format, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //Ԥ����Ҫ�ĳ���

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//ƴ���ַ���
		va_end(vlArgs);
		MessageBoxA(NULL, szBuffer, szOption, MB_OK);
		delete[] szBuffer;

		return nLen;
	}

	//����ѡ��ͳ���
	inline std::pair<size_t, DWORD> m_PrintMessageBox3(const char* szOption,unsigned uChoice, const char* const _Format, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //Ԥ����Ҫ�ĳ���

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//ƴ���ַ���
		va_end(vlArgs);
		DWORD dwRet = MessageBoxA(NULL, szBuffer, szOption, uChoice);
		delete[] szBuffer;

		return {nLen,dwRet};
	}

	/// <summary>
	/// ��ӡ��Ϣ���ļ���
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
			if (error != 0)//ʧ����
			{
				DebugBreak();
			}
		}
		va_list vlArgs = NULL;
		va_start(vlArgs, _Format);
		size_t nLen = _vscprintf(_Format, vlArgs) + 1; //Ԥ����Ҫ�ĳ���

		char* szBuffer = new char[nLen];
		_vsnprintf_s(szBuffer, nLen, nLen, _Format, vlArgs);//ƴ���ַ���
		fprintf_s(pFile, szBuffer);

		va_end(vlArgs);
		delete[] szBuffer;
		if(szFileName != NULL)
			fclose(pFile);
		return nLen;
	}
}
