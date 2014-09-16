#pragma once
#include "skeleton.h"
#include "sphereEnergyMngr.h"
class energyDoc
{
public:
	energyDoc();
	~energyDoc();

	void draw(BOOL mode[10]);
	void draw2(bool mode[10]);

	void init();

	void receiveKey(char c);
	void updateIdx(int yIdx, int zIdx);
	void updateRealtime();

private:
	skeletonPtr curSkeleton;
	skeletonPtr originSkeleton;

	engergyMngerPtr energyObjs;
};

