#pragma once
#include "stdafx.h"
#include "DataTypes\mat.h"
#include "DataTypes\vec.h"
#include "DataTypes\define.h"

namespace transformUtil
{

	Mat4x4f translationMat(Vec3f t)
	{
		arrayFloat pp = { 1, 0, 0, t[0],
			0, 1, 0, t[1],
			0, 0, 1, t[2],
			0, 0, 0, 1 };

		return Mat4x4f(pp);
	}

	Mat4x4f rot_X_Mat(float angle) // deg
	{
		angle = angle * PI / 180.0;

		arrayFloat ip = { 1, 0, 0, 0,
			0, cos(angle), -sin(angle), 0,
			0, sin(angle), cos(angle), 0,
			0, 0, 0, 1 };

		return Mat4x4f(ip);
	}

	Mat4x4f rot_Y_Mat(float angle) // deg
	{
		angle = angle * PI / 180.0;

		arrayFloat ip = { cos(angle), 0, sin(angle), 0,
			0, 1, 0, 0,
			-sin(angle), 0, cos(angle), 0,
			0, 0, 0, 1 };

		return Mat4x4f(ip);
	}

	Mat4x4f rot_Z_Mat(float angle) // deg
	{
		angle = angle * PI / 180.0;

		arrayFloat ip = { cos(angle), -sin(angle), 0, 0,
			sin(angle), cos(angle), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };

		return Mat4x4f(ip);
	}
}