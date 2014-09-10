#pragma once
#include "afxext.h"
#include "stdafx.h"
#include "resource.h"


class CMainFrame;

typedef enum dialogPos_
{
	DIALOG_LEFT = 0,
	DIALOG_RIGHT
}dialogPos;

class SideDialog :
	public CDialogBar
{
public:
	SideDialog(void);
	~SideDialog(void);

	dialogPos position;
	bool shown;

	CMainFrame* parent;
public:
	void init(CWnd* pParentWnd);

	void OnSidedialogHide();
	void OnSidedialogLeft();
	void OnSidedialogRight();
	void OnUpdateSidedialogHide(CCmdUI *pCmdUI);
	void OnUpdateSidedialogLeft(CCmdUI *pCmdUI);
	void OnUpdateSidedialogRight(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()

};

