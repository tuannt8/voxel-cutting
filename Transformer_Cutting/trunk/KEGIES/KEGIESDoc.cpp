// KEGIESDoc.cpp : CKEGIESDoc 
//

#include "stdafx.h"
#include "KEGIES.h"
#include "KEGIESDoc.h"
#include "KEGIESView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKEGIESDoc

IMPLEMENT_DYNCREATE(CKEGIESDoc, CDocument)

BEGIN_MESSAGE_MAP(CKEGIESDoc, CDocument)
	
	ON_COMMAND(ID_TOOLBAR_STARTTHREAD, &CKEGIESDoc::OnToolbarStartthread)
	ON_COMMAND(ID_EXT_LOADOBJ, &CKEGIESDoc::OnExtLoadobj)
	ON_COMMAND(ID_TOOL_CONVERT, &CKEGIESDoc::OnToolConvert)
END_MESSAGE_MAP()


// CKEGIESDoc

CKEGIESDoc::CKEGIESDoc()
{
	// TODO:
	objItem = NULL;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
}

CKEGIESDoc::~CKEGIESDoc()
{
	if(objItem)
	{
		delete objItem;
		objItem = NULL;
	}
	
}

BOOL CKEGIESDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;



	return TRUE;
}

BOOL CKEGIESDoc::openLastDoc()
{
	CString lastDocPath = AppSetting::getObjectPath();
	if(!lastDocPath.IsEmpty())
	{
		OnOpenDocument(lastDocPath);
	}
	return FALSE;
}



// CKEGIESDoc serialization

void CKEGIESDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:
	}
	else
	{
		// TODO:
	}
}


// CKEGIESDoc 

#ifdef _DEBUG
void CKEGIESDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CKEGIESDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


BOOL CKEGIESDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;



	return TRUE;
}


void CKEGIESDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class


	CDocument::OnCloseDocument();
}
DWORD CKEGIESDoc::StartThread (LPVOID param)
{
	CKEGIESDoc* pointer = (CKEGIESDoc*)param;
	return 0;
}

void CKEGIESDoc::OnToolbarStartthread()
{
	// TODO: Add your command handler code here
	CWinThread *thrd = AfxBeginThread ((AFX_THREADPROC)CKEGIESDoc::StartThread, (LPVOID)this);
	thrd->SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
}

void CKEGIESDoc::OnExtLoadobj()
{

}


void CKEGIESDoc::OnToolConvert()
{
	// TODO: Add your command handler code here

}
