// cutBoneGroupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cutBoneGroupDlg.h"
#include "afxdialogex.h"
#include "groupCutManager.h"


// cutBoneGroupDlg dialog

IMPLEMENT_DYNAMIC(cutBoneGroupDlg, CDialogEx)

cutBoneGroupDlg::cutBoneGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(cutBoneGroupDlg::IDD, pParent)
{

}

cutBoneGroupDlg::~cutBoneGroupDlg()
{
}

void cutBoneGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, boneGoupListBox);
	DDX_Control(pDX, IDC_EDIT1, poseCurIdxText);
	DDX_Control(pDX, IDC_EDIT2, PoseTotalText);
	DDX_Control(pDX, IDC_EDIT3, curIdxInPoseText);
	DDX_Control(pDX, IDC_EDIT4, totalIdxInPoseText);
}


BEGIN_MESSAGE_MAP(cutBoneGroupDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &cutBoneGroupDlg::OnCbnSelchangeBoneGoup)
	ON_BN_CLICKED(IDC_BUTTON1, &cutBoneGroupDlg::previousPose)
	ON_BN_CLICKED(IDC_BUTTON2, &cutBoneGroupDlg::nextPoseClick)
	ON_BN_CLICKED(IDC_BUTTON3, &cutBoneGroupDlg::previousConfigureClick)
	ON_BN_CLICKED(IDC_BUTTON4, &cutBoneGroupDlg::NextCongifureClick)
	ON_BN_CLICKED(IDC_BUTTON5, &cutBoneGroupDlg::AcceptClick)
	ON_EN_CHANGE(IDC_EDIT1, &cutBoneGroupDlg::poseIdxChange)
	ON_EN_CHANGE(IDC_EDIT3, &cutBoneGroupDlg::configureIdxChange)
	ON_BN_CLICKED(IDOK, &cutBoneGroupDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// cutBoneGroupDlg message handlers


void cutBoneGroupDlg::OnCbnSelchangeBoneGoup()
{
	changeBoneSlect(boneGoupListBox.GetCurSel());
}


void cutBoneGroupDlg::previousPose()
{
	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int curIdx = _ttoi(stext) - 1;

	if (setPoseSelection(curIdx))
	{
		AcceptClick();
	}

//	
}


void cutBoneGroupDlg::nextPoseClick()
{
	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int curIdx = _ttoi(stext) + 1;

	if (setPoseSelection(curIdx))
	{
		AcceptClick();
	}
}


void cutBoneGroupDlg::previousConfigureClick()
{
	CString stext;
	curIdxInPoseText.GetWindowText(stext);
	int curIdx = StrToInt(stext) - 1;
	if (setselectIdxInPose(curIdx))
	{
		stext.Format(_T("%d"), curIdx);
		curIdxInPoseText.SetWindowText(stext);
	}

	AcceptClick();
}


void cutBoneGroupDlg::NextCongifureClick()
{
	CString stext;
	curIdxInPoseText.GetWindowText(stext);
	int curIdx = StrToInt(stext) + 1;
	if (setselectIdxInPose(curIdx))
	{
		stext.Format(_T("%d"), curIdx);
		curIdxInPoseText.SetWindowText(stext);
	}

	AcceptClick();
}


void cutBoneGroupDlg::AcceptClick()
{
	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int curPoseIdx = _ttoi(stext);

	curIdxInPoseText.GetWindowText(stext);
	int curIdxInPose = StrToInt(stext);

	int boneIdx = boneGoupListBox.GetCurSel();

	idxChoosen[boneIdx] = Vec2i(curPoseIdx, curIdxInPose);
}


void cutBoneGroupDlg::poseIdxChange()
{

}


void cutBoneGroupDlg::configureIdxChange()
{

}

void cutBoneGroupDlg::Init(groupCutManager* _groupCutMngr)
{
	groupCutMngr = _groupCutMngr;
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;

	for (int i = 0; i < groupBoneArray->size(); i++)
	{
		idxChoosen.push_back(Vec2i(-1, -1));
		boneGoupListBox.AddString(groupBoneArray->at(i).sourcePiece->boneName);
	}

	boneGoupListBox.ResetContent();
	for (int i = 0; i < groupBoneArray->size(); i++)
	{
		CString name = groupBoneArray->at(i).sourcePiece->boneName;
		boneGoupListBox.AddString(name);
	}

	boneGoupListBox.SetCurSel(0);
	changeBoneSlect(0);
}

void cutBoneGroupDlg::changeBoneSlect(int boneIdx)
{
	// Refresh list box
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;

	// Update index
	groupCutMngr->curBoneIdx = boneIdx;
	groupCut *curG = &groupBoneArray->at(boneIdx);
	Vec2i selected = idxChoosen[boneIdx];

	// pose
	CString a; a.Format(_T("%d"), curG->boxPose.poseMap.size());
	PoseTotalText.SetWindowText(a);

	int selectPoseIdx = selected[0] == -1 ? 0 : selected[0];
	a.Format(_T("%d"), selectPoseIdx);
	poseCurIdxText.SetWindowText(a);

	// configuration
	int configInPoseIdx = selected[1] == -1 ? 0 : selected[1];

	neighborPose curP = curG->boxPose.getPoseByIdx(selectPoseIdx);
	a.Format(_T("%d"), curP.nodeGroupBoneCut.size());
	totalIdxInPoseText.SetWindowText(a);

	a.Format(_T("%d"), configInPoseIdx);
	curIdxInPoseText.SetWindowText(a);
}

bool cutBoneGroupDlg::setPoseSelection(int poseIdx)
{
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;

	groupCut *curG = &groupBoneArray->at(boneGoupListBox.GetCurSel());

	int nodeIdxInPose = groupCutMngr->updateToPoseIdx(poseIdx);

	if (nodeIdxInPose == -1)
	{
		return false;
	}

	neighborPose pp = curG->boxPose.getPoseByIdx(poseIdx);

	// Update text
	CString a; 
	a.Format(_T("%d"), poseIdx);
	poseCurIdxText.SetWindowText(a);

	a.Format(_T("%d"), nodeIdxInPose);
	curIdxInPoseText.SetWindowText(a);

	a.Format(_T("%d"), pp.nodeGroupBoneCut.size());
	totalIdxInPoseText.SetWindowTextW(a);

	return true;
}

bool cutBoneGroupDlg::setselectIdxInPose(int nodeIdxInPose)
{
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;
	int boneIdxIdx = boneGoupListBox.GetCurSel();
	groupCut *curG = &groupBoneArray->at(boneIdxIdx);

	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int curIdx = _ttoi(stext);

	neighborPose curP = curG->boxPose.getPoseByIdx(curIdx);

	if (nodeIdxInPose < 0
		|| nodeIdxInPose >= curP.nodeGroupBoneCut.size())
	{
		return false;
	}

	CString a;
	a.Format(_T("%d"), curP.nodeGroupBoneCut.size());
	totalIdxInPoseText.SetWindowText(a);
	return true;
}


void cutBoneGroupDlg::OnBnClickedOk()
{
	groupCutMngr->updateAndChangeMode(idxChoosen);
	
	CDialogEx::OnOK();
}
