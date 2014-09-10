#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include "Utility_wrap.h"

// This class will not use any specific data structure

class mirrorDraw
{
public:
	mirrorDraw(){};
	mirrorDraw(int axis, float coord)
	{
		mirrorAxis = axis;
		mirrorCoord = coord;
	}
	~mirrorDraw(){};

	Vec3f mirror(Vec3f pt)
	{
		Vec3f ptOut = pt;
		ptOut[mirrorAxis] = 2*mirrorCoord - ptOut[mirrorAxis];
		return ptOut;
	}

	void drawMirrorBox(Vec3f ld, Vec3f ru)
	{
		float offset = (mirrorCoord-ru[mirrorAxis]) + mirrorCoord - ld[mirrorAxis];
		ru[mirrorAxis] += offset;
		ld[mirrorAxis] += offset;

		Util_w::drawBoxWireFrame(ld, ru);
	}
public:
	int mirrorAxis;			// 0-x, 1-y, 2-z
	float mirrorCoord;		// position of mirror surface
};