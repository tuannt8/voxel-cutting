#pragma once
#include "stdafx.h"
#include "DataTypes\define.h"
#include "skeleton.h"

enum orient
{
	R_Z_90		= 0x001,
	R_Y_90		= 0x010,
	R_X_90		= 0x100
};

#define VOLUME_REC(a) (a[0]*a[1]*a[2])

class boneAbstract
{
public:
	boneAbstract(bone* boneO, float voxelSize);
	boneAbstract(void);
	~boneAbstract(void);

	Vec3i getSizeByOrientation(int orientation);
	Vec3i getSizeByOrientation(BOOL xRot, BOOL yRot, BOOL zRot);

	// Need to find
//	Vec3i posi;  // 
//	int orientation;
	bone *original;

	// Unchanged
	Vec3i sizei;

	// temp
	float volumeRatiof; // ratio to total volume
	float aspectRatiof[2]; // ratio of two longer edge over shortest edge


	// For maintaining order after sorting
	int idxInArray;
};

