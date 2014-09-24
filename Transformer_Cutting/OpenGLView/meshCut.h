#pragma once
#include "stdafx.h"
#include "DataTypes\define.h"
#include "voxelObject.h"

extern class meshCut;
typedef std::shared_ptr<meshCut> meshCutPtr;

class meshCut
{
public:
	meshCut();
	~meshCut();
	meshCut(arrayInt idxs);
	meshCutPtr clone();
	void updateOther(voxelObjectPtr vObj);
public:
	arrayInt voxelIdx;
	Vec3f leftDownf;
	Vec3f rightUpf;
};


