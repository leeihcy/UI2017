#pragma once

namespace UI
{
    enum OBJ_TYPE;
}

void  MsgBox(LPCTSTR szText, LPCTSTR szTitle = NULL);
void  GetUIEditorConfigFilePath(TCHAR* pszPath);

bool  WaitForEnter2Confirm(HWND hWnd);
bool  SaveTemplateResFileTo(LPCTSTR szDstPath, UINT nId, LPCTSTR szResType);

void  ParsePropertyCommand(LPCTSTR  szCmdLine, String& strCommand, String& strArgs);
void  ParsePropertyCommandArg(LPCTSTR  szArgs, String& strKey, String& strValue);

void  GetObjectTypeBitmap(OBJ_TYPE eType, UINT nControlType, IRenderBitmap** ppRenderBitma);

void  TrimStringLeft(String&  str, const TCHAR c = _T(' '));
void  TrimStringRight(String&  str, const TCHAR c = _T(' '));
void  TrimString(String&  str, const TCHAR c = _T(' '));

void  GetAppPath_(TCHAR* strPath);
BOOL  IsFullPath(LPCTSTR szPath);
BOOL  GetPathFileName(LPCTSTR szPath, TCHAR* szOutDir);
BOOL  GetPathFileExt(LPCTSTR szPath, TCHAR* szOutExt);
BOOL  GetPathDir(LPCTSTR szPath, TCHAR* szOutDir);
BOOL  CalcRelativePathToFile(
        LPCTSTR szFile, LPCTSTR szPath, TCHAR* szOutRelativePath);
BOOL  CalcRelativePathToDir(
        LPCTSTR szDir, LPCTSTR szPath, TCHAR* szOutRelativePath);
BOOL  CalcFullPathByRelative(
        LPCTSTR _szDir, LPCTSTR szRelative, TCHAR* szOut);

int  FontHeight2Size(int nHeight);
int  FontSize2Height(int nSize);

bool CreateEmptyXmlFile(LPCTSTR szPath, LPCTSTR szRoot, LPCTSTR szNode);