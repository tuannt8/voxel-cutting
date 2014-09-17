#include "stdafx.h"
#include "bitSetMangr.h"

namespace energy{

	bitSetMangr::bitSetMangr()
	{
	}


	bitSetMangr::~bitSetMangr()
	{
	}

	void bitSetMangr::init(voxelObjectPtr voxelHash)
	{
		leftDown = voxelHash->m_hashTable.leftDown;
		rightUp = voxelHash->m_hashTable.rightUp;
		numXYZ = voxelHash->m_hashTable.NumXYZ;
		boxSizef = voxelHash->m_voxelSizef;

		hashTable = &voxelHash->m_hashTable;

	}

	energy::bitSetObjectPtr bitSetMangr::createSphere(Vec3f center, float radius)
	{
		// Create bit set of the sphere
		// Any voxel that center line inside the sphere

		Vec3i posi = hashTable->getVoxelCoord(center);
		int disi = round(radius / boxSizef);
		Vec3i ldi = posi - Vec3i(disi, disi, disi);
		Vec3i rui = posi + Vec3i(disi, disi, disi);

		bitSetObjectPtr newObj = bitSetObjectPtr(new bitSetObject(numXYZ));
		for (int i = ldi[0]; i < rui[0]; i++)
		{
			for (int j = ldi[1]; j < rui[1]; j++)
			{
				for (int k = ldi[2]; k < rui[2]; k++)
				{
					Vec3i vPosi = Vec3i(i, j, k);
					Vec3f vPosf = hashTable->IJK2XYZ(vPosi);
					if ((center-vPosf).norm() < radius)
					{
						newObj->setAtPos(vPosi);
					}
				}
			}
		}

		return newObj;
	}

	void bitSetMangr::drawSolidBit(bitSetObjectPtr obj)
	{
		ASSERT(obj);
		for (int i = 0; i < bitSetSize(); i++)
		{
			if (obj->at(i))
			{
				Vec3f pos = posfFromIndex(i);
				Util_w::drawBoxSolidCenter(pos, boxSizef / 2);
			}
		}
	}

	int bitSetMangr::bitSetSize()
	{
		return numXYZ[0] * numXYZ[1] * numXYZ[2];
	}

	Vec3i bitSetMangr::posiFromIndex(int idx)
	{
		Vec3i pos;
		pos[2] = floor(idx / (numXYZ[0]*numXYZ[1]));
		pos[1] = (idx - pos[2] * numXYZ[0] * numXYZ[1]) / numXYZ[0];
		pos[0] = idx - pos[2] * numXYZ[0] * numXYZ[1] - pos[1] * numXYZ[0];

		return pos;
	}

	Vec3f bitSetMangr::posfFromIndex(int idx)
	{
		Vec3i posi = posiFromIndex(idx);
		return leftDown + Util_w::IJK2XYZ(posi, boxSizef);
	}

	void bitSetMangr::drawWireBit(bitSetObjectPtr obj)
	{
		ASSERT(obj);
		for (int i = 0; i < bitSetSize(); i++)
		{
			if (obj->at(i))
			{
				Vec3f pos = posfFromIndex(i);
				Util_w::drawBoxWire(pos, boxSizef / 2);
			}
		}
	}

	void bitSetMangr::drawBoundingBox()
	{
		Util_w::drawBoxWireFrame(leftDown, rightUp);
	}

}


