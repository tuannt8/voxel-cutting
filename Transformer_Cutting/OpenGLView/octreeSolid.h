#pragma once
#include "DataTypes/vec.h"
#include "Graphics/Surfaceobj.h"
#include "cutTreeUtil.h"

class octreeSNode
{
public:
	octreeSNode();
	~octreeSNode();

public:

	void drawWireBox();
	void removeChild(octreeSNode* n);
	int childCount();
public:
	octreeSNode* parent;
	octreeSNode* children[8];

	// Octree bounding box
	Vec3f leftDownf;
	Vec3f rightUpf;

	// Bounding box of real mesh
	Vec3f leftDownTight;
	Vec3f rightUpTight;
	float volumef;

	int nodeDepth;
	bool bEnd;

	// For second phase
	int idxInLeaf;
};

class octreeSolid
{
public:
	octreeSolid(void);
	~octreeSolid(void);

	void init(char* filePath, int res);
	void initTest(); 
	void init(SurfaceObj* obj, float voxelSize);
	void init(SurfaceObj* obj, int res);
	void removeLowOccupationBox(octreeSolid* highResOctree);

	int drawMode;

	void draw(BOOL mode[10]);
	void drawTree(octreeSNode *node, int mode = 0);
	void drawTightTree(octreeSNode *node);
	void drawMesh();
	void drawWireOctree(int mode = 0);
	void drawBoundingBox();

public:
	void constructTree(SurfaceObj *obj, float voxelSize);

	void constructTree(SurfaceObj *obj, int res);
	void computeBoxAndVolume(octreeSNode *node);

	bool isColidWithBox(Box b);
	arrayInt intersectWithBox(Vec3f ldf, Vec3f ruf);
	meshPiece intersectWithBox(meshPiece &boxIn);
	void intersectWithBox(Box boxIn, Box &intersectBox, float &volumeSide);



private:
	void intersectWithBox(octreeSNode* node, Vec3f &ldf, Vec3f &ruf, arrayInt &idxs);
	bool isColidWithBox(octreeSNode* node, Box &b);
	void constructTreeRecur(octreeSNode * node, int depth);
	std::vector<Box> get8ChildrenBox(Vec3f leftDownf, Vec3f rightUpf);
	bool isSurfIntersectWithBox(SurfaceObj * sufObj, Box boxf);
	void intersectWithBox(octreeSNode* node, meshPiece &boxOut, meshPiece &boxIn);
	void intersectWithBox(octreeSNode* node, Box boxIn, Box &intersectBox, float &volumeSide);
	void enlargerToCoverBox(meshPiece& boxOut, Vec3f leftDown, Vec3f rightUp);
	void removeEmptyNode(octreeSNode * node);
public:
	octreeSNode *m_root;
	int treeDepth;

	SurfaceObj *sufObj;
	Vec3f centerMesh;
	float boxSize;

	// temp
	bool surfaceLoaded;

public:
	std::vector<octreeSNode*> leaves;
};

