#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "skeleton.h"
#include "meshObject.h"


class boneModifyingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(boneModifyingDlg)

public:
	void init(std::vector<meshObjectPtr> meshArray);
	std::vector<meshObjectPtr> s_meshArray;

	arrayVec3f arrayBoneTrans;
public:
	boneModifyingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~boneModifyingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox boneList;
	CEdit moveSpeed;
	CEdit boneScale;

	afx_msg void ReduceX();
	afx_msg void IncreaseX();
	afx_msg void ReduceY();
	afx_msg void increaseY();
	afx_msg void reduceZ();
	afx_msg void increase_Z();

private:
	void changeBoneSlection(int idx);
	float getMoveStep();
public:


};

