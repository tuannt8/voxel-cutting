#include "stdafx.h"
#include "voxelSplitObj.h"
#include "voxelObject.h"


voxelSplitObj::voxelSplitObj()
{
}


voxelSplitObj::~voxelSplitObj()
{
}

void voxelSplitObj::drawVoxelBox()
{
	for (int i = 0; i < groupVoxels.size(); i++)
	{
		groupVoxels[i].drawBoxSolid(hashTable);
	}
}

std::vector<voxelSplitObj> voxelSplitObj::cutByOthogonalFace(int axis, int coordi)
{
	assert(coordi >= 0);
	assert(axis >= 0 && axis < 3);

	std::vector<voxelSplitObj> outArray;

	// First, split the object to two half
	
	{// Cut by lower bound
		Vec3i ruLower = rightUpi;
		ruLower[axis] = coordi;
	static	voxelBitConvex setLower = voxelBitConvex::makeCubeBitSet(leftDowni, ruLower);

		voxelSplitObj lowerObj;
		for (int i = 0; i < groupVoxels.size(); i++)
		{
			const voxelBitConvex curBS = getConvexSet(i);
			voxelBitConvex newCVSet = curBS.intersectWith(&setLower);
			if (newCVSet.any())
			{
				newCVSet.tightBoundingBox(hashTable);
				lowerObj.groupVoxels.push_back(newCVSet);
			}

		}
		if (lowerObj.groupVoxels.size() > 0)
		{
			std::vector<voxelSplitObj> lowerGroup = lowerObj.detachIndependentConvex();

			for (int i = 0; i < lowerGroup.size(); i++)
			{
				lowerGroup[i].hashTable = hashTable;
				lowerGroup[i].computeBoundingBox();
				outArray.push_back(lowerGroup[i]);
			}
		}
	}
	
	{// Upper bound
		Vec3i ldUpper = leftDowni;
		ldUpper[axis] = coordi;
	static	voxelBitConvex setUpper = voxelBitConvex::makeCubeBitSet(ldUpper, rightUpi);

		voxelSplitObj upperObj;
		for (int i = 0; i < groupVoxels.size(); i++)
		{
			const voxelBitConvex curBS = getConvexSet(i);
			voxelBitConvex newCVSet = curBS.intersectWith(&setUpper);
			if (newCVSet.any())
			{
				newCVSet.tightBoundingBox(hashTable);
				upperObj.groupVoxels.push_back(newCVSet);
			}
		}
		if (upperObj.groupVoxels.size() > 0)
		{
			std::vector<voxelSplitObj> uGroup = upperObj.detachIndependentConvex();

			for (int i = 0; i < uGroup.size(); i++)
			{
				uGroup[i].hashTable = hashTable;
				uGroup[i].computeBoundingBox();
				outArray.push_back(uGroup[i]);
			}
		}	
	}
	
	return outArray;
}

const voxelBitConvex & voxelSplitObj::getConvexSet(int idx)
{
	return groupVoxels[idx];
}

void voxelSplitObj::computeBoundingBox()
{
	// Detect bounding box by shifting the object
	// The bounding is smaller than current bounding box
	leftDowni = Vec3i(MAX, MAX, MAX);
	rightUpi = Vec3i(MIN, MIN, MIN);

	GeometricFunc geoFunc;
	for (int i = 0; i < groupVoxels.size(); i++)
	{
		geoFunc.combineBoxi(leftDowni, rightUpi, groupVoxels[i].leftDowni, groupVoxels[i].rightUpi);
	}

	// Compute float coord
	m_leftDownf = hashTable->IJK2XYZLower(leftDowni); 
	m_rightUpf = hashTable->IJK2XYZLower(rightUpi);
}

voxelBitConvex & voxelSplitObj::BoundBitSet()
{
//	Vec3i sizei = hashTable->NumXYZ + Vec3i(2, 2, 2);
	static voxelBitConvex gBS;

	return gBS;
}

void voxelSplitObj::drawFullBit()
{
	for (int ii = 0; ii < X_BIT_DIM; ii++)
	{
		for (int jj = 0; jj < Y_BIT_DIM; jj++)
		{
			for (int kk = 0; kk < Z_BIT_DIM; kk++)
			{
				Vec3i voxelCi = Vec3i(ii, jj, kk);
				Vec3f coordf = hashTable->IJK2XYZ(voxelCi);
				Util_w::drawBoxWire(coordf, hashTable->voxelSize / 2);
			}
		}
	}
}

void voxelSplitObj::drawBoundingBox()
{
	Util_w::drawBoxWireFrame(m_leftDownf, m_rightUpf);
}

