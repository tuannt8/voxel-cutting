#pragma once
#include "stdafx.h"
#include "DataTypes/define.h"
#include "nodeGenFuncs.h"

class VoxelObj;
#define ZERO_INT 0

class sumTable
{
public:
	sumTable();
	~sumTable();

	void initSumTable(VoxelObj *obj);
	void setTablePointer(int ***table){m_table = table;};

	void computeSum();
	int volumeInBox(Boxi b);
	int volumeInBox(Vec3i leftDown, Vec3i rightUp);

	int sumValueAt(int i, int j, int k);
	int sumValueAt(Vec3i p);
	void setSumValue(Vec3i p, int val);
	void setSumValue(int i, int j, int k, int val);
	void initFromNode(nodeGeneration &nodeGen);
	float readVoxelData(char* filePath);
public:
	Vec3i m_leftDown;
	int ***m_table;
	Vec3i m_tableSize;
};