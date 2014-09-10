#pragma once
#include "stdafx.h"
#include "skeleton.h"

class voxelList
{
public:
	voxelList();
	~voxelList();

	// draw
	void drawWire();
	void drawFace();

	// Compute the volume ration
	void init();
	void updateIdxs(arrayInt idxs);

	arrayFloat getErrorAssumeVoxelList(arrayInt idxs);

public: // Data manipulation
	Vec3f centerPoint();
	void updateBoundingBox();

public: // variables
	// Voxel list
	arrayInt m_voxelIdxs;
	
	Vec3f m_curLeftDown3F;
	Vec3f m_curRightUp3F;

public:
	// Unchanged information
	Vec3f m_BoxSize3F;
	float m_volumeRatio;

	bone* s_corressBondP;
	voxelObject *s_voxelObj;

public:
	// Temp variable, for debugging


private: // bump function
	Box getBoundingOfVoxels(arrayInt idxs);
	Box combineBox(Box box1, Box box2);
	float getExpectedVolumeRatioSym();
	Vec3f getSizeSym();
};

