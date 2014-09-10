// MainFrm.h : CMainFrame 
//
#include "SideDialog.h"

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // serialization
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)


//
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//	virtual void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
// 
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  //
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	SideDialog sideDlg;
//
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnUpdateSpecialF(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSidedialogMenu(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	void setStatusBar(CString status, int index);
	void timerUpdate(bool start);
};


