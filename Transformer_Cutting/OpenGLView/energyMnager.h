#pragma once
#include "sphere.h"
#include "DataTypes\define.h"
#include "skeleton.h"
#include "boneSphere.h"

#define vicousCoff 1
/* To divide the edge of the bone to a integer 
	the residual > thres --> we add one to the result
*/
#define thres_divide_bone 0.3 

class energyMnager
{
public:
	energyMnager();
	~energyMnager();

	void initTest();
	void initFromSkeleton(skeletonPtr ske);

	void drawSphere();
	void drawFixConstraint();
	void drawNeighbor();

	std::shared_ptr<energyMnager> clone() const;

private:
	void initConstraints(std::vector<std::pair<int, int>> neighborInfo);

private:
	std::vector<skeSpherePtr> m_sphereArray;
	arrayVec2i fixConstraints;
	arrayVec2i neighborCts;

	std::vector<boneSpherePtr> m_boneArray;
};

typedef std::shared_ptr<energyMnager> energyMngerPtr;