void voxelSplitObj::drawVoxelBoxWire()
{
	for (int i = 0; i < groupVoxels.size(); i++)
	{
		groupVoxels[i].drawBoxWire(hashTable);
	}
}

std::vector<voxelSplitObj> voxelSplitObj::detachIndependentConvex()
{
	// Build neighbor relationship
	std::vector<arrayInt> nb = getNeighborInfo();

	int *marked = new int[nb.size()];
	std::fill(marked, marked + nb.size(), 0);

	// breadth first search
	// even it is very small
	std::vector<voxelSplitObj> out;
	while (TRUE)
	{
		// find a free convex
		int idx = -1;
		for (int i = 0; i < nb.size(); i++)
		{
			if (marked[i] == 0)
			{
				idx = i;
				break;
			}
		}
		if (idx == -1)
		{
			break;
		}

		std::queue<int> q;
		q.push(idx);
		voxelSplitObj newObj;
		while (!q.empty())
		{
			int curIdx = q.front();
			q.pop();

			newObj.groupVoxels.push_back(groupVoxels[curIdx]);
			marked[curIdx] = 1;

			arrayInt neighbor = nb[curIdx];
			for (int j = 0; j < neighbor.size(); j++)
			{
				if (marked[neighbor[j]] == 0)
				{
					q.push(neighbor[j]);
				}
			}
		}
		out.push_back(newObj);
	}

	return out;
}

std::vector<arrayInt> voxelSplitObj::getNeighborInfo()
{
	std::vector<arrayInt> nb;
	nb.resize(groupVoxels.size());
	for (int i = 0; i < groupVoxels.size(); i++)
	{
		for (int j = i+1; j < groupVoxels.size(); j++)
		{
			if (groupVoxels[i].isContactWith(groupVoxels[j]))
			{
				nb[i].push_back(j);
				nb[j].push_back(i);
			}
		}
	}

	return nb;
}

void voxelBitConvex::drawBoxSolid(hashVoxel * hashTable)
{
	for (int ii = 0; ii < X_BIT_DIM; ii++)
	{
		for (int jj = 0; jj < Y_BIT_DIM; jj++)
		{
			for (int kk = 0; kk < Z_BIT_DIM; kk++)
			{

				if (isVoxelOccuby(ii, jj ,kk))
				{
					Vec3i voxelCi = Vec3i(ii, jj, kk);
					Vec3f coordf = hashTable->IJK2XYZ(voxelCi); 
					Util_w::drawBoxSolidCenter(coordf, hashTable->voxelSize / 2);
				}
			}
		}
	}
}

bool voxelBitConvex::operator [](const Vec3i idx)
{
	return isVoxelOccuby(idx[0], idx[1], idx[2]);
}

bool voxelBitConvex::isVoxelOccuby(int i, int j, int k)
{
	int idx = bitIdx(i, j, k);
	return voxelBitSet[idx];
}

voxelBitConvex::voxelBitConvex()
{
	voxelBitSet.reset();
}

void voxelBitConvex::setValue1(Vec3i coord)
{
	setValue1(coord[0], coord[1], coord[2]);
}

void voxelBitConvex::setValue1(int i, int j, int k)
{
	int idx = bitIdx(i, j, k); // 1 layer gap
	voxelBitSet[idx] = TRUE;
}

void voxelBitConvex::drawBoxWire(hashVoxel * hashTable)
{
	for (int ii = 0; ii < X_BIT_DIM; ii++)
	{
		for (int jj = 0; jj < Y_BIT_DIM; jj++)
		{
			for (int kk = 0; kk < Z_BIT_DIM; kk++)
			{

				if (isVoxelOccuby(ii, jj, kk))
				{
					Vec3i voxelCi = Vec3i(ii, jj, kk);
					Vec3f coordf = hashTable->IJK2XYZ(voxelCi);
					Util_w::drawBoxWire(coordf, hashTable->voxelSize / 2);
				}
			}
		}
	}
}

voxelBitConvex voxelBitConvex::makeCubeBitSet(Vec3i ldi, Vec3i rui)
{
	voxelBitConvex newSet;
	for (int i = ldi[0]; i < rui[0]; i++)
	{
		for (int j = ldi[1]; j < rui[1]; j++)
		{
			for (int k = ldi[2]; k < rui[2]; k++)			
			{
				newSet.setValue1(i, j, k);
			}
		}
	}

	return newSet;
}

voxelBitConvex voxelBitConvex::intersectWith(voxelBitConvex *BS) const
{
	voxelBitConvex newBS;
	newBS.voxelBitSet = voxelBitSet & BS->voxelBitSet;
	newBS.leftDowni = leftDowni;
	newBS.rightUpi = rightUpi;
	return newBS;
}

