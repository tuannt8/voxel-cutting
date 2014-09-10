#include "stdafx.h"
#include "meshMnger.h"
#include "Utility_wrap.h"
#include "mirrorDraw.h"
#include "transformCoord.h"


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
		PolyhedronPtr curMesh = m_meshArray[i]->m_polyMesh;
		glColor3fv(color[i].data());
		drawUtil::drawPolygonFacePtr(curMesh);
		if (m_meshArray[i]->s_bone->m_type == SIDE_BONE)
		{
			glPushMatrix();
			glTranslatef(mirror.mirrorCoord, 0, 0);
			glScalef(-1, 1, 1);
			drawUtil::drawPolygonFacePtr(curMesh);
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
