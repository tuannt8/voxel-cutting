#pragma once
#include "skeleton.h"
#include "energyMnager.h"
#include "Voxel.h"
#include "voxelObject.h"
#include "meshSphereObj.h"
#include "bitSetMangr.h"

using namespace energy;

class energyDoc
{
public:
	energyDoc();
	~energyDoc();

	void draw(BOOL mode[10]);
	void draw2(bool mode[10]);

	void init();

	void receiveKey(char c);
	void updateIdx(int yIdx, int zIdx);
	void updateRealtime();

private:
	skeletonPtr curSkeleton; // Skeleton
	skeletonPtr originSkeleton;

	SurfaceObjPtr surObj;	// Surface
	voxelObjectPtr voxel_Obj; // Voxel; Donot use the hash table
	
	bitSetMngrPtr bitSetSpace; // Manage bit set operation

	meshSphereObjPtr objEnergy; // Energy sphere in mesh

	energyMngerPtr energyObjsOrigin; // Sphere energy
	energyMngerPtr curEnergyObj; // Sphere energy for modification
};

