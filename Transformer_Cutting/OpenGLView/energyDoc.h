#pragma once
#include "skeleton.h"
#include "energyMnager.h"
#include "Voxel.h"
#include "voxelObject.h"
#include "meshSphereObj.h"
#include "bitSetMangr.h"
#include "dynamicManager.h"
#include "log.h"

using namespace energy;

class energyDoc : public command::cmdBase
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

	void compute();

	virtual bool receiveCmd(std::vector<std::string> args);
private:
	void displayOtherInfo();
private:
	skeletonPtr curSkeleton; // Skeleton
	skeletonPtr originSkeleton;

	SurfaceObjPtr surObj;	// Surface
	voxelObjectPtr voxel_Obj; // Voxel; Donot use the hash table
	
	bitSetMngrPtr bitSetSpace; // Manage bit set operation

	meshSphereObjPtr objEnergy; // Energy sphere in mesh

	energyMngerPtr energyObjsOrigin; // Sphere energy in bone
	energyMngerPtr curEnergyObj; // Sphere energy for modification
	float boneScale;

	dynamicManagerPtr dynamicM;
};

