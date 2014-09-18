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
	enum drawMode
	{
		DRAW_SPHERE = 0x0001,
		DRAW_FIX_CONSTRAINT = 0x0010,
		DRAW_NEIGHBOR = 0x0100,

		DRAW_ALL = 0x1111
	};

	energyMnager();
	~energyMnager();

	void initTest();
	void initFromSkeleton(skeletonPtr ske);

	void draw(drawMode mode);
	void drawSphere();
	void drawFixConstraint();
	void drawNeighbor();
	void drawBoneBoundingBox();

	std::shared_ptr<energyMnager> clone() const;
	void scale(float s);
public:
	std::vector<skeSpherePtr> sphereArray();
private:
	void initConstraints(std::vector<std::pair<int, int>> neighborInfo);
	void drawBoneBoundingBox(boneSpherePtr b, Vec3f color);

private:
	std::vector<skeSpherePtr> m_sphereArray;
	arrayVec2i fixConstraints;
	arrayVec2i neighborCts;

	std::vector<boneSpherePtr> m_boneArray;
};

typedef std::shared_ptr<energyMnager> energyMngerPtr;
