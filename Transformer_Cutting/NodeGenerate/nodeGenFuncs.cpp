#include "StdAfx.h"
#include "MeshfreeNodeGenerator.h"
#include "nodeGenFuncs.h"
#include "Utility_wrap.h"
#include "triBoxTest.h"


nodeGeneration::nodeGeneration()
{
	m_surObj = nullptr;
}
// 
// nodeGenerate::nodeGeneration::~nodeGeneration()
// {
// 
// }

BOOL nodeGeneration::loadSurfaceSTL(char *filePath)
{
	// Read surface data
	m_surObj = std::shared_ptr<SurfaceObj>(new SurfaceObj);

	m_surObj->readObjDataSTL(filePath);
	m_surObj->constructAABBTree();

	return TRUE;
}

BOOL nodeGeneration::loadSurfaceSTLNoAABB(char *filePath)
{
	// Read surface data
	m_surObj = std::shared_ptr<SurfaceObj>(new SurfaceObj);

	m_surObj->readObjDataSTL(filePath);

	return TRUE;
}


BOOL nodeGeneration::generateUniformNode(int res)
{
	// Generate node
	MeshfreeNodeGenerator nodeGenerator;
	nodeGenerator.setSurfObj(m_surObj.get());

	//nodeGenerator.generateUniformNode(res);
	m_voxelSize = nodeGenerator.rayTracingToGenVoxel(res);

	// Receive information
	m_nodePos = *nodeGenerator.nodePos();
	return TRUE;
}

void nodeGeneration::drawSurface(int mode)
{
	if (!m_surObj)
	{
		return;
	}
	
	if (mode == 1 || mode == 3) // Solid surface
	{
		m_surObj->drawObject();
	}

	if (mode == 2 || mode == 3) // Wire frames
	{
		m_surObj->drawWireFrame();
	}
}

void nodeGeneration::drawNode(float radius)
{
	if (!m_surObj)
		return;

	if (radius == 0)
	{
		radius = m_voxelSize/4;
	}

	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();

	for (int i = 0; i < m_nodePos.size(); i++)
	{
		glPushMatrix();
		glTranslatef(m_nodePos[i][0], m_nodePos[i][1], m_nodePos[i][2]);
		gluSphere(qobj,radius,5,5);
		glPopMatrix();
	}

// 	Vec3f LeftDownPoint = m_surObj->getBVH()->root()->LeftDown;
// 	Vec3f RightUpPoint = m_surObj->getBVH()->root()->RightUp;
// 	Vec3i Pmin = Util_w::XYZ2IJK(LeftDownPoint, m_voxelSize);
// 	Vec3i Pmax = Util_w::XYZ2IJK(RightUpPoint, m_voxelSize);
// 
// 	glBegin(GL_LINES);
// 	Vec3f row(10,0,0);
// 	for (int j = Pmin[1]; j <= Pmax[1]; j++)
// 	{
// 		for (int k = Pmin[2]; k <= Pmax[2]; k++)
// 		{
// 			Vec3f pt = Util_w::IJK2XYZ(Vec3i(0,j,k), m_voxelSize);
// 
// 			glVertex3fv((pt-row).data());
// 			glVertex3fv((pt+row).data());
// 			
// 		}
// 	}
// 	glEnd();
}

void nodeGeneration::drawBVH()
{
	if (m_surObj && m_surObj->getBVH())
	{
		m_surObj->getBVH()->drawBoundingBox();
	}

}

void nodeGeneration::savePoint(char* filePath)
{
	FILE *f = fopen(filePath, "w");
	if (f)
	{
		fprintf(f, "%f\n", m_voxelSize);
		fprintf(f, "%d\n", m_nodePos.size());
		for (int i = 0; i < m_nodePos.size(); i++)
		{
			fprintf(f, "%lf %lf %lf\n", m_nodePos[i][0],  m_nodePos[i][1],  m_nodePos[i][2]);
		}
		fclose(f);
	}
}

