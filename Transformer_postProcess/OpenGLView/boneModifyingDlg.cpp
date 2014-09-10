// boneModifyingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "boneModifyingDlg.h"
#include "afxdialogex.h"
#include "meshMnger.h"


// boneModifyingDlg dialog

IMPLEMENT_DYNAMIC(boneModifyingDlg, CDialogEx)

boneModifyingDlg::boneModifyingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(boneModifyingDlg::IDD, pParent)
{

}

boneModifyingDlg::~boneModifyingDlg()
{
}

void boneModifyingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, boneList);
	DDX_Control(pDX, IDC_EDIT1, moveSpeed);
	DDX_Control(pDX, IDC_EDIT3, boneScale);
}


BEGIN_MESSAGE_MAP(boneModifyingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &boneModifyingDlg::ReduceX)
	ON_BN_CLICKED(IDC_BUTTON4, &boneModifyingDlg::IncreaseX)
	ON_BN_CLICKED(IDC_BUTTON2, &boneModifyingDlg::ReduceY)
	ON_BN_CLICKED(IDC_BUTTON5, &boneModifyingDlg::increaseY)
	ON_BN_CLICKED(IDC_BUTTON3, &boneModifyingDlg::reduceZ)
	ON_BN_CLICKED(IDC_BUTTON6, &boneModifyingDlg::increase_Z)
END_MESSAGE_MAP()


// boneModifyingDlg message handlers


void boneModifyingDlg::ReduceX()
{
	int curIdx = boneList.GetCurSel();
	arrayBoneTrans[curIdx][0] -= getMoveStep();
}


void boneModifyingDlg::IncreaseX()
{
	int curIdx = boneList.GetCurSel();
	arrayBoneTrans[curIdx][0] += getMoveStep();
}


void boneModifyingDlg::ReduceY()
{
	int curIdx = boneList.GetCurSel();
	arrayBoneTrans[curIdx][1] -= getMoveStep();
}


void boneModifyingDlg::increaseY()
{
	int curIdx = boneList.GetCurSel();
	arrayBoneTrans[curIdx][1] += getMoveStep();
}


void boneModifyingDlg::reduceZ()
{
	int curIdx = boneList.GetCurSel();
	arrayBoneTrans[curIdx][2] -= getMoveStep();
}

void boneModifyingDlg::increase_Z()
{
	int curIdx = boneList.GetCurSel();
	arrayBoneTrans[curIdx][2] += getMoveStep();
}


void boneModifyingDlg::init(std::vector<meshObjectPtr> meshArray)
{
	s_meshArray = meshArray;

	// Init combo box
	for (int i = 0; i < s_meshArray.size(); i++)
	{
		boneList.AddString(s_meshArray[i]->s_bone->m_name);
	}

	arrayBoneTrans.resize(s_meshArray.size());

	boneList.SetCurSel(0);
	changeBoneSlection(0);

	moveSpeed.SetWindowText(_T("0.2"));
	boneScale.SetWindowText(_T("1.0"));
}

void boneModifyingDlg::changeBoneSlection(int idx)
{
	
}

float boneModifyingDlg::getMoveStep()
{
	CString strStep;
	moveSpeed.GetWindowText(strStep);
	return _ttof(strStep);
}


