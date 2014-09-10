#include "stdafx.h"
#include "tranformCoord.h"
#include "DataTypes\define.h"


tranformCoord::tranformCoord()
{
}


tranformCoord::~tranformCoord()
{
}

Vec3f tranformCoord::tranfrom(Vec3f pt)
{
	arrayVec3f xyz = { Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0), Vec3f(0.0, 0.0, 1.0) };

	Vec3f ptTrans = pt - m_tranf;
	Vec3f ptOut;
	for (int i = 0; i < 3; i++)
	{
		ptOut = ptOut + xyz[m_coord[i]]*ptTrans[i];
	}

	return ptOut;
}
