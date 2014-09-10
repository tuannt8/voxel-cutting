#pragma once
#include "afxwin.h"


// SimulationDlg dialog

class SimulationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SimulationDlg)
private:
	CStatic m_objPathText;
	CStatic m_simDataPath;

public:
	SimulationDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SimulationDlg();

// Dialog Data
	enum { IDD = IDD_SIMULATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	void updateSetting();
	afx_msg void OnBnClickedButtonObjPathChange();
	afx_msg void OnBnClickedButtonSimPathChange();
};
