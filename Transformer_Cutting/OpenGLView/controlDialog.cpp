#include "StdAfx.h"
#include "controlDialog.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(controlDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &controlDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_1_LOAD_MESH, &controlDialog::OnBnClicked1LoadMesh)
END_MESSAGE_MAP()

controlDialog::controlDialog(void)
{
}


controlDialog::~controlDialog(void)
{
}

void controlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL controlDialog::OnInitDialog()
{
	BOOL a = CDialog::OnInitDialog();


	return a;
}

BOOL controlDialog::DestroyWindow()
{
	return CDialog::DestroyWindow();
}


void controlDialog::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
}

void controlDialog::disableAllControl()
{
	GetDlgItem(IDC_1_LOAD_MESH)->EnableWindow(FALSE);
	GetDlgItem(IDC_1_VOXEL_RES)->EnableWindow(FALSE);
	GetDlgItem(IDC_1_CREATE_VOXEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_1_BUILD_TREE)->EnableWindow(FALSE);
}

void controlDialog::changeToState(APP_STATE s)
{
	disableAllControl();

	switch(s)
	{
	case STATE_1_LOAD_FILE:
		GetDlgItem(IDC_1_LOAD_MESH)->EnableWindow(TRUE);
		break;
	case STATE_1_GEN_VOXEL:
		GetDlgItem(IDC_1_VOXEL_RES)->EnableWindow(TRUE);
		GetDlgItem(IDC_1_CREATE_VOXEL)->EnableWindow(TRUE);
		break;
	}
}


void controlDialog::OnBnClicked1LoadMesh()
{
	CFileDialog fOpenDlg(TRUE, _T("stl"), _T("Load mesh file"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, 
		_T("STL file (*.stl)|*.stl||"), this);
	fOpenDlg.m_pOFN->lpstrTitle=_T("Load mesh file");

	if(fOpenDlg.DoModal()==IDOK)
	{
		CString path = (LPCTSTR)fOpenDlg.GetPathName();
		TRACE(path);
		// Do something useful here
	}

	changeToState(STATE_1_GEN_VOXEL);
}
