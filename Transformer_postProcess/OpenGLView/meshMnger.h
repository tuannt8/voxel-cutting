#pragma once
#include "stdafx.h"
#include "drawUtil.h"
#include "skeleton.h"
#include "boneModifyingDlg.h"
#include "meshObject.h"

class meshMnger
{
public:
	meshMnger();
	~meshMnger();

	void showModifyingDlg();
	void drawMeshFace();

	void updateBone();

	void transformMesh();
	void transformMesh2();
	PolyhedronPtr loadPolyFromObj(const char * path) const;
public:
	std::vector<meshObjectPtr> m_meshArray;
	SurfaceObjPtr surObj;
	skeletonPtr skeleton;

	boneModifyingDlg * dlg;
};

typedef std::shared_ptr<meshMnger> meshMngerPtr;