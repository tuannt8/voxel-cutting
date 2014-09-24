#pragma once
#include "stdafx.h"
#include "DataTypes\define.h"
#include "skeleton.h"
#include "meshCut.h"
#include "voxelObject.h"

extern class nonSymNode;
typedef std::shared_ptr<nonSymNode> nonSymNodePtr;

#define MERGE_MESH_RATIO 0.1 // ratio that small mesh should be merge to bigger mesh after splitting

class nonSymNode
{
public:
	nonSymNode();
	nonSymNode(nonSymNodePtr parentN);
	~nonSymNode();

	int boxCount();
	void drawBoundingBox() const;
	std::vector<meshCutPtr>& getMeshArray();
public:
	int depth;
	nonSymNodePtr parent;
	std::vector<nonSymNodePtr> childs;

	// mesh
	std::vector<meshCutPtr> meshArray;
};

class nonSymTree
{
public:
	nonSymTree();
	~nonSymTree();

	void init();
	void drawLeave(int idx1) const;

private:
	void parserSkeletonGroup();
	void constructTree();
	void constructTreeRecur(nonSymNodePtr node);

	int boneCount();
	std::vector<std::vector<meshCutPtr>> splitMesh(nonSymNodePtr node, int meshIdx, int direct, float coord);
	std::vector<meshCutPtr> splitOnePiece(meshCutPtr meshes, int direct, float coord);
	bool validateBoxCount(nonSymNodePtr node);
	std::vector<meshCutPtr> mergeToTwo(std::vector<meshCutPtr> cutPiece);
	std::vector<std::vector<meshCutPtr>> group2pieces(meshCutPtr originMesh, std::vector<meshCutPtr> cutPiece);
	bool isConnected(arrayInt remainIdx, std::vector<voxelBox> * boxes);
public:
	skeletonPtr s_groupSkeleton;
	SurfaceObjPtr surObj;
	voxelObjectPtr voxelObj;

private:
	std::vector<bone*> sortedBone;
	arrayVec2i neighborPair;

private:
	nonSymNodePtr root;

	std::vector<nonSymNodePtr> leaves;
};

typedef std::shared_ptr<nonSymTree> nonSymTreePtr;