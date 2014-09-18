#pragma once
#include "DataTypes\vec.h"
#include "stdafx.h"
#include <memory>

class sphere
{
public:
	sphere();
	sphere(Vec3f centerPos, float r);
	virtual ~sphere();

	void drawSphere() const;

	// get set
	Vec3f &centerPos();
	float &radius();

protected:
	Vec3f m_center;
	float m_radius;
};

typedef std::shared_ptr<sphere> spherePtr;

// Energy sphere of skeleton
class skeSphere : public sphere
{
public:
	skeSphere();
	skeSphere(skeSphere * s);
	skeSphere(Vec3f centerPos, float r);
	~skeSphere();

	void draw() const;
	void scale(Vec3f basePt, float s);

private:
	float intersectMeshRatio; // In intersection with 
};
typedef std::shared_ptr<skeSphere> skeSpherePtr;

// Energy sphere of mesh
class meshSphere : public sphere
{
	meshSphere();
public:
	meshSphere(Vec3f c, float r);
	~meshSphere();

private:
	float weight; // Since the sphere may intersect each other, 
					// This weight is base on density
	float occupiedRatio; // Percentage occupied by bone sphere
	arrayInt voxelIdxs;
};
typedef std::shared_ptr<meshSphere> meshSpherePtr;