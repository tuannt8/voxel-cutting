#pragma once
#include "afxext.h"
#include "stdafx.h"
#include "resource.h"

#define WM_TEST WM_APP+30

class Dialog
{
public:
	Dialog(void);
	~Dialog(void);
};

class dialogVoxel : public CDialog
{
public:
	dialogVoxel();
	enum { IDD = IDD_DIALOG_TEST_VOXEL };

	BOOL Create(UINT nID, CWnd * pWnd)
	{ return CDialog::Create(nID,pWnd);}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	void* view;
	afx_msg void OnEnChangeEdit1();
};

