#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "DataTypes\vec.h"
#include "manipulateVoxel.h"


// movePlacedBoxDlg dialog

class movePlacedBoxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(movePlacedBoxDlg)

public:
	movePlacedBoxDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~movePlacedBoxDlg();
	int getCurBoneIdx();
	void initFromVoxelProcess(manipulateVoxelPtr voxelProcess);

// Dialog Data
	enum { IDD = IDD_DIALOG_MOVE_PLACED_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	manipulateVoxelPtr s_voxelProcess;
	arrayVec3f centerBox;
	// Text edit box. Scale bone ratio. For all bone.
	CEdit m_scaleRatioBox;
	// List box of bones
	CComboBox m_boneListBox;
	CEdit m_speedTextBox;
	afx_msg void Move_X_BACK();
	afx_msg void MOVE_X_FORWARD();
	afx_msg void MOVE_Y_BACK();
	afx_msg void MOVE_Y_FORWARD();
	afx_msg void MOVE_Z_BACK();
	afx_msg void MOVE_Z_FORWARD();

private:
	void move(Vec3f offSet);

public:
	afx_msg void Update_scale_bone();
};

typedef std::shared_ptr<movePlacedBoxDlg> movePlacedBoxDlgPtr;