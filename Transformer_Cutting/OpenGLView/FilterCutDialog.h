#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "cutSurfTreeMngr2.h"


// FilterCutDialog dialog
extern class myDocment;

class FilterCutDialog : public CDialogEx
{
	DECLARE_DYNAMIC(FilterCutDialog)

public:
	FilterCutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~FilterCutDialog();

	void initFromCutTree(cutSurfTreeMngr2 *cutMangr);

// Dialog Data
	enum { IDD = IDD_DIALOG_FILTER_CUT_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1(); // Update
	CComboBox joinComboBox;
	CComboBox JoinTypeComboBox;

	myDocment * doc;

public:
	std::vector<neighborPos> chosenPose;

public:
	afx_msg void OnCbnSelchangeJoin();
	afx_msg void OnCbnSelchangeJoinType();
	afx_msg void OnBnClickedClearFilter();

};
