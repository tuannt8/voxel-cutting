// KEGIES.h : 
//
#pragma once

#ifndef __AFXWIN_H__
	#error "Error window"
#endif

#include "resource.h"

class CKEGIESApp : public CWinApp
{
public:
	CKEGIESApp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnToolConvertSTLtoObj();
	afx_msg void OnToolTest();
	afx_msg void OnToolStartSimulation();
	afx_msg void OnToolConvertstltocenterline();
};

extern CKEGIESApp theApp;