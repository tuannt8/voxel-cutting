#include "stdafx.h"
#include "sphereEnergyMngr.h"


sphereEnergyMngr::sphereEnergyMngr()
{
}


sphereEnergyMngr::~sphereEnergyMngr()
{
	for (int i = 0; i < sphereArray.size(); i++)
	{
		sphereArray[i].reset();
	}
	sphereArray.clear();
}

void sphereEnergyMngr::initTest()
{

}

void sphereEnergyMngr::drawSphere()
{

}

void sphereEnergyMngr::drawFixConstraint()
{

}

void sphereEnergyMngr::drawNeighbor()
{

}