voxelBitConvex & voxelBitConvex::BoundBitSet(Vec3i ldi, Vec3i rui)
{
	voxelBitConvex gBS;
// 	// x direction
	for (int j = ldi[1]; j < rui[1]; j++)
	{
		for (int k = ldi[2]; k < rui[2]; k++)
		{
			gBS.setValue1Direct(ldi[0]-1, j, k);
			gBS.setValue1Direct(rui[0], j, k);
		}
	}

	// y direction
	for (int i = ldi[0]; i < rui[0]; i++)
	{
		for (int k = ldi[2]; k < rui[2]; k++)
		{
			gBS.setValue1Direct(i, ldi[1]-1, k);
			gBS.setValue1Direct(i, rui[1], k);
		}
	}

	// z direction
	for (int i = ldi[0]; i < rui[0]; i++)
	{
		for (int j = ldi[1]; j < rui[1]; j++)
		{
			gBS.setValue1Direct(i, j, ldi[2]-1);
			gBS.setValue1Direct(i, j, rui[2]);
		}
	}

	return gBS;
}

void voxelBitConvex::setValue1Direct(int i, int j, int k)
{
	int idx = bitIdx(i , j , k ); // 1 layer gap
	voxelBitSet[idx] = TRUE;
}

void voxelBitConvex::tightBoundingBox(hashVoxel * hashTable)
{
	Vec3i ldi = Vec3i(1, 1, 1);
	Vec3i rui = hashTable->NumXYZ - Vec3i(1, 1, 1);
	voxelBitConvex gapBound = voxelBitConvex::BoundBitSet(ldi, rui);

	// left down x direction
	Vec3i ldOff = leftDowni - ldi;
	Vec3i ruOff = rui - rightUpi;

	assert(ldOff[0] >= 0 && ldOff[1] >= 0 && ldOff[2] >= 0);
	assert(ruOff[0] >= 0 && ruOff[1] >= 0 && ruOff[2] >= 0);

	for (int direct = 0; direct < 3; direct++)
	{
		// Left down offset
		while (true)
		{
			std::bitset<BITS_SIZE> shifted = shift(direct, -ldOff[direct]);
			if (gapBound.isIntersectWith(shifted))
			{
				break;
			}
			ldOff[direct]++;
		}
		// Right up offset
		while (true)
		{
			std::bitset<BITS_SIZE> shifted = shift(direct, ruOff[direct]);
			assert(shifted.any());
			if (gapBound.isIntersectWith(shifted))
			{
				break;
			}
			ruOff[direct]++;
		}
	}

	// Modify the bounding box
	leftDowni = ldOff;
	rightUpi = rui - ruOff+Vec3i(1,1,1);
}

std::bitset<BITS_SIZE> voxelBitConvex::shift(int axis, int offset)
{
	int dis = 0;

	if (axis == 2) // z
	{
		dis = std::abs(offset);
	}

	if (axis == 1) // y
	{
		dis = std::abs(offset) * Z_BIT_DIM;
	}

	if (axis == 0) // z
	{
		dis = std::abs(offset) * Y_BIT_DIM * Z_BIT_DIM;
	}

	if (offset < 0)
	{
		return voxelBitSet >> dis;
	}
	else
		return voxelBitSet << dis;
}

bool voxelBitConvex::isIntersectWith(std::bitset<BITS_SIZE> bitSet) const
{
	return (voxelBitSet & bitSet).any();
}

void voxelBitConvex::drawBoudingBox(hashVoxel * hashTable)
{
	Vec3f ld = hashTable->leftDown + Util_w::Vec3iXfloat(leftDowni, hashTable->voxelSize);
	Vec3f ru = hashTable->leftDown + Util_w::Vec3iXfloat(rightUpi, hashTable->voxelSize);

	Util_w::drawBoxWireFrame(ld, ru);
}

void voxelBitConvex::drawPureBit(hashVoxel * hashTable)
{
	for (int ii = 0; ii < X_BIT_DIM; ii++)
	{
		for (int jj = 0; jj < Y_BIT_DIM; jj++)
		{
			for (int kk = 0; kk < Z_BIT_DIM; kk++)
			{
				if (isVoxelOccuby(ii, jj, kk))
				{
					Vec3i voxelCi = Vec3i(ii, jj, kk);
					Vec3f coordf = hashTable->IJK2XYZ(voxelCi);
					Util_w::drawBoxWire(coordf, hashTable->voxelSize / 2);
				}
			}
		}
	}
}

bool voxelBitConvex::isContactWith(const voxelBitConvex &vb)
{
	for (int direct = 0; direct < 3; direct++)
	{
		if (vb.isIntersectWith(shift(direct, -1)))
		{
			return true;
		}
		if (vb.isIntersectWith(shift(direct, 1)))
		{
			return true;
		}
	}

	return false;
}
