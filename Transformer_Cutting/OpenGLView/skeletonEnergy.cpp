#include "stdafx.h"
#include "skeletonEnergy.h"


skeletonEnergy::skeletonEnergy()
{
}


skeletonEnergy::~skeletonEnergy()
{
	for (int i = 0; i < sphereArray.size(); i++)
	{
		sphereArray[i].reset();
	}
	sphereArray.clear();
}

void skeletonEnergy::initTest()
{

}

void skeletonEnergy::drawSphere()
{

}

void skeletonEnergy::drawFixConstraint()
{

}

void skeletonEnergy::drawNeighbor()
{

}
