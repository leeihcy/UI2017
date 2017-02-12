#pragma once
#include "Other\resource.h"
#include "Dialog\Framework\childframe.h"

class CCodeWebBrowser : public IExplorerMDIClientView, 
						public CAxDialogImpl<CCodeWebBrowser>
{
public:
	enum {IDD = IDD_DIALOG_CODE_WEBBROWSER};
	CCodeWebBrowser(void);
	~CCodeWebBrowser(void);

	BEGIN_MSG_MAP_EX(CCodeWebBrowser)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()

#pragma region
	virtual void**  QueryInterface(REFIID riid);
	virtual ISkinRes*   GetSkinRes();
	virtual void    SetSkinRes(ISkinRes* hSkin);
	virtual HWND    GetHWND();
	virtual long*   GetKey()  { return NULL; }
	virtual UI_RESOURCE_TYPE GetResourceType() { return UI_RESOURCE_SOURCEFILE; }

	virtual bool    GetFilePath(__out TCHAR* szPath);
	virtual bool    SetFilePath(__in  LPCTSTR szPath);
#pragma endregion

public:
	BOOL  OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void  OnSize(UINT nType, CSize size);

protected:
	CComPtr<IWebBrowser2>  m_pWebBrowser; 
	CAxWindow  m_wndIE;

	ISkinRes*  m_pSkin;
	String m_strFilePath;

};
