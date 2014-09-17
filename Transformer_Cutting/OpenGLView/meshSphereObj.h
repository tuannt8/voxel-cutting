#pragma once
#include "stdafx.h"
#include "Graphics\Surfaceobj.h"
#include "sphere.h"
#include "voxelObject.h"
#include "bitSetMangr.h"

#define SPHERE_COVER_CONSTRUCT 0.3

class meshSphereObj
{
public:
	meshSphereObj();
	~meshSphereObj();

	void initFromMesh(SurfaceObjPtr surObj, voxelObjectPtr voxel_Obj, energy::bitSetMngrPtr bitSetSpace, int res);

	void draw();

private:
	std::vector<meshSpherePtr> m_meshSphereArray;
	Vec3f leftDownf;
};

typedef std::shared_ptr<meshSphereObj> meshSphereObjPtr;