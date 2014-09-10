// coordAsignDlg.cpp : implementation file
//

#include "stdafx.h"
#include "coordAsignDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "KEGIESDoc.h"
#include "detailSwapManager.h"


// coordAsignDlg dialog

IMPLEMENT_DYNAMIC(coordAsignDlg, CDialog)

coordAsignDlg::coordAsignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(coordAsignDlg::IDD, pParent)
{

}

coordAsignDlg::~coordAsignDlg()
{
}

void coordAsignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BONE_NAME, boneComboBox);
	DDX_Control(pDX, IDC_COMBO_MAP_X, comboMapX);
	DDX_Control(pDX, IDC_COMBO_MAP_Y, comboMapY);
	DDX_Control(pDX, IDC_STATIC_MAP_Z, mapZ);
}


BEGIN_MESSAGE_MAP(coordAsignDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_COORD, &coordAsignDlg::OnBnClickedButtonUpdateCoord)
	ON_BN_CLICKED(IDC_BUTTON_FINISH, &coordAsignDlg::OnBnClickedButtonFinish)
	ON_CBN_SELCHANGE(IDC_COMBO_BONE_NAME, &coordAsignDlg::OnCbnSelchangeComboBoneName)
	ON_CBN_SELCHANGE(IDC_COMBO_MAP_X, &coordAsignDlg::OnCbnSelchangeComboMapX)
	ON_CBN_SELCHANGE(IDC_COMBO_MAP_Y, &coordAsignDlg::OnCbnSelchangeComboMapY)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_COORD, &coordAsignDlg::OnBnClickedButtonSaveCoord)
	ON_BN_CLICKED(IDC_BUTTON2_LOAD_COORD, &coordAsignDlg::OnBnClickedButton2LoadCoord)
END_MESSAGE_MAP()

void coordAsignDlg::init(detailSwapManager* detailSwap)
{
	s_detailSwap = detailSwap;
	std::vector<bvhVoxel*> meshBox = detailSwap->meshBox;

	coords.resize(meshBox.size());
	std::fill(coords.begin(), coords.end(), Vec3i(-1,-1,-1));

	//
	for (int i = 0; i < meshBox.size(); i++)
	{
		boneComboBox.AddString(meshBox[i]->boneName);
	}

	comboMapX.AddString(_T("X"));
	comboMapX.AddString(_T("Y"));
	comboMapX.AddString(_T("Z"));

	comboMapY.AddString(_T("X"));
	comboMapY.AddString(_T("Y"));
	comboMapY.AddString(_T("Z"));

	boneComboBox.SetCurSel(0);
	setCurBoneSlection(0);
}

void coordAsignDlg::init(std::vector<bone*> *boneFullArray, std::vector<bvhVoxel> *meshBoxFull)
{
	s_boneFullArray = boneFullArray;
	s_meshBoxFull = meshBoxFull;

	coords.resize(s_meshBoxFull->size());
	std::fill(coords.begin(), coords.end(), Vec3i(-1, -1, -1));

	//
	for (int i = 0; i < s_meshBoxFull->size(); i++)
	{
		boneComboBox.AddString((*s_meshBoxFull)[i].boneName);
	}

	comboMapX.AddString(_T("X"));
	comboMapX.AddString(_T("Y"));
	comboMapX.AddString(_T("Z"));

	comboMapY.AddString(_T("X"));
	comboMapY.AddString(_T("Y"));
	comboMapY.AddString(_T("Z"));

	boneComboBox.SetCurSel(0);
	setCurBoneSlection(0);
}


// coordAsignDlg message handlers


void coordAsignDlg::OnBnClickedButtonUpdateCoord()
{
	int curIdx = getCurBoneIdx();
	Vec3i cc = coords[curIdx];

	if (cc[0] == cc[1])
	{
		AfxMessageBox(_T("X and Y is same!"));
		return;
	}
	
	for (int i = 0; i < 3; i++)
	{
		if (i != cc[0] && i!= cc[1])
		{
			cc[2] = i;
			break;
		}
	}

	coords[curIdx] = cc;

	updateComboBoxText();
	setCurBoneSlection(curIdx);
}