void nodeGeneration::drawDebug(int yIdx, int zIdx)
{
	if (!m_surObj)
		return;

	Vec3f pt = Util_w::IJK2XYZ(Vec3i(0,yIdx, zIdx), m_voxelSize);

	Vec3f LeftDownPoint = m_surObj->getBVH()->root()->LeftDown;
	Vec3f RightUpPoint = m_surObj->getBVH()->root()->RightUp;
	Vec3f direct(RightUpPoint[0]-LeftDownPoint[0],0,0);

	glBegin(GL_LINES);
	glVertex3fv((pt-direct).data());
	glVertex3fv((pt+direct).data());
	glEnd();
}

void nodeGeneration::voxelFillObject(float voxelSize)
{
	
	m_voxelSize = voxelSize;
	std::vector<Vec3f> *points = m_surObj->point();
	std::vector<Vec3i> *tris = m_surObj->face();

	// 1. Init voxel array
	Vec3f LeftDownPoint(MAX, MAX, MAX);
	Vec3f RightUpPoint(MIN, MIN, MIN);

	for (int i = 0; i < points->size(); i++)
	{
		for (int j =0; j< 3; j++)
		{
			if (LeftDownPoint[j] > points->at(i)[j])
				LeftDownPoint[j] = points->at(i)[j];

			if (RightUpPoint[j] < points->at(i)[j])
				RightUpPoint[j] = points->at(i)[j];
		}
	}

	leftDowni = Util_w::XYZ2IJK(LeftDownPoint, m_voxelSize);
	rightUpi = Util_w::XYZ2IJK(RightUpPoint, m_voxelSize);
	sizei = rightUpi - leftDowni + Vec3i(1,1,1);

	vTable = new int **[sizei[0]];
	for (size_t i = 0; i < sizei[0]; i++)
	{
		vTable[i] = new int *[sizei[1]];
		for (size_t j = 0; j < sizei[1]; j++)
		{
			vTable[i][j] = new int[sizei[2]];
			std::fill(vTable[i][j], vTable[i][j] + sizei[2], 0);
		}
	}

	// 2. Voxelize surface
	for (int i = 0; i < tris->size(); i++)
	{
		Vec3i cTri = tris->at(i);
		// Potential voxels
		Vec3f ld(MAX, MAX, MAX);
		Vec3f ru(MIN, MIN, MIN);
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (ld[k] > points->at(cTri[j])[k])
					ld[k] = points->at(cTri[j])[k];

				if (ru[k] < points->at(cTri[j])[k])
					ru[k] = points->at(cTri[j])[k];
			}
		}
		Vec3i ldi = Util_w::XYZ2IJK(ld, m_voxelSize);
		Vec3i rui = Util_w::XYZ2IJK(ru, m_voxelSize);
		// Check all potential voxel
		for (int ii = ldi[0]; ii <= rui[0]; ii++)
		{
			for (int jj = ldi[1]; jj <= rui[1]; jj++)
			{
				for (int kk = ldi[2]; kk <= rui[2]; kk++)
				{
					if (getTableAt(ii,jj,kk) == 0)
					{
						if (voxelAndTriIntersect(points, cTri, Vec3i(ii,jj,kk), m_voxelSize) == 1)
						{
							setTableAt(ii,jj,kk,1);
						}
					}
				}
			}
		}
	}


	// 3. Flood fill the outside of the box (Since the object may contains multiple hollow space
	lineFillObject();
}

bool nodeGeneration::voxelAndTriIntersect(std::vector<Vec3f> * points, Vec3i cTri, Vec3i voxelPt, float voxelSize)
{
	Vec3f centerB = Util_w::IJK2XYZ(voxelPt, voxelSize);

	float triVert[3][3];
	float centerBox[3];
	float boxHalfSize[3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			triVert[i][j] = points->at(cTri[i])[j];
		}
		centerBox[i] = centerB[i];
		boxHalfSize[i] = voxelSize/2.;
	}

	return triboxTest::triBoxOverlap(centerBox, boxHalfSize, triVert);
}

int nodeGeneration::getTableAt(int i, int j, int k)
{
	Vec3i offsetI = Vec3i(i,j,k) - leftDowni;
	
	if (offsetI[X] < 0 || offsetI[Y] < 0 || offsetI[Z] < 0
		|| offsetI[X] >= sizei[X] || offsetI[Y] >= sizei[Y] || offsetI[Z] >= sizei[Z])
	{
		return 0;
	}

	return vTable[offsetI[X]][offsetI[Y]][offsetI[Z]];
}

