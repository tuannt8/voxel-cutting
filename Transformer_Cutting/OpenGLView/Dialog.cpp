#include "StdAfx.h"
#include "Dialog.h"

//#include "KEGIESView.h"

BEGIN_MESSAGE_MAP(dialogVoxel, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &dialogVoxel::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &dialogVoxel::OnEnChangeEdit1)
END_MESSAGE_MAP()

Dialog::Dialog()
{
}


Dialog::~Dialog()
{
}

void dialogVoxel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

dialogVoxel::dialogVoxel() : CDialog()
{

}


void dialogVoxel::OnBnClickedButton1()
{
	AfxGetMainWnd()->SendMessage(WM_TEST, 0, (LPARAM)this);
}


void dialogVoxel::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
