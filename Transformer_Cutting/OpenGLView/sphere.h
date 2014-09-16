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
private:
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

private:

};
typedef std::shared_ptr<skeSphere> skeSpherePtr;

// Energy sphere of mesh
class meshSphere : public sphere
{
public:
	meshSphere();
	~meshSphere();

private:

};
typedef std::shared_ptr<meshSphere> meshSpherePtr;