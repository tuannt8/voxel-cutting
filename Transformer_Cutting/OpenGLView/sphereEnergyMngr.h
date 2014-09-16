#pragma once
#include "sphere.h"
#include "DataTypes\define.h"
#include "skeleton.h"

#define vicousCoff 1

class sphereEnergyMngr
{
public:
	sphereEnergyMngr();
	~sphereEnergyMngr();

	void initTest();
	void initFromSkeleton(skeletonPtr ske);

	void drawSphere();
	void drawFixConstraint();
	void drawNeighbor();

private:
	std::vector<skeSpherePtr> sphereArray;
	arrayVec2i fixConstraints;
	arrayVec2i neighbor;
};

typedef std::shared_ptr<sphereEnergyMngr> engergyMngerPtr;
