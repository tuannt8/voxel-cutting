#pragma once
#include "stdafx.h"
#include "DataTypes\define.h"
#include "skeleton.h"

class boneSphere
{
public:
	boneSphere();
	boneSphere(boneSphere *b);
	boneSphere(bone* b);
	~boneSphere();

public:
	// List of sphere
	arrayInt sphereIdxs;
	bone * originBone;

	// Coordinate information

};

typedef std::shared_ptr<boneSphere> boneSpherePtr;