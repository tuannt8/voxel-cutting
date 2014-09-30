#pragma once

#include "stdafx.h"
#include "DataTypes\vec.h"
#include <map>
// #include "skeleton.h"
// #include "boneAbstract.h"
#include "octreeSolid.h"
#include "mirrorDraw.h"
#include "voxelSplitObj.h"
#include "bitSetObject.h"
#include "energyMnager.h"

class voxelBox // For 2 pixel voxel
{
public:
	voxelBox();
	voxelBox(Vec3f ld, Vec3f ru);
	~voxelBox();

	void draw(int mode); // 0: line; 1: face
public:
	// Voxel info
	Vec3f leftDown;
	Vec3f rightUp;
	Vec3f center;

	Vec3i xyzIndex; // Discretized index

	// Belong to what bone
	int boneIndex;
	int state;
};

class hashVoxel
{
public:
	hashVoxel();
	~hashVoxel();

	Vec3i leftDowni(){ return Vec3i(1, 1, 1); };
	Vec3i rightUpi(){ return NumXYZ - Vec3i(1, 1, 1); };

	int getBoxIndexFromCoord(Vec3f coordf);
	int getBoxIndexFromVoxelCoord(Vec3i coordi);
	int getBoxIndexFromCoord(Vec3f leftDown, Vec3f rightUp);
	Vec3i getVoxelCoord(Vec3f coordf);
	int getSymmetricBox(int idx);//get index of mirror box of idx
	std::vector<int> getNeighbor(int idx);
	Vec3f IJK2XYZ(Vec3i voxelCi);
	Vec3f IJK2XYZLower(Vec3i coordi);
	Vec3f IJK2XYZUpper(Vec3i coordi);
public:
	std::vector<voxelBox>* boxes;
	std::vector<int> voxelHash; // hash boxes to voxel space
	Vec3f leftDown;
	Vec3f rightUp;
	float voxelSize;
	Vec3i NumXYZ;

	// Neighbor
	std::vector<arrayInt> neighborOfBox;
};

class voxelObject
{
public:
	voxelObject();
	~voxelObject();

	bool init(SurfaceObj *obj, int res); 
	bool init(SurfaceObj *obj, int res, float scale);
	bool init(voxelObject *highRes, int downSampleRate);

	void drawBitSet(voxelSplitObj* vBitSet);
	void drawVoxelBitDecomposed();
	void drawBitSetBoundingBox();

	void drawVoxel(int mode = 0);
	void drawVoxelLeaf(int mode = 0); // 0: edge; 1: Solid

	Vec3f floorV(Vec3f v, float d);
	float volumef() const;
	void updateSphereOccupy(energyMngerPtr curEnergyObj);
private:
	void constructVolxeHash();
	void constructVolxeHash(float scale);
	void constructNeighbor();
	void decomposeConvexes();
	void constructBitSetMesh();


public: // Private member variable	
	octreeSolid m_octree;
	std::vector<voxelBox> m_boxes;
	std::vector<arrayInt> m_allBoxAroundBox; // All neighbor of voxel
	std::vector<arrayInt> m_boxShareFaceWithBox; // Neighbor that share face with voxel

	voxelSplitObj m_voxelBitSet; // Bit set for splitting operation
								// Include set of convex group voxel

	energy::bitSetObjectPtr meshBitSet;

	hashVoxel m_hashTable; // Hash table

	int m_res;
	float m_voxelSizef;
	Vec3f m_centerf;

private: // Shared resource
	SurfaceObj *s_surObj;

}; /*Voxel Object*/

typedef std::shared_ptr<voxelObject> voxelObjectPtr;
