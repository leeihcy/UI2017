// UISkinPack.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\3rd\zip\ziputil.h"

// /pack dirpath zippath

int _tmain(int argc, _TCHAR* argv[])
{
// 	ZipDir(
// 		TEXT("D:\\Skin"), 
// 		TEXT("D:\\Work\\git\\desktop_c\\LenovoServiceClient\\bin\\Release\\Skin.zip"));
// 	return 0;
	
	if (argc < 2)
		return 0;

	if (0 == _tcscmp(argv[1], TEXT("/pack")))
	{
		if (argc != 4)
			return 0;

		CString strDirPath(argv[2]);
		CString strZipPath(argv[3]);

// 		OutputDebugString(strDirPath);
// 		OutputDebugString(TEXT("\r\n"));
// 		OutputDebugString(strZipPath);
// 		OutputDebugString(TEXT("\r\n"));
// 		MessageBox(nullptr, strDirPath, L"dir", 0);
// 		MessageBox(nullptr, strZipPath, L"zip", 0);
		ZipDir(strDirPath, strZipPath);
	}
	return 0;
}

