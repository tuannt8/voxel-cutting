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

	if (!mode[3] && voxel_Obj)
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
	if (mode[5] && curEnergyObj)
	{
		curEnergyObj->draw(energyMnager::DRAW_ALL);
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

	boneScale = 0.5;
	cout << "Clone bone with scale ratio: " << boneScale << endl;
	curEnergyObj = energyObjsOrigin->clone();
	curEnergyObj->scale(boneScale);
	// Volume match

	// 3. Load surface object
	char * surPath = "../../Data/Fighter/fighter.stl";
	cout << "Load surface object: " << surPath << endl;

	surObj = SurfaceObjPtr(new SurfaceObj);
	surObj->readObjDataSTL(surPath);
	cout << "Construct AABB tree of surface" << endl;
	surObj->constructAABBTree();

	// 4. Construct voxel
	float scale = 1.0;	int voxelRes = 5;
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

	displayOtherInfo();
}

bool energyDoc::receiveCmd(std::vector<std::string> args)
{
	return false;
}

void energyDoc::displayOtherInfo()
{
	cout << "-----------------------------" << endl;
	cout << "Information:" << endl;

	// Volume match
	float meshVol = voxel_Obj->volumef();
	float skeVol = curSkeleton->getVolume()*pow(boneScale, 3);
	float ratioVol = meshVol / skeVol;
	cout << "Volume ratio mesh / bone = " << ratioVol << endl;
}

void energyDoc::compute()
{
	// 1. Force between mesh and skeleton
	// 1.a. Update mesh - bone intersection
	voxel_Obj->updateSphereOccupy(curEnergyObj);
}
