#pragma once
#if 0

class CRecentFileList
{
	// Constructors
public:
	CRecentFileList(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);

	// Attributes
	int GetSize() const;
	CString& operator[](int nIndex);

	// Operations
	virtual void Remove(int nIndex);
	virtual void Add(LPCTSTR lpszPathName);
	virtual BOOL GetDisplayName(CString& strName, int nIndex,
		LPCTSTR lpszCurDir, int nCurDir, BOOL bAtLeastName = TRUE) const;
	virtual void UpdateMenu(CCmdUI* pCmdUI);
	virtual void ReadList();    // reads from registry or ini file
	virtual void WriteList();   // writes to registry or ini file

	// Implementation
	virtual ~CRecentFileList();

	int m_nSize;                // contents of the MRU list
	CString* m_arrNames;
	CString m_strSectionName;   // for saving
	CString m_strEntryFormat;
	UINT m_nStart;              // for displaying
	int m_nMaxDisplayLength;
	CString m_strOriginal;      // original menu item contents
};
#endif