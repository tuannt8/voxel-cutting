// KEGIESDoc.cpp : CKEGIESDoc 
//

#include "stdafx.h"
#include "KEGIES.h"
#include "KEGIESDoc.h"
#include "KEGIESView.h"
#include "log.h"
#include <bitset>
#include <climits>
#include <ostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif // _DEBUG


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
	m_mode = MODE_NONE;
	
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

// 	if (m_mode == MODE_FINDING_CUT_SURFACE)
// 	{
// 		CTimeTick timeTool;
// 
// 		timeTool.SetStart();
// 
// 		testCut.loadVoxelAndSkeleton();
// 		testCut.ParseSkeletonOrderSymf();
// 		testCut.decideOcTreeSize();
// 		testCut.estimateBonePosUsingVoxel();
// 
// 		timeTool.SetEnd();
// 		Util::log("Time: %f", timeTool.GetTick());
// 	}
// 	else if (m_mode == MODE_SPLIT_BONE_GROUP)
// 	{
// 		groupCutMngr.init();
// 		groupCutMngr.loadMeshBox("../meshBox158.txt");
// 		groupCutMngr.manualInit();
// 	}

//	CWinThread *thrd = AfxBeginThread((AFX_THREADPROC)CKEGIESDoc::StartThread, (LPVOID)this);

	command::startThread();

	// test
	test();

	return TRUE;
}

BOOL CKEGIESDoc::openLastDoc()
{

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
	command::logData::getInstance()->setTerminate();

	CDocument::OnCloseDocument();
}
DWORD CKEGIESDoc::StartThread (LPVOID param)
{
	CKEGIESDoc* pointer = (CKEGIESDoc*)param;

	std::string s;
	getline(std::cin, s);
	
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

void CKEGIESDoc::test()
{

}
