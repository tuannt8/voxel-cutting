// movePlacedBoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "movePlacedBoxDlg.h"
#include "afxdialogex.h"
#include "Resource.h"


// movePlacedBoxDlg dialog

IMPLEMENT_DYNAMIC(movePlacedBoxDlg, CDialogEx)

movePlacedBoxDlg::movePlacedBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(movePlacedBoxDlg::IDD, pParent)
{

}

movePlacedBoxDlg::~movePlacedBoxDlg()
{
}

void movePlacedBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_SCALE, m_scaleRatioBox);
	DDX_Control(pDX, IDC_COMBO_BOX_PLACE_BONE_LIST, m_boneListBox);
	DDX_Control(pDX, IDC_EDIT_SPEED, m_speedTextBox);
}


BEGIN_MESSAGE_MAP(movePlacedBoxDlg, CDialogEx)
	ON_BN_CLICKED(IDC_MOVE_X_BACK, &movePlacedBoxDlg::Move_X_BACK)
	ON_BN_CLICKED(IDC_MOVE_X_F, &movePlacedBoxDlg::MOVE_X_FORWARD)
	ON_BN_CLICKED(IDC_BUTTON3_Y_B, &movePlacedBoxDlg::MOVE_Y_BACK)
	ON_BN_CLICKED(IDC_BUTTON4_Y_F, &movePlacedBoxDlg::MOVE_Y_FORWARD)
	ON_BN_CLICKED(IDC_BUTTON5_Z_B, &movePlacedBoxDlg::MOVE_Z_BACK)
	ON_BN_CLICKED(IDC_BUTTON6_Z_F, &movePlacedBoxDlg::MOVE_Z_FORWARD)
	ON_BN_CLICKED(IDC_BUTTON1_UPDATE, &movePlacedBoxDlg::Update_scale_bone)
END_MESSAGE_MAP()


// movePlacedBoxDlg message handlers


void movePlacedBoxDlg::Move_X_BACK()
{
	move(Vec3f(-1, 0, 0));
}


void movePlacedBoxDlg::MOVE_X_FORWARD()
{
	move(Vec3f(1, 0, 0));
}


void movePlacedBoxDlg::MOVE_Y_BACK()
{
	move(Vec3f(0, -1, 0));
}


void movePlacedBoxDlg::MOVE_Y_FORWARD()
{
	move(Vec3f(0, 1, 0));
}


void movePlacedBoxDlg::MOVE_Z_BACK()
{
	move(Vec3f(0, 0, -1));
}


void movePlacedBoxDlg::MOVE_Z_FORWARD()
{
	move(Vec3f(0, 0, 1));
}

void movePlacedBoxDlg::move(Vec3f offSet)
{
	try{
		int boneIdx = m_boneListBox.GetCurSel();
		CString text; m_speedTextBox.GetWindowTextW(text);
		float dis = _ttof(text);
		ASSERT(boneIdx < centerBox.size());
		centerBox[boneIdx] += offSet*dis;
		Update_scale_bone();
	}
	catch (...)
	{
		AfxMessageBox(_T("Check input text in distance step"));
	}
}

void movePlacedBoxDlg::initFromVoxelProcess(manipulateVoxelPtr voxelProcess)
{
	s_voxelProcess = voxelProcess;
	
	// Scale
	m_scaleRatioBox.SetWindowTextW(_T("100"));

	// Init bone list
	std::vector<bone*> boneArray = voxelProcess->boneArray;
	for (auto b : boneArray)
	{
		m_boneListBox.AddString(b->m_name);
	}
	m_boneListBox.SetCurSel(0);

	// Speed: 10% of min size
	float speed = s_voxelProcess->s_voxelObj->m_voxelSizef / 2;
	CString text;
	text.Format(_T("%f"), speed);
	m_speedTextBox.SetWindowTextW(text);

	// Center box
	std::vector<voxelList*> meshBoxes = voxelProcess->meshBox;
	for (auto m : meshBoxes)
	{
		centerBox.push_back(m->centerPoint());
	}
	Update_scale_bone();
}


void movePlacedBoxDlg::Update_scale_bone()
{
	int percent;
	
	try{
		CString text;
		m_scaleRatioBox.GetWindowTextW(text);
		percent = _ttoi(text);
		s_voxelProcess->updateBox(percent, centerBox);
	}
	catch (...)
	{
		AfxMessageBox(_T("Error input scale ratio. Should be integer"));
	}

}

int movePlacedBoxDlg::getCurBoneIdx()
{
	return m_boneListBox.GetCurSel();
}
