// NodeGenerate.h : main header file for the NodeGenerate DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


class CNodeGenerateApp : public CWinApp
{
public:
	CNodeGenerateApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
