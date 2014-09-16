#pragma once
#include "DataTypes\vec.h"
#include "stdafx.h"
#include <memory>

class sphere
{
public:
	sphere();
	~sphere();

private:
	Vec3f center;
	float radius;
};

typedef std::shared_ptr<sphere> spherePtr;

class skeSphere : public sphere
{
public:
	skeSphere();
	~skeSphere();

private:

};

typedef std::shared_ptr<skeSphere> skeSpherePtr;