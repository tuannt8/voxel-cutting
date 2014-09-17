#include "stdafx.h"
#include "meshSphereObj.h"

using namespace energy;

meshSphereObj::meshSphereObj()
{
}


meshSphereObj::~meshSphereObj()
{
	for (auto s : m_meshSphereArray)
	{
		s.reset();
	}

	m_meshSphereArray.clear();
}

void meshSphereObj::initFromMesh(SurfaceObjPtr surObj, voxelObjectPtr voxel_Obj, bitSetMngrPtr bitSetSpace, int res)
{
	// We use combination of two uniform grids

	// 1. Decide the sphere radius
	Vec3f boundingSize = surObj->getBoundingBoxSize();
	float smallestEdge = std::min({ boundingSize[0], boundingSize[1], boundingSize[2] });
	float _radius = smallestEdge / res;

	// 2. Uniformly sphere
	// Layer 1
	bitSetObjectPtr mesh = voxel_Obj->meshBitSet;
	leftDownf = voxel_Obj->m_octree.m_root->leftDownTight;
	for (float _x = leftDownf[0] + _radius; _x < boundingSize[0]; _x += _radius)
	{
		for (float _y = leftDownf[1] + _radius; _y < boundingSize[1]; _y += _radius)
		{
			for (float _z = leftDownf[0] + _radius; _z < boundingSize[0]; _z += _radius)
			{
				Vec3f c(_x, _y, _z);

				// Check if this sphere is OK: 
				bitSetObjectPtr s = bitSetSpace->createSphere(c, _radius);
				bitSetObjectPtr intersect = mesh->intersectWith(s);

				if ((float)intersect->count()/s->count() > SPHERE_COVER_CONSTRUCT)
				{
					meshSpherePtr newSphere = meshSpherePtr(new meshSphere(c, _radius));
					m_meshSphereArray.push_back(newSphere);
				}
			}
		}
	}
}

void meshSphereObj::draw()
{
	for (auto s : m_meshSphereArray)
	{
		s->drawSphere();
	}
}
