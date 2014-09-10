#pragma once
#include "neighbor.h"


class BoneMapTreeNode
{
public:
	BoneMapTreeNode();
	~BoneMapTreeNode();

public:
	int depth;
	std::vector<BoneMapTreeNode*> children;
	BoneMapTreeNode *parent;

public:
	int indexOfMesh;

	// leaf node
	std::map<int,int> boneMeshIdxMap;
};

class BoneMapTree
{
public:
	BoneMapTree(void);
	~BoneMapTree(void);

	void constructTree();

public:
	BoneMapTreeNode *m_root;
	std::vector<BoneMapTreeNode*> leaves;

public:
	std::vector<bone*>* sortedBone;
	std::vector<arrayInt>* boneAroundBone;
	std::vector<Vec2i>* neighborPair;
	int nbCenterBone;

private:
	void constructTree(BoneMapTreeNode *node);
	int *mark;

	int findIdx(std::vector<bone*>* v, bone* e);
	void boneMeshIdxMapRecur(BoneMapTreeNode * node, std::map<int,int>* boneMeshIdxMap);
};

