#pragma once
#include "stdafx.h"
#include "cutSurfTreeMngr2.h"

class configTreeMng
{
public:
	configTreeMng(void);
	~configTreeMng(void);

	// Process
	void loadDataFromFile();// Bypass if data is ready

	//cutSurfTreeMngr2 m_data;
};

