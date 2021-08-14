#include <stdio.h>

#include <string>

#include <Windows.h>
#include <strsafe.h>

#include "FILE_PATH.h"
int main() {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	while (true)
	{
		int i_index = 4;
		printf("请输入要打开文件的序号：");
		scanf_s("%d", &i_index);

		switch (i_index)
		{
		case -1: {
			std::string kongzhitaicanshu{ "del " };
			kongzhitaicanshu += NotePad_Work_Path Delete_All_File1" ";
			kongzhitaicanshu += NotePad_Work_Path Delete_All_File2" ";
			kongzhitaicanshu += NotePad_Work_Path Delete_All_File3" ";
			system(kongzhitaicanshu.c_str());
			continue;
		}break;
		default:
			break;
		}


		TCHAR file1[MAX_PATH+1];
		TCHAR file2[MAX_PATH+1];
		TCHAR file3[MAX_PATH+1];
		std::wstring kongzhitaicanshu;
	
		StringCchPrintf(file1, MAX_PATH, TEXT(NotePad_Work_Path Open_File1), i_index);
		StringCchPrintf(file2, MAX_PATH, TEXT(NotePad_Work_Path Open_File2), i_index);
		StringCchPrintf(file3, MAX_PATH, TEXT(NotePad_Work_Path Open_File3), i_index);

		kongzhitaicanshu.clear();
		kongzhitaicanshu += TEXT(NotePad_PATH);
		kongzhitaicanshu += L" ";;

		kongzhitaicanshu += file1;
		kongzhitaicanshu += L" ";
		kongzhitaicanshu += file2;
		kongzhitaicanshu += L" ";
		kongzhitaicanshu += file3;
		kongzhitaicanshu += L" ";


		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(NULL, (LPWSTR)kongzhitaicanshu.c_str(), NULL, NULL, FALSE,
			0, NULL, NULL, &si, &pi)) {
			printf("CreateProcess %s failed (%d).\n", NotePad_PATH, GetLastError());
			getchar();
			return -1;
		}
		CloseHandle(pi.hThread);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
	}
	return 0;
}