void coordAsignDlg::OnBnClickedButtonFinish()
{
	//Check all coord
	for (int i = 0; i < coords.size(); i++)
	{
		if (coords[i][2] == -1)
		{
			AfxMessageBox(_T("Not all coords is set"));
			return;
		}
	}

	// Change state
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CKEGIESDoc *doc = (CKEGIESDoc *) pFrame->GetActiveDocument();

	doc->detailSwap.coords = coords;
	doc->detailSwap.computeAspectRatio();
	doc->m_mode = MODE_SWAP_VOXEL;

	OnOK();
}

void coordAsignDlg::setCurBoneSlection(int boneIdx)
{
	Vec3i cc = coords[boneIdx];

	LPCTSTR xyzName[3] = {_T("X"), _T("Y"), _T("Z")}; 
	if (cc[2] != -1)
	{
		comboMapX.SetCurSel(cc[0]);
		comboMapY.SetCurSel(cc[1]);
		mapZ.SetWindowText(xyzName[cc[2]]);
	}
	else
	{
		comboMapX.SetCurSel(0);
		comboMapY.SetCurSel(1);
		mapZ.SetWindowText(_T("Not set"));

		coords[boneIdx][0] = 0;
		coords[boneIdx][1] = 1;
	}
}


void coordAsignDlg::OnCbnSelchangeComboBoneName()
{
	setCurBoneSlection(boneComboBox.GetCurSel());
}


void coordAsignDlg::OnCbnSelchangeComboMapX()
{
	int idx = getCurBoneIdx();
	coords[idx][0] = comboMapX.GetCurSel();
}


void coordAsignDlg::OnCbnSelchangeComboMapY()
{
	int idx = getCurBoneIdx();
	coords[idx][1] = comboMapY.GetCurSel();
}

int coordAsignDlg::getCurBoneIdx()
{
	return boneComboBox.GetCurSel();
}

void coordAsignDlg::updateComboBoxText()
{
	int idx = getCurBoneIdx();

	boneComboBox.ResetContent();

	for (int i = 0; i < s_meshBoxFull->size(); i++)
	{
		CString curName = (*s_meshBoxFull)[i].boneName;
		if (coords[i][2] != -1)
		{
			curName += " - set";
		}
		boneComboBox.AddString(curName);
	}

	boneComboBox.SetCurSel(idx);
}


void coordAsignDlg::OnBnClickedButtonSaveCoord()
{
	// TODO: Add your control notification handler code here
	//Check all coord
	for (int i = 0; i < coords.size(); i++)
	{
		if (coords[i][2] == -1)
		{
			AfxMessageBox(_T("Not all coords is set"));
			return;
		}
	}

	CFileDialog dlg(FALSE);
	dlg.m_ofn.lpstrFilter=_T("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0");
	dlg.m_ofn.lpstrTitle=_T("Save coord as");

	CString filename;

	if(dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName(); // return full path and filename
		//add .txt if missing
		::PathRemoveExtension(filename.GetBuffer(_MAX_PATH));
		::PathAddExtension(filename.GetBuffer(_MAX_PATH), _T(".txt"));
		filename.ReleaseBuffer(-1);

		FILE *f = fopen((LPCSTR)CStringA(filename), "w");
		ASSERT(f);
		fprintf(f, "%d\n", coords.size());
		for (int i = 0; i < coords.size(); i++)
		{
			fprintf(f, "%d %d %d\n", coords[i][0], coords[i][1], coords[i][2]);
		}
		fclose(f);
	}
}


void coordAsignDlg::OnBnClickedButton2LoadCoord()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter= _T("TXT Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0");
	dlg.m_ofn.lpstrTitle= _T("Open Coordinate file");
	CString filename;

	if(dlg.DoModal() == IDOK)
	{
		filename = dlg.GetPathName(); // return full path and filename
		FILE *f = fopen((LPCSTR)CStringA(filename), "r");
		ASSERT(f);

		int nb;
		fscanf(f, "%d\n", &nb);
		if (nb != coords.size())
		{
			AfxMessageBox(_T("Number of coordinate is different"));
			fclose(f);
			return;
		}

		for (int i = 0; i < nb; i++)
		{
			fscanf(f, "%d %d %d\n", &coords[i][0], &coords[i][1], &coords[i][2]);
		}
	}

	updateComboBoxText();
	boneComboBox.SetCurSel(0);
	setCurBoneSlection(0);
}
