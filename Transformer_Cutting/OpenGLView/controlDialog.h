#pragma once
#include "stdafx.h"

typedef enum
{
	STATE_1_LOAD_FILE,
	STATE_1_GEN_VOXEL,
	STATE_USER_DEFINE_ERROR_WEIGHT,
	STATE_WRAP_VOXEL,
}APP_STATE;

class controlDialog : public CDialog
{
public:
	controlDialog(void);
	~controlDialog(void);

	void changeToState(APP_STATE s);
private:
	void disableAllControl();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow( );

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClicked1LoadMesh();
};

