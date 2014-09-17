#pragma once
#include "stdafx.h"
#include "bitSetObject.h"
#include "DataTypes\vec.h"
#include "voxelObject.h"

namespace energy
{
	class bitSetMangr
	{
	public:
		bitSetMangr();
		~bitSetMangr();

		void init(voxelObjectPtr voxelHash);
		void drawBoundingBox();

		void drawSolidBit(bitSetObjectPtr obj);
		void drawWireBit(bitSetObjectPtr obj);

		bitSetObjectPtr createSphere(Vec3f center, float radius);
		int bitSetSize();
	private:
		Vec3f leftDown;
		Vec3f rightUp;

		Vec3i numXYZ;
		float boxSizef;

		// Share
		hashVoxel *hashTable;

	private:
		_inline Vec3i posiFromIndex(int idx);
		_inline Vec3f posfFromIndex(int idx);
	};

	typedef std::shared_ptr<bitSetMangr> bitSetMngrPtr;
}