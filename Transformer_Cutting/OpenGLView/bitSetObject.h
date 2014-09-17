#pragma once
#include "stdafx.h"
#include <bitset>
#include "DataTypes\vec.h"

namespace energy{
	using namespace std;

#define DIM_X 100
#define DIM_Y 100
#define DIM_Z 100
#define SET_SIZE (DIM_X*DIM_Y*DIM_Z)

	extern class bitSetObject;
	typedef std::shared_ptr<bitSetObject> bitSetObjectPtr;

	class bitSetObject 
	{
		bitSetObject();

	public:
		bitSetObject(Vec3i sizei);
		~bitSetObject();

		bitSetObjectPtr intersectWith(bitSetObjectPtr obj);

		void draw();

		// bit set operation
		void setAtPos(Vec3i pos, bool val = true);
		bool at(int idx);
		int count();

	private:
		Vec3i numXYZ;

		bitset<SET_SIZE> voxelBitSet;

	private:
		_inline int idxPos(Vec3i posi);
	};

//	typedef std::shared_ptr<bitSetObject> bitSetObjectPtr;
}

