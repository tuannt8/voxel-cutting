#include "StdAfx.h"
#include "boneAbstract.h"
#include "Utility_wrap.h"


boneAbstract::boneAbstract(void)
{
}

boneAbstract::boneAbstract(bone* boneO, float voxelSize)
{
	Vec3f sizef = boneO->m_sizef;
	sizei = Util_w::XYZ2IJK(sizef, voxelSize);

	Vec3i SMLIdx = Util_w::SMLIndexSizeOrder(sizef);
	aspectRatiof[0] = sizef[SMLIdx[2]]/sizef[SMLIdx[0]];
	aspectRatiof[1] = sizef[SMLIdx[1]]/sizef[SMLIdx[0]];
}

boneAbstract::~boneAbstract(void)
{
}


Vec3i boneAbstract::getSizeByOrientation(int orientation)
{
	int x = sizei[0], y = sizei[1], z = sizei[2];

	if (orientation & R_X_90)
	{
		// Swap y and z
		int temp = z;
		z = y;
		y = temp;
	}

	if (orientation & R_Y_90)
	{
		// Swap x and z
		int temp = z;
		z = x;
		x = temp;
	}

	if (orientation & R_Z_90)
	{
		// Swap y and x
		int temp = x;
		x = y;
		y = temp;
	}

	return Vec3i(x,y,z);
}

Vec3i boneAbstract::getSizeByOrientation(BOOL xRot, BOOL yRot, BOOL zRot)
{
	int orien = 0;
	if(xRot)
		orien = orien | R_X_90;
	if(yRot)
		orien = orien | R_Y_90;
	if(zRot)
		orien = orien | R_Z_90;

	return getSizeByOrientation(orien);
}