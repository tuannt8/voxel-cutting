// FilterCutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "FilterCutDialog.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "cutSurfTreeMngr2.h"
#include "skeleton.h"
#include "myDocment.h"

using namespace std;
// FilterCutDialog dialog

IMPLEMENT_DYNAMIC(FilterCutDialog, CDialogEx)

FilterCutDialog::FilterCutDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(FilterCutDialog::IDD, pParent)
{

}

FilterCutDialog::~FilterCutDialog()
{
}

void FilterCutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, joinComboBox);
	DDX_Control(pDX, IDC_COMBO2, JoinTypeComboBox);
}


BEGIN_MESSAGE_MAP(FilterCutDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &FilterCutDialog::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &FilterCutDialog::OnCbnSelchangeJoin)
	ON_CBN_SELCHANGE(IDC_COMBO2, &FilterCutDialog::OnCbnSelchangeJoinType)
	ON_BN_CLICKED(IDC_BUTTON2, &FilterCutDialog::OnBnClickedClearFilter)
END_MESSAGE_MAP()


// FilterCutDialog message handlers

void FilterCutDialog::initFromCutTree(cutSurfTreeMngr2 *cutMangr)
{
	// The neighbor list
	arrayVec2i neighborPair = cutMangr->poseMngr.neighborPair;
	vector<bone*> sortedBone = cutMangr->poseMngr.sortedBone;

	for (auto nb : neighborPair)
	{
		CString joinStr;
		joinStr = sortedBone[nb[0]]->m_name + _T(" -> ") + sortedBone[nb[1]]->m_name;
		joinComboBox.AddString(joinStr);

		chosenPose.push_back(NONE_NB);
	}

	// Filter combo box
	JoinTypeComboBox.AddString(_T("None"));
	JoinTypeComboBox.AddString(_T("X+"));
	JoinTypeComboBox.AddString(_T("X-"));
	JoinTypeComboBox.AddString(_T("Y+"));
	JoinTypeComboBox.AddString(_T("Y-"));
	JoinTypeComboBox.AddString(_T("Z+"));
	JoinTypeComboBox.AddString(_T("Z-"));

	joinComboBox.SetCurSel(0);
	OnCbnSelchangeJoin();
}


void FilterCutDialog::OnCbnSelchangeJoin()
{
	int idx = joinComboBox.GetCurSel();
	int typeIdx = (int)chosenPose[idx] + 1;

	JoinTypeComboBox.SetCurSel(typeIdx);
}


void FilterCutDialog::OnCbnSelchangeJoinType()
{
	int idx = joinComboBox.GetCurSel();
	neighborPos cpose = (neighborPos)(JoinTypeComboBox.GetCurSel() - 1);

	chosenPose[idx] = cpose;
}

void FilterCutDialog::OnBnClickedButton1()
{
	// Update filter
	doc->updateFilterCutGroup();

	// Print out
	cout << "Update filter group: " << endl;
	for (int i = 0; i < chosenPose.size(); i++)
	{
		CString nbText;
		joinComboBox.GetLBText(i, nbText);

		int poseIdx = (int)chosenPose[i] + 1;
		CString poseText;
		JoinTypeComboBox.GetLBText(poseIdx, poseText);

		wcout << " - " << nbText.GetBuffer() << ": " << poseText.GetBuffer() << endl;
	}
	cout << "-------------------------" << endl;
}

void FilterCutDialog::OnBnClickedClearFilter()
{
	for (auto & p : chosenPose)
	{
		p = NONE_NB;
	}

	joinComboBox.SetCurSel(0);
	OnCbnSelchangeJoin();
}

