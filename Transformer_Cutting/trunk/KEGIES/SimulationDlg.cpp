// SimulationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KEGIES.h"
#include "SimulationDlg.h"
#include "afxdialogex.h"
#include "KEGIESDoc.h"
#include "KEGIESView.h"
#include "Utility.h"


// SimulationDlg dialog

IMPLEMENT_DYNAMIC(SimulationDlg, CDialogEx)

SimulationDlg::SimulationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SimulationDlg::IDD, pParent)
{

}

SimulationDlg::~SimulationDlg()
{
}

void SimulationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OBJECT_PATH, m_objPathText);
	DDX_Control(pDX, IDC_SIM_DATA_PATH, m_simDataPath);
}


BEGIN_MESSAGE_MAP(SimulationDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SimulationDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OBJ_PATH_CHANGE, &SimulationDlg::OnBnClickedButtonObjPathChange)
	ON_BN_CLICKED(IDC_BUTTON_SIM_PATH_CHANGE, &SimulationDlg::OnBnClickedButtonSimPathChange)
END_MESSAGE_MAP()


// SimulationDlg message handlers

void SimulationDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(AppSetting::getObjectPath().IsEmpty()
		|| AppSetting::getSimDataPath().IsEmpty())
	{
		AfxMessageBox("Data is not available");
		return;
	}

	CKEGIESDoc* pDoc = (CKEGIESDoc*)Utility::GetSDIActiveDocument();

	CKEGIESView* pView = (CKEGIESView*)Utility::GetActiveView();
	pView->StartStopSim();
}

void SimulationDlg::updateSetting()
{
	m_objPathText.SetWindowText(AppSetting::getObjectPath());
	m_simDataPath.SetWindowText(AppSetting::getSimDataPath());
}

void SimulationDlg::OnBnClickedButtonObjPathChange()
{
	CString objPath = Utility::OpenFileDialog(FILE_TYPE_OBJ, "Open simulation data");
	if(!objPath.IsEmpty())
	{
		AppSetting::setObjPath(objPath.GetBuffer());
		m_objPathText.SetWindowText(objPath);
		CKEGIESDoc* pDoc = (CKEGIESDoc*)Utility::GetSDIActiveDocument();
		pDoc->OnOpenDocument(objPath.GetBuffer());
	}
}


void SimulationDlg::OnBnClickedButtonSimPathChange()
{
	CString simPath = Utility::OpenFileDialog(FILE_TYPE_PATH, "Open simulation data");
	if(!simPath.IsEmpty())
	{
		AppSetting::setSimDataPath(simPath.GetBuffer());
		m_simDataPath.SetWindowText(simPath);
	}
}
