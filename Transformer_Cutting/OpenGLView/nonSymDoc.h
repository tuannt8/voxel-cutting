#pragma once
#include "sphere.h"
#include "Graphics\Surfaceobj.h"
#include "skeleton.h"
#include "voxelObject.h"
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

private:
	skeletonPtr m_skeleton;
	SurfaceObjPtr m_surObj;
	voxelObjectPtr m_voxelObj;

};