void nodeGeneration::setTableAt(int i, int j, int k, int val)
{
	Vec3i offsetI = Vec3i(i,j,k) - leftDowni;

	ASSERT (!(offsetI[X] < 0 || offsetI[Y] < 0 || offsetI[Z] < 0
		|| offsetI[X] >= sizei[X] || offsetI[Y] >= sizei[Y] || offsetI[Z] >= sizei[Z]));

	vTable[offsetI[0]][offsetI[1]][offsetI[2]] = val;
}

void nodeGeneration::floodFillOutside(Vec3i pt)
{
	if (element(vTable, pt) == 0) 
		element(vTable, pt) = -1;

	for (int i = 0; i < 3; i ++)
	{
		Vec3i pt1 = pt, pt2 = pt;

		if (pt1[i] > 0)
		{
			pt1[i] --;
			if (element(vTable, pt1) == 0)
				floodFillOutside(pt1);
		}
		
		if (pt2[i] < sizei[i]-1)
		{
			pt2[i] ++;
			if (element(vTable, pt2) == 0)
				floodFillOutside(pt2);
		}
	}
}

void nodeGeneration::discretizeFloatPoints()
{
	Vec3f leftDown = m_surObj->getBVH()->root()->LeftDown;
	Vec3f righUp = m_surObj->getBVH()->root()->RightUp;

	leftDowni = Util_w::XYZ2IJK(leftDown, m_voxelSize);
	rightUpi = Util_w::XYZ2IJK(righUp, m_voxelSize);

	sizei = rightUpi - leftDowni + Vec3i(1,1,1);

	vTable = new int **[sizei[0]];
	for (size_t i = 0; i < sizei[0]; i++)
	{
		vTable[i] = new int *[sizei[1]];
		for (size_t j = 0; j < sizei[1]; j++)
		{
			vTable[i][j] = new int[sizei[2]];
			std::fill(vTable[i][j], vTable[i][j] + sizei[2], 0);
		}
	}

	for (int i = 0; i < m_nodePos.size(); i++)
	{
		Vec3i pti = Util_w::XYZ2IJK(m_nodePos[i], m_voxelSize);
		setTableAt(pti[0], pti[1], pti[2], 1);
	}
}

void nodeGeneration::lineFillObject()
{
	for (int i = 0; i < sizei[0]; i++)
	{
		for (int j = 0; j < sizei[1]; j++)
		{
			int state = 0; // outside
			for (int k = 0; k < sizei[2]; k++)
			{
				if (state == 0)
				{
					if (vTable[i][j][k] == 1) // Consider change state
					{
						// Check 
					}
				}
			}
		}
	}
}

void nodeGeneration::addSurfaceIntersectionVoxel()
{
	std::vector<Vec3f> *points = m_surObj->point();
	std::vector<Vec3i> *tris = m_surObj->face();

	for (int i = 0; i < tris->size(); i++)
	{
		Vec3i cTri = tris->at(i);
		// Potential voxels
		Vec3f ld(MAX, MAX, MAX);
		Vec3f ru(MIN, MIN, MIN);
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (ld[k] > points->at(cTri[j])[k])
					ld[k] = points->at(cTri[j])[k];

				if (ru[k] < points->at(cTri[j])[k])
					ru[k] = points->at(cTri[j])[k];
			}
		}
		Vec3i ldi = Util_w::XYZ2IJK(ld, m_voxelSize);
		Vec3i rui = Util_w::XYZ2IJK(ru, m_voxelSize);
		// Check all potential voxel
		for (int ii = ldi[0]; ii <= rui[0]; ii++)
		{
			for (int jj = ldi[1]; jj <= rui[1]; jj++)
			{
				for (int kk = ldi[2]; kk <= rui[2]; kk++)
				{
					if (getTableAt(ii,jj,kk) == 0)
					{
						if (voxelAndTriIntersect(points, cTri, Vec3i(ii,jj,kk), m_voxelSize) == 1)
						{
							setTableAt(ii,jj,kk,1);
						}
					}
				}
			}
		}
	}
}


