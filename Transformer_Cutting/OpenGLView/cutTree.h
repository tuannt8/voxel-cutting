#pragma once
#include "stdafx.h"
#include "DataTypes\define.h"
#include "Voxel.h"
#include "boneAbstract.h"
#include "neighbor.h"

typedef enum 
{
	PER_X = 0, // perpendicular with x
	PER_Y = 1,
	PER_Z = 2
}cutDirection;


class cutTreeNode
{
	// Optimize later
public:
	cutTreeNode(){};
	cutTreeNode(cutTreeNode *node);
	~cutTreeNode(){
		for (int i = 0; i< children.size(); i++)
		{
			delete children[i];
		}
		children.clear();
	};

	std::vector<Boxi> * boxes(){return &m_boxes;}
	void draw(float voxelSize);
	void draw(float voxelSize, int centerX);
	int boxCount();
public:
	// tree
	cutTreeNode* parent;
	int depth; // = 0 for root node
	std::vector<cutTreeNode*> children;

	// Define cut surface
	cutDirection m_cutDirect;
	int m_cutPos;

	int m_parentCutBoxIdx;
	std::vector<Boxi> m_boxes;

	// Symmetric
	std::vector<Boxi> centerBoxi;
	std::vector<Boxi> sideBoxi;
};



class cutTree
{
public:
	cutTree(void);
	~cutTree(void);

	// Draw for debug
	void drawLeaf(int nodeIdx);

	// Construct with symmetric
	void constructTreeSymetric();
	void constructTreeSymetricRcur(cutTreeNode* node);

	// Construct without symmetric
	void constructTree(VoxelObj* voxel, std::vector<boneAbstract> &m_boneOrder, int m_roughScaleStep);

private:
	void constructTreeRer(VoxelObj* voxel, std::vector<boneAbstract> &m_boneOrder, int m_roughScaleStep, cutTreeNode *curNode);
	bool cutBox(Boxi ParentBox, cutTreeNode* node);

private: // Debug
	cutTreeNode *findLeafIdx(int idx);
	cutTreeNode *findLeafIdxRer(cutTreeNode *node, int &curIdx, int idx);
	// cut centerbox, make side box
	bool cutNodeSym(cutTreeNode* newNode, int boxIdx, int xPos);
	bool cutNodeSymAtCenter(cutTreeNode* newNode, int boxIdx);
	bool cutNodeSymYZ(cutTreeNode * newNode, int boxIdx, int Pos, int direct);
	bool cutSideBox(cutTreeNode* newNode, int boxIdx, int direction, int pos);
	std::vector<meshCutRough> convertBoxesToMeshCut(std::vector<Boxi> &Boxi);
	bool validateBoxCount(cutTreeNode* newNode);
public:
	cutTreeNode *m_rootNode;
	int m_treeDepth;

	// Share
	float m_voxelSize;
	VoxelObj *meshObj;

	// Validation
	int minVoli; // Minimal volume of cut-mesh
	int cutStep;
	// With symmetric
	std::vector<boneAbstract> centerBoneOrder;
	std::vector<boneAbstract> sideBoneOrder;
	std::vector<neighbor> neighborInfo;

	// temp
	cutTreeNode *lestE2Node;
	float leatE2;
	int boxCount;
	Vec3i leftDown; // Of mesh
	Vec3i sizei;
	int centerXi;

	// debug
	std::vector<boneAbstract> boneOrder;
	std::vector<std::pair<int,int>> neighborPair;
	int treeSize;
	
};


