#pragma once
#include "stdafx.h"
#include "DataTypes\define.h"
#include "boneAbstract.h"
#include "neighbor.h"
#include "octreeSolid.h"
#include "cutTreeUtil.h"
#include "errorHash.h"
#include "voxelObject.h"
#include "poseManager.h"

typedef enum{
	CODE_NULL							= 0x0000,
	CODE_NEIGHBOR_CONTACT_SURFACE	= 0x0001,
} codeDebug;



class cutTreef
{
public:
	cutTreef(void);
	~cutTreef(void);

	void constructTree();
	void drawLeaf(int nodeIdx);
	void drawVoxel(cutTreefNode*node, std::vector<voxelBox>* boxes);

	void constructTreeWithVoxel();

private:
	void constructTreeRecur(cutTreefNode *node);
	bool cutNodeSym(cutTreefNode* newNode, int centerBoxIdx, float cx);
	bool validateBoxCount(cutTreefNode* newNode);
	bool cutNodeSymAtCenter(cutTreefNode* newNode, int boxIdx);
	bool cutNodeSymYZ(cutTreefNode* newNode, int cenerBoxIdx, float coord, int yz);
	bool cutSideBox(cutTreefNode * newNode, int sideBIdx, int dxyz, float coord);
	bool computeErrorLeaf(cutTreefNode * node);
	std::vector<meshCutRough> convertBoxesToMeshCut(std::vector<Box> centerBoxf);
	
private:
	void constructTreeVoxelRecur(cutTreefNode *node);
	bool cutNodeSymVoxel(cutTreefNode* newNode, int centerBoxIdx, float cx);
	bool cutNodeSymAtCenterVoxel(cutTreefNode* newNode, int boxIdx);
	bool cutNodeSymYZVoxel(cutTreefNode* newNode, int cenerBoxIdx, float coord, int yz);
	bool cutSideBoxVoxel(cutTreefNode * newNode, int sideBIdx, int dxyz, float coord);

public:
	cutTreefNode *m_root;
	int m_treeDepth;

	octreeSolid *octreeS;
	Vec3f centerMesh;
	float meshVolume;
	poseManager *poseMngr;

	// With symmetric
	float cutStep;
	bool bUniformCutStep;


	std::vector<boneAbstract> centerBoneOrder;
	std::vector<boneAbstract> sideBoneOrder;
	std::vector<neighbor> neighborInfo;
	float minVol; // Minimal cutmesh volume
	int boxNum;

	cutTreefNode *lestE2Node;
	float leatE2;

	// Hash voxel
	std::vector<voxelBox> *boxes; // all voxel pixel box
	hashVoxel *hashTable;
	int *mark;
	int *voxelOccupy;

	// For error
	errorHash m_hashErrorTable;

	// Debug
	std::vector<cutTreefNode*> leaves;
};

