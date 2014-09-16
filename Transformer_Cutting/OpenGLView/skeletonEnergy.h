#pragma once
#include "sphere.h"
#include "DataTypes\define.h"

#define vicousCoff 1

class skeletonEnergy
{
public:
	skeletonEnergy();
	~skeletonEnergy();

	void initTest();

	void drawSphere();
	void drawFixConstraint();
	void drawNeighbor();

private:
	std::vector<skeSpherePtr> sphereArray;
	arrayVec2i fixConstraints;
	arrayVec2i neighbor;
};

