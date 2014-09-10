#pragma once
#include "stdafx.h"
#include "voxelObject.h"
#include <memory>

class debugInfo
{
public:
	debugInfo();
	~debugInfo();

	void draw();


	void reload();


public:
	voxelObject* s_voxelObj;

	//1
	std::vector < std::vector<std::string>> data;
	arrayInt voxelIdxs;
	void drawVoxelIdxs();
	void reloadData();

	//2
	Vec2i pair;
	void drawtestVoxelSym();
};

typedef std::shared_ptr<debugInfo> debugInfoPtr;