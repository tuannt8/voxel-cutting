// MainFrm.cpp : CMainFrame
//

#include "stdafx.h"
#include "KEGIES.h"
#include "KEGIESDoc.h"

#include "MainFrm.h"
#include "Dialog.h"
#include "KEGIESView.h"
#include "View2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_MENUSELECT()
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F1, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F2, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F3, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F4, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F5, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F6, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F7, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F8, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F9, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F10, &CMainFrame::OnUpdateSpecialF)

	ON_UPDATE_COMMAND_UI(ID_SIDEDIALOG_HIDE, &CMainFrame::OnUpdateSidedialogMenu)
	ON_UPDATE_COMMAND_UI(ID_SIDEDIALOG_LEFT, &CMainFrame::OnUpdateSidedialogMenu)
	ON_UPDATE_COMMAND_UI(ID_SIDEDIALOG_RIGHT, &CMainFrame::OnUpdateSidedialogMenu)
	ON_BN_CLICKED(IDOK, &CMainFrame::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainFrame::OnBnClickedButton1)
	ON_MESSAGE(WM_TEST, &CKEGIESView::OnData)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_EXT,
};


// CMainFrame 

CMainFrame::CMainFrame()
{
	// TODO:
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Error toolbar 1\n");
		return -1;      
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Error toolbar 2\n");
		return -1;      
	}

	m_wndStatusBar.SetPaneInfo(4, ID_TIMER_STATUS, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneText(4, _T("Stopped"));

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	CRect rect;
	int nIndex = m_wndToolBar.GetToolBarCtrl().CommandToIndex(IDD_EDIT_BOX_1);
	m_wndToolBar.SetButtonInfo(nIndex, IDD_EDIT_BOX_1, TBBS_SEPARATOR, 30);
	m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex, &rect);
	m_edit1.Create(WS_VISIBLE, rect, &m_wndToolBar, IDD_EDIT_BOX_1);

	nIndex = m_wndToolBar.GetToolBarCtrl().CommandToIndex(IDD_EDIT_BOX_2);
	m_wndToolBar.SetButtonInfo(nIndex, IDD_EDIT_BOX_2, TBBS_SEPARATOR, 30);
	m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex, &rect);
	m_edit2.Create(WS_VISIBLE, rect, &m_wndToolBar, IDD_EDIT_BOX_2);
	//

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	
	sideDlg.init(this);




	
// 	test.Create(IDD_DIALOG_TEST_VOXEL, this);
// 	test.view = GetActiveView();
// 	test.ShowWindow(SW_SHOW);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT 

	return TRUE;
}

// CMainFrame 

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame

void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);

}


void CMainFrame::OnUpdateSpecialF(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CKEGIESDoc* doc = (CKEGIESDoc*)GetActiveDocument();
	CString title = _T("Unused");

	pCmdUI->Enable(FALSE);
// 	if (doc->document->upDateMenuSpecial(ID_SPECIAL_F1 + pCmdUI->m_nIndex, title))
// 	{
// 		pCmdUI->Enable(TRUE);
// 	}
	pCmdUI->SetText(title);
	
}

void CMainFrame::OnUpdateSidedialogMenu(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nIndex)
	{
	case 0:
		sideDlg.OnUpdateSidedialogHide(pCmdUI);
		break;
	case 2:
		sideDlg.OnUpdateSidedialogLeft(pCmdUI);
		break;
	case 3:
		sideDlg.OnUpdateSidedialogRight(pCmdUI);
		break;
	}
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	int nItemID = pMsg->wParam;
	CKEGIESDoc* doc = (CKEGIESDoc*)GetActiveDocument();

	// Special menu function
// 	for (int i = 0; i< 10; i++)
// 	{
// 		if (nItemID == ID_SPECIAL_F1 + i)
// 		{
// 			doc->document->specialMenuFunction(nItemID);
// 			break;
// 		}
// 	}

	// Side dialog view
	switch(nItemID)
	{
	case ID_SIDEDIALOG_HIDE:
		sideDlg.OnSidedialogHide();
		break;
	case ID_SIDEDIALOG_LEFT:
		sideDlg.OnSidedialogLeft();
		break;
	case ID_SIDEDIALOG_RIGHT:
		sideDlg.OnSidedialogRight();
		break;
	}


	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::setStatusBar( CString status, int index )
{
	m_wndStatusBar.SetPaneText(index, status);
}

void CMainFrame::timerUpdate( bool start )
{
	setStatusBar(start? _T("Running..."):_T("Stopped"), 4);

	CToolBarCtrl& Toolbar = m_wndToolBar.GetToolBarCtrl();
	CImageList *pList = Toolbar.GetImageList();
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(start? ID_ICON_PAUSE:ID_ICON_PLAY));
	pList->Replace(5, hIcon);
	m_wndToolBar.Invalidate();
}


void CMainFrame::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int a;
	a=3;
	a=5;
}


void CMainFrame::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	TRACE(_T("dfdfdfdfdfdf\n"));
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	int ncwidth = 1200; 
	int ncheight = 200; 

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CKEGIESView), CSize(ncwidth*0.7, ncheight), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(View2), CSize(ncwidth*0.3, ncheight), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	

	return TRUE;
}
