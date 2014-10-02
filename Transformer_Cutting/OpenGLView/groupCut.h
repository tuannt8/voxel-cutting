#pragma once
#include "stdafx.h"
#include "bvhVoxel.h"
#include "poseManager.h"

class groupCutNode
{
public:
	groupCutNode();
	~groupCutNode();

	groupCutNode(groupCutNode *parentIn);
	void draw(int mode);
	void draw(std::vector<bone*> bones, std::map<int, int> boneMeshmap);
	void drawNeighbor(std::vector<bone*> bones, std::map<int, int> boneMeshmap, arrayVec2i neighborInfo, std::vector<neighborPos> posConfig, float voxelSize);
public:
	// Tree data
	std::vector<groupCutNode*> child;
	groupCutNode* parent;
	int depth;

public:
	std::vector<meshPiece> boxf;

	// Voxel inside and bounding box
	Vec3f leftDown; 
	Vec3f rightUp;
	arrayInt voxIdxs;
};

class groupCut
{
public:
	groupCut();
	~groupCut();

	void constructTree();
	void drawLeaveIdx(int idx);
	void drawPose(int poseIdx, int configIdx);

private:
	void constructTreeRecur(groupCutNode * node);
	bool cutBox(groupCutNode * newNode, int boxIdx, int xyzd, float coord);
	bool computeError(groupCutNode * node);

public:
	arrayInt voxelIdxs;
	std::vector<bone*> bones; // Should be sorted by order of size
	bvhVoxel *sourcePiece;
	std::vector<voxelBox> *boxes;
	std::vector<arrayInt> *neighborVoxel;
	float voxelSize;

public:
	groupCutNode *m_root;
	std::vector<groupCutNode*> leaves;
	poseGroupCutManager boxPose; // We use the pose map only
private:
	// Temporary variable for cutting 
	int *mark;
	int *voxelOccupy;

};


