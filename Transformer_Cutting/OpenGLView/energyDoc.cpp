#include "stdafx.h"
#include "energyDoc.h"

using namespace std;
energyDoc::energyDoc()
{
	init();
}


energyDoc::~energyDoc()
{
}

void energyDoc::updateRealtime()
{

}

void energyDoc::draw(BOOL mode[10])
{

}

void energyDoc::draw2(bool mode[10])
{
	if (mode[1])
	{
		if (curSkeleton)
			curSkeleton->draw();
	}
}

void energyDoc::receiveKey(char c)
{

}

void energyDoc::updateIdx(int yIdx, int zIdx)
{

}

void energyDoc::init()
{
	// Init skeleton
	char * skePath = "../../Data/skeleton.xml";
	cout << "load skeleton: " << skePath << endl;
	curSkeleton = skeletonPtr(new skeleton);
	curSkeleton->loadFromFile(skePath);
	originSkeleton = skeletonPtr(new skeleton);
	originSkeleton->loadFromFile(skePath);
}
