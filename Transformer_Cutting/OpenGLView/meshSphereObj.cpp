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
	Vec3f ldf = surObj->getBVH()->root()->LeftDown - Vec3f(_radius, _radius, _radius);
	Vec3f ruf = surObj->getBVH()->root()->RightUp + Vec3f(_radius, _radius, _radius);
	for (float _x = ldf[0]; _x < ruf[0]; _x += _radius)
	{
		for (float _y = ldf[1]; _y < ruf[1]; _y += _radius)
		{
			for (float _z = ldf[2]; _z < ruf[2]; _z += _radius)
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
