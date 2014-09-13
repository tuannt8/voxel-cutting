#pragma once
#include "drawUtil.h"
#include "skeleton.h"
#include "DataTypes/objload.h"

typedef std::shared_ptr<Obj::File> objFilePtr;

struct coordInfo
{
	Vec3f origin;
	Vec3f xyzOrient[3];

	Vec3f tranfrom(Vec3f pt){
		Vec3f ptTrans = pt - origin;
		Vec3f ptOut;
		for (int i = 0; i < 3; i++)
		{
			ptOut[i] = xyzOrient[i]*ptTrans;
		}

		return ptOut;
	}
};

class meshObject
{
public:
	meshObject();
	~meshObject();

	void initOther();
	Vec3f centerPoint(){ return (leftDown + rightUp) / 2; }
public:
	PolyhedronPtr  m_polyMesh;

	bone* s_bone;
	Vec3i coord;

	Vec3f leftDown;
	Vec3f rightUp;

	// New
	objFilePtr objMesh;
	coordInfo coordMap;
};

typedef std::shared_ptr<meshObject> meshObjectPtr;