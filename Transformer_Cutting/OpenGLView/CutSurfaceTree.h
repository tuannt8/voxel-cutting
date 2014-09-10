#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include "Voxel.h"
#include "skeleton.h"

#define MAX_CUT_CHILD 100


class box
{
public:
	box(Vec3i ld, Vec3i ru){leftDown = ld; rightUp = ru;};
	~box(){};
	Vec3i leftDown;
	Vec3i rightUp;
};

class cutNode
{
public:
	cutNode();
	~cutNode();

public:
	int depth;
	cutNode * parent;
	int childCount;
	cutNode * child[MAX_CUT_CHILD];

	// Cut surface info
	int c_direction;
	int c_position;

	std::vector<box> boundingBoxes;	// Of each small piece after cut
	std::vector<box>& getPieces(){return boundingBoxes;};
};

class CutSurfaceTree
{
public:
	CutSurfaceTree(void);
	~CutSurfaceTree(void);

	void loadObject();

	void constructCutTree();

private:
	void constructChildNode(cutNode* node);

public:
	VoxelObj m_voxelMesh;
	skeleton m_skeleton;
	cutNode *m_root;

public:
	int nbBones;
	std::vector<bone*> boneArray;
};

