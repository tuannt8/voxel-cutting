#include "stdafx.h"
#include "meshMnger.h"
#include "Utility_wrap.h"
#include "mirrorDraw.h"
#include "transformCoord.h"
#include <iostream>
#include <string>
#include <sstream>


meshMnger::meshMnger()
{
	dlg = nullptr;
}


meshMnger::~meshMnger()
{
	for (int i = 0; i < m_meshArray.size(); i++)
	{
		m_meshArray[i].reset();
	}

	if (dlg)
	{
		delete dlg;
	}
}

void meshMnger::drawMeshFace()
{
	mirrorDraw mirror;
	mirror.mirrorAxis = 0;
	mirror.mirrorCoord = surObj->midPoint()[0];

	static arrayVec3f color = Util_w::randColor(10);
	for (int i = 0; i < m_meshArray.size(); i++)
	{
		glColor3fv(color[i].data());
		drawUtil::drawPolygonFacePtr(m_meshArray[i]->m_polyMesh);

		if (m_meshArray[i]->s_bone->m_type == SIDE_BONE)
		{
			glPushMatrix();
			glTranslatef(mirror.mirrorCoord, 0, 0);
			glScalef(-1, 1, 1);
			drawUtil::drawPolygonFacePtr(m_meshArray[i]->m_polyMesh);
			glPopMatrix();
		}
	}
}

void meshMnger::showModifyingDlg()
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CView * pView = pFrame->GetActiveView();

	dlg = new boneModifyingDlg(pView);
	dlg->Create(boneModifyingDlg::IDD, pView);
	dlg->init(m_meshArray);
	dlg->ShowWindow(SW_SHOW);
}

void meshMnger::updateBone()
{
	if (dlg)
	{
		arrayVec3f trans = dlg->arrayBoneTrans;

		for (int i = 0; i < m_meshArray.size(); i++)
		{
			m_meshArray[i]->s_bone->localTranslatef = trans[i];
		}
	}


}

void meshMnger::transformMesh()
{
	// Including translation and rotation
	for (int i = 0; i < m_meshArray.size(); i++)
	{
		PolyhedronPtr newMesh = PolyhedronPtr(new Polyhedron(*m_meshArray[i]->m_polyMesh));

		tranformCoord tc;
		tc.m_coord = m_meshArray[i]->coord;
		tc.m_tranf = m_meshArray[i]->centerPoint();

		std::vector<cVertex> * vertices = &newMesh->vertices;
		for (int j = 0; j < vertices->size(); j++)
		{
			cVertex curV = vertices->at(j);
			Vec3f curP(curV.v[0], curV.v[1], curV.v[2]);
			Vec3f tP = tc.tranfrom(curP);

			vertices->at(j) = carve::geom::VECTOR(tP[0], tP[1], tP[2]);
		}

		m_meshArray[i]->s_bone->mesh = newMesh;
		m_meshArray[i]->s_bone->meshLeftDown = tc.tranfrom(m_meshArray[i]->leftDown);
		m_meshArray[i]->s_bone->meshRightUp = tc.tranfrom(m_meshArray[i]->rightUp);
	}
}

PolyhedronPtr meshMnger::loadPolyFromObj(const char * path) const
{
	std::vector<carve::geom3d::Vector> vertices;
	arrayInt faces;
	int nbFace=0;

	using namespace std;
	ifstream file(path);
	string line;
	while (getline(file, line))
	{
		vector<string> params= Util::split(line, ' ');
		if (params[0] == "v")
		{
			float x = atof(params[1].c_str());
			float y = atof(params[2].c_str());
			float z = atof(params[3].c_str());
			vertices.push_back(carve::geom::VECTOR(x, y, z));
		}
		if (params[0] == "f")
		{
			nbFace++;
			int nbPt = params.size() - 1;
			faces.push_back(nbPt);
			for (int i = 0; i < nbPt; i++)
			{
				faces.push_back(atoi(params[i + 1].c_str())-1);
			}
		}
	}

	return PolyhedronPtr(new carve::poly::Polyhedron(vertices, nbFace, faces));
}

void meshMnger::transformMesh2()
{
	// Including translation and rotation
	for (int i = 0; i < m_meshArray.size(); i++)
	{
		PolyhedronPtr newMesh = PolyhedronPtr(new Polyhedron(*m_meshArray[i]->m_polyMesh));

		m_meshArray[i]->coordMap.origin = m_meshArray[i]->centerPoint();
		coordInfo tc;
		tc = m_meshArray[i]->coordMap;

		std::vector<cVertex> * vertices = &newMesh->vertices;
		for (int j = 0; j < vertices->size(); j++)
		{
			cVertex curV = vertices->at(j);
			Vec3f curP(curV.v[0], curV.v[1], curV.v[2]);
			Vec3f tP = tc.tranfrom(curP);

			vertices->at(j) = carve::geom::VECTOR(tP[0], tP[1], tP[2]);
		}

		m_meshArray[i]->s_bone->mesh = newMesh;
		m_meshArray[i]->s_bone->meshLeftDown = tc.tranfrom(m_meshArray[i]->leftDown);
		m_meshArray[i]->s_bone->meshRightUp = tc.tranfrom(m_meshArray[i]->rightUp);
	}
}
