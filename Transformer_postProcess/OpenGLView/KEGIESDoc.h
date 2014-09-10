// KEGIESDoc.h : CKEGIESDoc 
//
#pragma once
#include "resource.h"
#include "myDocument.h"


class CKEGIESDoc : public CDocument
{
public:

	myDocumentPtr m_doc;


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


	afx_msg void OnTimer(UINT nIDEvent);
};


