#pragma once
#include "stdafx.h"
#include "DataTypes/mat.h"




struct realativeCoord
{
	Vec3f XYZDirect[3];

};


// Transform bone to box cut from mesh
// bone -> Scale -> rot -> trans -> box
// pt(box) = trans + rot*scale*pt(bone)
//
// Bone: Put at world coord. Have bounding box info
// Box: have bounding box info. Have 


class boneTransform2
{
public:
	boneTransform2(){};
	~boneTransform2(){};

	int idxNoZero(Vec3f a)
	{
		if (abs(a[0]) >= 0.1)
			return 0;
		if (abs(a[1]) >= 0.1)
			return 1;
		if (abs(a[2]) >= 0.1)
			return 2;
	}

	void init(realativeCoord coordOrient, Vec3f meshBoxLD, Vec3f meshBoxRU, Vec3f boneLD, Vec3f boneRU)
	{
		Vec3f sizeBone = boneRU - boneLD;
		Vec3f sizeMesh = meshBoxRU - meshBoxLD;

		XYZLength = sizeBone;
		
		XYZSource[0] = Vec3f(sizeBone[0], 0, 0);
		XYZSource[1] = Vec3f(0, sizeBone[1], 0);
		XYZSource[2] = Vec3f(0, 0, sizeBone[2]);
		
		for (int i = 0; i < 3; i++)
		{
			int idxNozero = idxNoZero(coordOrient.XYZDirect[i]);
			XYZDest[i] = coordOrient.XYZDirect[i]*sizeMesh[idxNozero];
		}

		CenterSource = (boneLD + boneRU)/2;
		CenterDest = (meshBoxLD + meshBoxRU)/2;
	}

	Vec3f transform(Vec3f bonePoint)
	{
		Vec3f relativeC = bonePoint - CenterSource;

		Vec3f abg;
		for (int i = 0; i < 3; i++)
		{
			abg[i] = relativeC[i]/XYZLength[i];
		}

		Vec3f out;
		for (int i = 0; i < 3; i++)
		{
			out = out + XYZDest[i]*abg[i];
		}
		return out + CenterDest;
	}

	void draw()
	{
		Vec3f origin = transform(Vec3f(0,0,0));

		Vec3f t1 = transform(Vec3f(1,0,0));
		Vec3f tt1 = t1 - origin;

		Vec3f t12 = transform(Vec3f(0,1,0));
		Vec3f tt12 = t12 - origin;

		Vec3f t13 = transform(Vec3f(0,0,1));
		Vec3f tt13 = t13 - origin;


		glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3fv(origin.data());
		glVertex3fv((transform(Vec3f(1,0,0))).data());
		glColor3f(0,1,0);
		glVertex3fv(origin.data());
		glVertex3fv((transform(Vec3f(0,1,0))).data());
		glColor3f(0,0,1);
		glVertex3fv(origin.data());
		glVertex3fv((transform(Vec3f(0,0,1))).data());
		glEnd();
	}

public:
	Vec3f XYZSource[3];
	Vec3f XYZLength;
	Vec3f CenterSource;

	Vec3f XYZDest[3];
	Vec3f CenterDest;
};