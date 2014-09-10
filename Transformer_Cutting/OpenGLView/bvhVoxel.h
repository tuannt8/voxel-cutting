#pragma once
#include "stdafx.h"
#include "Modules\AABB.h"
#include "voxelObject.h"

class bvhVoxel: public AABBTree
{
public:
	bvhVoxel(void);
	virtual ~bvhVoxel(void);

	void drawVoxels(mirrorDraw* mirror, int mode = 0);
	void drawBoundingBoxSym(mirrorDraw *mirror);
	void drawOriginalBox();
	void drawLeaves();
	void drawLeavesSym(mirrorDraw *mirror);
	void drawVoxelBoneIdx(int idx);
	void drawVoxelIndex();

	// Modify
	void removeVoxel(AABBNode* node);
	void removeNodeWithVoxelIdx(int idx);
	void addVoxel(AABBNode* backUpNode);
	void addNodeWithVoxelIdx(int idx);

	// error With assumption of modification
	void error(float &volE, float &aspectE, float &volFillE, float &premeter);

	void errorWithAssumpRemoveNode(AABBNode* voxelAABB, float &volE, float &aspectE, float &volFillE, float &premeter);
	void errorWithAssumpRemoveNode(std::vector<AABBNode*> voxelAABBs, float &volE, float &aspectE, float &volFillE, float &premeter);

	void errorWithAssumpAddNode(AABBNode* voxelAABB, float &volE, float &aspectE, float &volFillE, float &premeter);
	void errorWithAssumpAddNode(std::vector<AABBNode*> voxelAABBs, float &volE, float &aspectE, float &volFillE, float &premeter);

	// Error if one node is remove
	Box boundingBoxIfRemoveNode(AABBNode* leaf);

	virtual void constructAABBTree();
	virtual void updateAABBTreeBottomUp();

	// Remove all node that does not included in set
	void removeNodeNotInList(hashVoxel* hashBigVox, int *mark, int boxIdx, arrayInt &idxsBig);
public:
	// 
	std::vector<AABBNode*> leaves;

	// Mesh info - read from file
	// Unchanged. Load in loadBoxMesh function
	CString boneName;
	int boneType;
	Vec3f leftDown;
	Vec3f rightUp;

	float expectedVolRatio;
	Vec3f xyzRatio; // Expected 1, y/x, z/x in world coord
	Vec3f boxSize; // Size of the bone that is translated to mesh coordinate
	float expectedPerimeterRatio;
	// For current config


	//
	bool bIsGroupBone;

public:
	// For constructing tree
	std::vector<voxelBox> *boxes;
	float voxelSize;
	hashVoxel *hashTable;
	octreeSolid *s_octree;
	arrayInt voxelIdxs; // voxel that belong to this mesh
	Vec3f curLeftDown, curRightUp;

public:
	Vec3f centerPoint();

public:
	void constructAABBTree(AABBNode* node);
	void updateAABBTreeBottomUp(AABBNode* node);

	void boundingBoxIfRemoveNode(AABBNode *node, Box &boxOut);
	void boundingBoxIfRemoveNodes(std::vector<AABBNode *> nodes, Box &boxOut);

	Box combineBox(Box box1, Box box2);
	void reconstructLeaves(AABBNode* node);
	void removeNodeNotInList(AABBNode* node, hashVoxel* hashBigVox, int *mark, int boxIdx, arrayInt &idxsBig);
	Box boundingBoxAssumeRemove(AABBNode* node);



	// We just need hash table
public:
	/* Use list of voxel index instead of bvh*/

	void getCurrentError(float &aspectE, float &volumeE);
	void getErrorWithAssumeAddVoxel(float &aspectE, float &volumeE, arrayInt addIdxs);
	void getErrorWithAssumeRemoveVoxel(float &aspectE, float &volumeE, arrayInt removeIdxs);
	// For new algorithm
	arrayFloat getCurrentError2();
	arrayFloat getErrorWithAssumeRemoveVoxel2(arrayInt addIdxs);
	arrayFloat getErrorWithAssumeAddVoxel2(arrayInt removeIdxs);

	Box getBoundingOfVoxels(arrayInt idxs);

	//Update
	void removeVoxel(arrayInt newVoxelIdx);
	void addVoxel(arrayInt newVoxelIdx);
	void update();

	void initOther();

public:
	arrayFloat getErrorSymWithidxs(const arrayInt & idxs);

	arrayFloat getErrorSym();
	arrayFloat getESymAssumeRemove(const arrayInt & temp);
	arrayFloat getESymAssumeAdd(const arrayInt & temp);

	float getExpectedVolumeRatioSym();
	Vec3f getSizeSym();
	void getInfoFromBone(bone* b);
};

