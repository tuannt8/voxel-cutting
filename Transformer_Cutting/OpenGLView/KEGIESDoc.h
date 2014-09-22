// KEGIESDoc.h : CKEGIESDoc 
//
#pragma once
#include "resource.h"
#include "cutSurfTreeMngr2.h"
#include "detailSwapManager.h"
#include "coordAssignManager.h"
#include "groupCutManager.h"
#include "myDocment.h"
#include "energyDoc.h"
#include "nonSymDoc.h"

class CKEGIESDoc : public CDocument
{
public:
	nonSymDoc document;

	appMode m_mode;
public:


protected: // serialization
	CKEGIESDoc();
	DECLARE_DYNCREATE(CKEGIESDoc)

public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();

	virtual void Serialize(CArchive& ar);

public:
	virtual ~CKEGIESDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:

	CObj* objItem;

public:
	BOOL openLastDoc();
	static DWORD  StartThread (LPVOID param);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnToolbarStartthread();

	afx_msg void OnExtLoadobj();
	afx_msg void OnToolConvert();
	void test();
};


