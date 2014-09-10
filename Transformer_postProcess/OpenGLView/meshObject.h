#pragma once
#include "drawUtil.h"
#include "skeleton.h"

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
};

typedef std::shared_ptr<meshObject> meshObjectPtr;