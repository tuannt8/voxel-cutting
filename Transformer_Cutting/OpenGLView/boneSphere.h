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

	arrayInt sphereIdxs;
	bone * originBone;
};

typedef std::shared_ptr<boneSphere> boneSpherePtr;