#pragma once
#include "stdafx.h"

class dynamicManager
{
public:
	dynamicManager();
	~dynamicManager();

	void compute();
};

typedef std::shared_ptr<dynamicManager> dynamicManagerPtr;