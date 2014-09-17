#include "stdafx.h"
#include "energyDoc.h"

using namespace std;
energyDoc::energyDoc()
{
	init();
}


energyDoc::~energyDoc()
{
}

void energyDoc::updateRealtime()
{

}

void energyDoc::draw(BOOL mode[10])
{
	if (bitSetSpace)
	{
		glColor3f(1, 0, 0);
		bitSetSpace->drawBoundingBox();
	}
	if (mode[1] && surObj)
	{
		glColor3f(0.8, 0.8, 0.8);
		surObj->drawObject();
	}

	if (!mode[2] && voxel_Obj)
	{
		voxel_Obj->drawVoxel();
		glColor3f(0.6, 0.6, 0.6);
		voxel_Obj->drawVoxelLeaf(1);
	}

	if (mode[3] && voxel_Obj)
	{
		glColor3f(0.7, 0.7, 0.7);
		bitSetSpace->drawSolidBit(voxel_Obj->meshBitSet);
		glColor3f(0, 0, 1);
		bitSetSpace->drawWireBit(voxel_Obj->meshBitSet);
	}
	if (mode[4] && objEnergy)
	{
		glColor3f(0, 1, 0);
		voxel_Obj->m_octree.drawBoundingBox();

		objEnergy->draw();
	}
}

void energyDoc::draw2(bool mode[10])
{
	if (mode[1] && curSkeleton) // Skeleton
	{
		curSkeleton->draw();
	}

	if (mode[2] && energyObjsOrigin) // Sphere energy
	{
		energyObjsOrigin->drawSphere();
	}

	if (mode[3] && energyObjsOrigin) // Connection between sphere in bone
	{
		glColor3f(0, 0, 1);
		energyObjsOrigin->drawFixConstraint();
	}

	if (mode[4] && energyObjsOrigin) // Neighbor
	{
		glColor3f(1, 1, 0);
		energyObjsOrigin->drawNeighbor();
	}
}

void energyDoc::receiveKey(char c)
{

}

void energyDoc::updateIdx(int yIdx, int zIdx)
{

}

void energyDoc::init()
{
	// 1. Init skeleton
	char * skePath = "../../Data/skeleton.xml";
	cout << "load skeleton: " << skePath << endl;

	curSkeleton = skeletonPtr(new skeleton);
	curSkeleton->loadFromFile(skePath);
	originSkeleton = skeletonPtr(new skeleton);
	originSkeleton->loadFromFile(skePath);

	// 2. Init energy ball from skeleton
	cout << "Construct sphere energy from skeleton" << endl;

	energyObjsOrigin = energyMngerPtr(new energyMnager);
	energyObjsOrigin->initFromSkeleton(curSkeleton);

	// 3. Load surface object
	char * surPath = "../../Data/Fighter/fighter.stl";
	cout << "Load surface object: " << surPath << endl;

	surObj = SurfaceObjPtr(new SurfaceObj);
	surObj->readObjDataSTL(surPath);
	cout << "Construct AABB tree of surface" << endl;
	surObj->constructAABBTree();

	// 4. Construct voxel
	float scale = 2.0;	int voxelRes = 5;
	cout << "Construct voxel hashing (" << scale << " scale; " << voxelRes << " resolution" << endl;
	voxel_Obj = voxelObjectPtr(new voxelObject);
	voxel_Obj->init(surObj.get(), voxelRes, scale);

	// 4.a. Bit set space
	cout << "Construct bitset space." << endl;
	bitSetSpace = bitSetMngrPtr(new bitSetMangr);
	bitSetSpace->init(voxel_Obj);

	// 5. Sphere energy of mesh object
	int sphereRes = 3;
	cout << "Construct mesh sphere object (Res: " << sphereRes << ")" << endl;
	objEnergy = meshSphereObjPtr(new meshSphereObj);
	objEnergy->initFromMesh(surObj, voxel_Obj, bitSetSpace, sphereRes);
}
