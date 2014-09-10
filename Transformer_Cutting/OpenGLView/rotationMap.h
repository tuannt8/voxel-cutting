#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include <map>
#include "..\KEGIES\bonePacking.h"
#include "DataTypes\mat.h"

#define matEYEi Mat3x3i(Vec3i(1,0,0), Vec3i(0,1,0), Vec3i(0,0,1))
#define rotXi Mat3x3i(Vec3i(1,0,0), Vec3i(0,0,1), Vec3i(0,1,0))
#define rotYi Mat3x3i(Vec3i(0,0,1), Vec3i(0,1,0), Vec3i(1,0,0))
#define rotZi Mat3x3i(Vec3i(0,1,0), Vec3i(1,0,0), Vec3i(0,0,1))

class rotationMap
{

};