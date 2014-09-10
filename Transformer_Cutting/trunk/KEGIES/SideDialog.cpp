#include "StdAfx.h"
#include "SideDialog.h"

#include "MainFrm.h"


#define DIALOG_POSITION "Side dialog's position"
#define DIALOG_SHOW		"Show/hide side dialog bar"

SideDialog::SideDialog(void)
{
	//Load setting
	position = (dialogPos)AppSetting::readIntSetting(DIALOG_POSITION);
	shown = AppSetting::readBoolSetting(DIALOG_SHOW);

	parent = NULL;
}


SideDialog::~SideDialog(void)
{
	// Save setting
	AppSetting::saveIntSetting(position, DIALOG_POSITION);
	AppSetting::saveBoolSetting(shown, DIALOG_SHOW);
}

UINT positionConvert(dialogPos pos)
{
	switch(pos)
	{
	case DIALOG_LEFT:
		return CBRS_LEFT;
	case DIALOG_RIGHT:
		return CBRS_RIGHT;
	}
	return 0;
}

void SideDialog::init(CWnd* pParentWnd)
{
	parent = (CMainFrame*)pParentWnd;
	Create(pParentWnd, IDD_DIALOGBAR, positionConvert(position), IDD_DIALOGBAR);
	ShowWindow(shown);
}

BEGIN_MESSAGE_MAP(SideDialog, CDialogBar)

END_MESSAGE_MAP()


void SideDialog::OnSidedialogHide()
{
	shown =  !shown;

	ShowWindow(shown);
	parent->RecalcLayout(TRUE);
}


void SideDialog::OnSidedialogLeft()
{
	position = DIALOG_LEFT;
}


void SideDialog::OnSidedialogRight()
{
	position = DIALOG_RIGHT;
}


void SideDialog::OnUpdateSidedialogHide(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(shown);
	pCmdUI->Enable(TRUE);	
}


void SideDialog::OnUpdateSidedialogLeft(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(position == DIALOG_LEFT);
	pCmdUI->Enable(TRUE);
}


void SideDialog::OnUpdateSidedialogRight(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(position == DIALOG_RIGHT);
	pCmdUI->Enable(TRUE);
}
