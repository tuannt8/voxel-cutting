#include "StdAfx.h"
#include "coordAssignManager.h"
#include "Resource.h"
#include "detailSwapManager.h"


coordAssignManager::coordAssignManager(void)
{
	s_detailSwap = nullptr;
	dlg = nullptr;

}


coordAssignManager::~coordAssignManager(void)
{
}

void coordAssignManager::assignCoord(detailSwapManager* detailSwap, CWnd* parent)
{
	s_detailSwap = detailSwap;

	dlg = new coordAsignDlg(parent);
	dlg->Create(IDD_DIALOG_ASSIGN_COORD, parent);
	dlg->init(detailSwap);
	dlg->ShowWindow(SW_SHOW);
}

void coordAssignManager::draw(BOOL mode[10])
{
	static arrayVec3f color = Util_w::randColor(10);
	mirrorDraw mirror;
	mirror.mirrorAxis = 0;
	mirror.mirrorCoord = s_detailSwap->m_octree.centerMesh[0];
	std::vector<bvhVoxel*> meshBox = s_detailSwap->meshBox;

	if (mode[1]) // Draw box
	{
		int curBone = dlg->getCurBoneIdx();
		for (int i = 0; i < meshBox.size(); i++)
		{
			if (i == curBone)
			{
				glLineWidth(2.0);
			}
			else
			{
				glLineWidth(1.0);
			}

			glColor3fv(color[i].data());
			meshBox[i]->drawOriginalBox();

			if (meshBox[i]->boneType == SIDE_BONE)
			{
				glColor3fv(color[i].data());
				mirror.drawMirrorBox(meshBox[i]->leftDown, meshBox[i]->rightUp);
			}
		}

	}

	if (mode[2]) // draw coord
	{
		arrayVec3i coords = dlg->coords;
		Vec3f RGBcolor[3] = {Vec3f(1,0,0), Vec3f(0,1,0), Vec3f(0,0,1)};
		glBegin(GL_LINES);
		for (int i = 0; i < coords.size(); i++)
		{
			Vec3i cc = coords[i];
			Vec3f c = (meshBox[i]->leftDown + meshBox[i]->rightUp)/2;
			for (int j = 0; j < 3; j++)
			{
				if (cc[j] == -1)
				{
					continue;
				}

				glColor3fv(RGBcolor[j].data());
				glVertex3fv(c.data());
				glVertex3fv((c+RGBcolor[cc[j]]).data());

				if (meshBox[i]->boneType == SIDE_BONE)
				{
					Vec3f mm = mirror.mirror(c);
					glVertex3fv(mm.data());
					glVertex3fv((mm+RGBcolor[cc[j]]).data());
				}
			}
		}
		glEnd();
	}

	if (mode[3]) // draw mesh
	{
		s_detailSwap->m_octree.drawMesh();
	}
}

void coordAssignManager::init(std::vector<bone*> boneFullArray, std::vector<bvhVoxel> meshBoxFull)
{
	m_boneFullArray = boneFullArray;
	m_meshBoxFull = meshBoxFull;

	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CView * pView = pFrame->GetActiveView();
	dlg = new coordAsignDlg(pView);
	dlg->Create(IDD_DIALOG_ASSIGN_COORD, pView);
	dlg->init(&m_boneFullArray, &m_meshBoxFull);
	dlg->ShowWindow(SW_SHOW);
}

void coordAssignManager::draw2(BOOL mode[10])
{
	static arrayVec3f color = Util_w::randColor(10);
	mirrorDraw mirror;
	mirror.mirrorAxis = 0;
	mirror.mirrorCoord = s_detailSwap->m_octree.centerMesh[0];

	// Draw box
	{
		int curBone = dlg->getCurBoneIdx();
		for (int i = 0; i < m_meshBoxFull.size(); i++)
		{
			if (i == curBone)
			{
				glLineWidth(2.0);
			}
			else
			{
				glLineWidth(1.0);
			}

			glColor3fv(color[i].data());
			m_meshBoxFull[i].drawOriginalBox();

			if (m_meshBoxFull[i].boneType == SIDE_BONE)
			{
				glColor3fv(color[i].data());
				mirror.drawMirrorBox(m_meshBoxFull[i].curLeftDown, m_meshBoxFull[i].curRightUp);
			}
		}
	}

	// draw coord
	{
		arrayVec3i coords = dlg->coords;
		float length = 3;
		Vec3f RGBcolor[3] = { Vec3f(1, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1) };
		glBegin(GL_LINES);
		for (int i = 0; i < coords.size(); i++)
		{
			Vec3i cc = coords[i];
			Vec3f c = (m_meshBoxFull[i].curLeftDown + m_meshBoxFull[i].curRightUp) / 2;
			for (int j = 0; j < 3; j++)
			{
				if (cc[j] == -1)
				{
					continue;
				}

				glColor3fv(RGBcolor[j].data());
				glVertex3fv(c.data());
				glVertex3fv((c + RGBcolor[cc[j]]).data());

				if (m_meshBoxFull[i].boneType == SIDE_BONE)
				{
					Vec3f mm = mirror.mirror(c);
					glVertex3fv(mm.data());
					glVertex3fv((mm + (RGBcolor[cc[j]])*length).data());
				}
			}
		}
		glEnd();
	}
}

void coordAssignManager::drawBoneMap()
{
	arrayVec3i coords = dlg->coords;
	for (size_t i = 0; i < m_meshBoxFull.size(); i++)
	{
		bvhVoxel *m = &m_meshBoxFull[i];
		Vec3f sizeMesh = m->curRightUp - m->curLeftDown;

		Vec3i coordMap = coords[i];
		if (coordMap[0] == -1 || coordMap[1] == -1 || coordMap[2] == -1)
		{
			continue;
		}

		Vec3f sizeBoneMap;
		for (int j = 0; j < 3; j++)
		{
			sizeBoneMap[j] = sizeMesh[coordMap[j]];
		}

		// Fit size Bone to bone
		Vec3f sizeBone = m_boneFullArray[i]->m_sizef;
		float scalef = std::pow((sizeBone[0] * sizeBone[1] * sizeBone[2]) / (sizeBoneMap[0]*sizeBoneMap[1]*sizeBoneMap[2]), 1.0/3);

		for (int j = 0; j < 3; j++)
		{
			sizeBoneMap[j] *= scalef;
		}

		m_boneFullArray[i]->meshSizeScale = sizeBoneMap;
	}
}
