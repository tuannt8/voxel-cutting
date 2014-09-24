#pragma once
#include "sphere.h"
#include "Graphics\Surfaceobj.h"
#include "skeleton.h"
#include "voxelObject.h"
#include "nonSymTree.h"
class nonSymDoc
{
public:
	nonSymDoc();
	~nonSymDoc();

public:
	// Receive from View
	void draw(BOOL mode[10]);
	void draw2(bool mode[10]);

	void receiveKey(char c);
	void updateIdx(int yIdx, int zIdx);
	void updateRealtime();

public:
	void loadFile();
	void constructTree();

private:
	skeletonPtr m_skeleton;
	SurfaceObjPtr m_surObj;
	voxelObjectPtr m_voxelObj;

	nonSymTreePtr m_cutTree;

private: // Debug
	int idx1, idx2;
	
};

