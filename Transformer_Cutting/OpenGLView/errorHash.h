#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include "cutTreeUtil.h"

class errorHashElement
{
public:
	errorHashElement();
	~errorHashElement();

public:
	std::vector<cutTreefNode* > cutNodes;
};

class errorHash
{
public:
	errorHash(void);
	~errorHash(void);

	void initTable(Vec3i tableSize);
	void addNode(cutTreefNode* node);

	cutTreefNode* findSmallestNode(Vec3f errorW);
	cutTreefNode* findSmallestNode(float neighborW, float aspectW, float volW);

public:
	Vec3i m_tableSizei;
	Vec3f m_distancef;
	errorHashElement ***m_hashTable;

public:
	Vec3i floatToIntIndex(Vec3f pt);
	errorHashElement* getNode(int i, int j, int k);
	errorHashElement* getNode(Vec3i indexi);
};

