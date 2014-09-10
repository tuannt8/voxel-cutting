#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include <bitset>
#include "DataTypes\define.h"

// Currently don't know how to declare bit set dynamically
#define X_BIT_DIM 30
#define Y_BIT_DIM 18
#define Z_BIT_DIM 34
#define BITS_SIZE (X_BIT_DIM*Y_BIT_DIM*Z_BIT_DIM)
#define VEC3_BIT_SIZE Vec3i(X_BIT_DIM, Y_BIT_DIM, Z_BIT_DIM)

typedef std::bitset<BITS_SIZE> myBitSet;

extern class hashVoxel;

_inline int bitIdx(int i, int j, int k)
{
	return i*Y_BIT_DIM*Z_BIT_DIM + j * Z_BIT_DIM + k;
}

class voxelBitConvex
{
public:
	voxelBitConvex();
	~voxelBitConvex(){};

	// draw
	void drawPureBit(hashVoxel * hashTable);
	void drawBoxSolid(hashVoxel * hashTable);
	void drawBoxWire(hashVoxel * hashTable);
	void drawBoudingBox(hashVoxel * hashTable);

	bool isIntersectWith(std::bitset<BITS_SIZE> bitSet) const;
	bool isContactWith(const voxelBitConvex &vb);
	voxelBitConvex intersectWith(voxelBitConvex *BS) const;
	void tightBoundingBox(hashVoxel * hashTable);

	// Operator
	std::bitset<BITS_SIZE> shift(int axis, int offset);
	bool operator [](const Vec3i idx);
	bool any(){ return voxelBitSet.any(); }

	// For outside access
	// Will be offset by gap
	void setValue1(Vec3i coord);
	void setValue1(int i, int j, int k);

	// Direct to bit
	void setValue1Direct(int i, int j, int k);
public:
	static voxelBitConvex makeCubeBitSet(Vec3i ldi, Vec3i rui);// Coordinate of object
	static voxelBitConvex & BoundBitSet(Vec3i ldi, Vec3i rui);
public:
	std::bitset<BITS_SIZE> voxelBitSet;
	Vec3i leftDowni; // In object coordinate
	Vec3i rightUpi;

private:
	bool isVoxelOccuby(int i, int j, int k);

};
typedef std::vector<voxelBitConvex> voxelBitConvex_array;

class voxelSplitObj
{

public:
	voxelSplitObj();
	~voxelSplitObj();

	void drawVoxelBox();
	void drawVoxelBoxWire();
	void drawFullBit();
	void drawBoundingBox();

	std::vector<voxelSplitObj> cutByOthogonalFace(int axis, int coordi);
	std::vector<voxelSplitObj> detachIndependentConvex();

	void computeBoundingBox();
	static voxelBitConvex & BoundBitSet();
public:
	voxelBitConvex_array* getBitSetArray(){ return &groupVoxels; };
	const voxelBitConvex & getConvexSet(int idx);
	


public:
	voxelBitConvex_array groupVoxels;
	Vec3f m_leftDownf;
	Vec3f m_rightUpf;
	Vec3i leftDowni;
	Vec3i rightUpi;

	// hash info
	hashVoxel *hashTable;
public:
	std::vector<arrayInt> getNeighborInfo();
};

