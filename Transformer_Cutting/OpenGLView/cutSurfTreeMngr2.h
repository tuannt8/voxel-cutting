#pragma once
#include "stdafx.h"
#include "Voxel.h"
#include "skeleton.h"
#include "boneAbstract.h"
#include "cutTree.h"
#include "neighbor.h"
#include "octreeSolid.h"
#include "cutTreef.h"
#include "boneTransform.h"
#include "voxelObject.h"
#include "poseManager.h"

class cutSurfTreeMngr2
{
public:
	cutSurfTreeMngr2(void);
	~cutSurfTreeMngr2(void);

	// Visualize debug
	void draw(BOOL displayMode[10]);

	void drawLeaf(int nodeIdx);
	void updateNode(int nodeIdx);

	void updateDisplay(int idx1, int idx2);
	int updateBestIdx(int idx1);

	void showWeightInputDialog();
	int findBestOption(int yIdx);

	// The order
	// 1. Load
	void loadVoxelAndSkeleton(); 
	void constructVolxeHash();

	// 2. Bone order m_boneOrder
	void ParseSkeletonOrder();
	void ParseSkeletonOrderSym();
	void ParseSkeletonOrderSymf();
	
	// 3. Distance step For rough estimating
	void decideDistanceStep(); 
	void decideDistanceStepByRatio();
	void decideDistanceStepByRatioSym();
	void decideOcTreeSize();

	// 4. RoughEstimation
	void estimateBonePos();
	void estimateBonePosUsingOctree();
	void estimateBonePosUsingVoxel();

	// 5. Cut the mesh
	void cutAndMapMeshToBone();

	// 6. Export 
	std::vector<meshCutRough> convertBoxesToMeshCut(std::vector<Boxi> centerBoxi);

	void exportMesh();
	void drawLocalCoord(cutTreefNode * node);

	// Group bone
	void init();
	void parserSkeletonGroup();
	void constructCutTree();

private:
	void setVoxelArray();
	void drawNeighborRelation();
public:
	// Group bone
	// Share data from main document
	skeleton *s_groupSkeleton;
	voxelObject *s_voxelObj;



	// Data load from file
	VoxelObj m_voxelHighRes;

	//
	skeleton m_skeleton;
	octreeSolid m_octree;

	// Different poses
	poseManager poseMngr;

	// Hash voxel
	std::vector<voxelBox> boxes; // all voxel pixel box
	hashVoxel hashTable;

	bool bUniformCut;
	cutTreef m_tree2;
	cutTreefNode *leatE2Node2;

	// variables
	std::vector<boneAbstract> m_boneOrder;
	std::vector<std::pair<int,int>> neighborPair;

	// With symmetric
	std::vector<boneAbstract> m_centerBoneOrder;
	std::vector<boneAbstract> m_sideBoneOrder;
	std::vector<neighbor> neighborInfo;

	int m_roughScaleStep;	// Step for rough estimation
	float octreeSizef;

	// tree
	cutTree m_tree;
	cutTreeNode *leatE2Node;

	// Debug
	int poseIdx, nodeIdx; // Index in pose array
	cutTreefNode* curNode;
	std::vector<boneTransform2> coords;
	std::vector<CString> names;
	std::vector<Vec3f> centerPos;
	arrayVec2i meshNeighbor;
	neighborPose currentPose;
	std::vector<meshPiece> allMeshes;

	// User define weight error
	Vec3f m_weightError; // neighbor - aspect - volume
};

