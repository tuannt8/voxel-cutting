#include "stdafx.h"
#include "bitSetObject.h"

namespace energy{

	bitSetObject::bitSetObject()
	{
		voxelBitSet.reset();
	}

	bitSetObject::bitSetObject(Vec3i sizei)
		: bitSetObject()
	{
		numXYZ = sizei;
	}


	bitSetObject::~bitSetObject()
	{
	}

	void bitSetObject::setAtPos(Vec3i pos, bool val /*= true*/)
	{
#ifdef _DEBUG
		int idx = idxPos(pos);
#endif
		voxelBitSet.set(idxPos(pos), val);
	}

	int bitSetObject::idxPos(Vec3i coordi)
	{
		return coordi[2] * numXYZ[0] * numXYZ[1] + coordi[1] * numXYZ[0] + coordi[0];
	}

	energy::bitSetObjectPtr bitSetObject::intersectWith(bitSetObjectPtr obj)
	{
		bitSetObjectPtr interS = bitSetObjectPtr(new bitSetObject(numXYZ));
		interS->voxelBitSet = voxelBitSet & obj->voxelBitSet;

		return interS;
	}

	bool bitSetObject::at(int idx)
	{
		return voxelBitSet[idx];
	}

	int bitSetObject::count()
	{
		return voxelBitSet.count();
	}

}


