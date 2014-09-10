#pragma once
#include "DataTypes\vec.h"

class tranformCoord
{
public:
	tranformCoord();
	~tranformCoord();

	Vec3f tranfrom(Vec3f pt);
public:
	Vec3f m_tranf;
	Vec3i m_coord;
	// From xyz (012) to coord
};

