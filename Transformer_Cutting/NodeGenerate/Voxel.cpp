// Voxel.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Voxel.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include "Utility_wrap.h"
#include "nodeGenFuncs.h"


VoxelObj::VoxelObj()
{
	hashTable = NULL;
	m_voxelArray = nullptr;
}

VoxelObj::~VoxelObj()
{
	if (hashTable)
	{
		delete hashTable;
	}
	if (m_voxelArray)
	{
		for (int i = 0; i<m_sizei[0]; i++)
		{
			for (int j = 0; j<m_sizei[1]; j++)
			{
				delete []m_voxelArray[i][j];
			}
			delete []m_voxelArray[i];
		}
		delete []m_voxelArray;
	}
}

void VoxelObj::draw(drawMode mode)
{
// 	for (int i = 0; i < m_cubes.size(); i++)
// 	{
// 		m_cubes[i].draw(mode);
// 	}

	if (mode & DRAW_WIRE)
	{
		for (int i =0; i<m_points.size(); i++)
		{
			Vec3f pt = Util_w::IJK2XYZ(m_points[i], m_voxelSize);
			Util::drawBoxWire(pt[X], pt[Y], pt[Z], m_voxelSize/2.);
		}
	}

	if(mode & DRAW_SURFACE)
	{
		for (int i =0; i<m_points.size(); i++)
		{
			Vec3f pt = Util_w::IJK2XYZ(m_points[i], m_voxelSize);
			Util::drawBoxSurface(pt[X], pt[Y], pt[Z], m_voxelSize/2.);
		}
	}

}

void VoxelObj::loadVoxelFromFileFloat(char* filePath)
{
	// Load unordered points
	FILE* f = fopen(filePath, "r");
	ASSERT(f);

	int nbPoint = 0;
	fscanf(f, "%f\n", &m_voxelSize);
	fscanf(f, "%d\n", &nbPoint);

	for (int i = 0; i < nbPoint; i++)
	{
		float x,y,z;
		fscanf(f, "%f %f %f\n", &x, &y, &z);
		m_points.push_back(Util_w::XYZ2IJK(Vec3f(x,y,z), m_voxelSize));
	}
	fclose(f);

	// Bounding box
	Vec3i leftDown(VOXEL_MAX, VOXEL_MAX, VOXEL_MAX);
	Vec3i rightUp(VOXEL_MIN, VOXEL_MIN, VOXEL_MIN);
	for (size_t i = 0; i < m_points.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (leftDown[j] > m_points[i][j])
				leftDown[j] = m_points[i][j];
			if (rightUp[j] < m_points[i][j])
				rightUp[j] = m_points[i][j];
		}
	}

	// Build sum table
	m_sumTable.initSumTable(this);
}

BOOL VoxelObj::intersectionWithBox(Vec3f leftDown, Vec3f rightUp, std::vector<int> &intersectedIdxs)
{
	return intersectionWithBox(Util_w::XYZ2IJK(leftDown, m_voxelSize), Util_w::XYZ2IJK(rightUp, m_voxelSize), intersectedIdxs);
}

BOOL VoxelObj::intersectionWithBox(Vec3i leftDown, Vec3i rightUp, std::vector<int> &intersectedIdxs)
{
	for (int i = leftDown[0]; i <= rightUp[0]; i++)
	{
		for (int j = leftDown[1]; j <= rightUp[1]; j++)
		{
			for (int k = leftDown[2]; k <= rightUp[2]; k++)
			{
				int idx = hashTable->intersectWithCube(Vec3i(i,j,k));
				if (idx != INVALID_IDX)
				{
					intersectedIdxs.push_back(idx);
				}
			}
		}
	}
	return intersectedIdxs.size() > 0;
}

void VoxelObj::generateVoxelFromMesh_1(char* filePath, float res)
{
	m_sufObj.readObjDataSTL(filePath);
	m_voxelSize = res; // Optimized base on bones

	// Generate node
	nodeGeneration nodeGen;
	nodeGen.loadSurfaceSTLNoAABB(filePath);
	nodeGen.voxelFillObject(m_voxelSize);

	m_leftDown = nodeGen.leftDowni;
	m_rightUp = nodeGen.rightUpi;

	m_sumTable.initFromNode(nodeGen);
	getPointFromSumtable();

	m_sumTable.computeSum();
}

void VoxelObj::getPointFromSumtable()
{
	// testing
	m_sizei = m_sumTable.m_tableSize;
	m_leftDown = m_sumTable.m_leftDown;
	m_rightUp = m_leftDown + m_sizei - Vec3i(1,1,1);
	m_voxelArray = Util::cloneArray(m_sumTable.m_table, m_sizei[0], m_sizei[1], m_sizei[2]);

	for (int i = 0; i < m_sizei[0]; i++)
	{
 		for (int j = 0; j < m_sizei[1]; j++)
 		{
			for (int k = 0; k < m_sizei[2]; k++)
			{
				if (m_sumTable.sumValueAt(i,j,k) != 0)
				{
					m_points.push_back(Vec3i(i,j,k)+m_leftDown);
				}
 			}
 		}
	}
}

void VoxelObj::drawMesh()
{
	glColor3f(1,0,1);
	m_sufObj.drawWireFrame();
//	m_sufObj.drawObject();
}

void VoxelObj::drawVoxelIdx()
{
	glPointSize(2);
	glBegin(GL_POINTS);
	for (int i =0; i<m_points.size(); i++)
	{
		Vec3f pt = Util_w::IJK2XYZ(m_points[i], m_voxelSize);
		glVertex3fv(pt.data());
	}
	glEnd();

	for (int i =0; i<m_points.size(); i++)
	{
		Vec3f pt = Util_w::IJK2XYZ(m_points[i], m_voxelSize);
		Util::printw(pt[0], pt[1], pt[2], " %d", i);
	}
}

void VoxelObj::drawTable()
{
	Vec3i sizei = m_sumTable.m_tableSize;
	int ***table = m_sumTable.m_table;

	for (int i = 0; i<sizei[0]; i++)
	{
		for (int j = 0; j<sizei[1]; j++)
		{
			for (int k = 0; k < sizei[2]; k++)
			{
				Vec3f pt = Util_w::IJK2XYZ(m_leftDown + Vec3i(i,j,k), m_voxelSize);
				glColor3f(0,0,1);
				Util_w::drawBoxWire(pt, m_voxelSize/2.);

				Util::printw(pt[0], pt[1], pt[2], "%d", table[i][j][k]);
			}
		}
	}
}

void VoxelObj::generateVoxelFromMesh(char* filePath, int res)
{
	m_sufObj.readObjDataSTL(filePath);
	m_voxelSize = res; // Optimized base on bones

	// Generate node
	nodeGeneration nodeGen;
	nodeGen.loadSurfaceSTL(filePath);
	nodeGen.generateUniformNode(res);
	nodeGen.discretizeFloatPoints();
	nodeGen.addSurfaceIntersectionVoxel();
	m_voxelSize = nodeGen.m_voxelSize;

	m_leftDown = nodeGen.leftDowni;
	m_rightUp = nodeGen.rightUpi;

	m_sumTable.initFromNode(nodeGen);
	getPointFromSumtable();

	m_sumTable.computeSum();
}

void VoxelObj::writeVoxelData(char* filePath)
{
	FILE *f = fopen(filePath, "w");
	ASSERT(f);

	fprintf(f, "%d %d %d\n", m_leftDown[0], m_leftDown[1], m_leftDown[2]);
	fprintf(f, "%d %d %d\n", m_sizei[0], m_sizei[1], m_sizei[2]);
	fprintf(f, "%lf\n", m_voxelSize);

	for (int i = 0; i < m_sizei[0]; i++)
	{
		for (int j = 0; j < m_sizei[1]; j++)
		{
			for (int k = 0; k < m_sizei[2]; k++)
			{
				int kk = m_voxelArray[i][j][k];
				fprintf(f, "%d ", m_voxelArray[i][j][k]);
			}
			fprintf(f, "\n");
		}
	}

	fclose(f);
}

void VoxelObj::readVoxelData(char* filePath)
{
	m_voxelSize = m_sumTable.readVoxelData(filePath);
	getPointFromSumtable();
	m_sumTable.computeSum();
}

int VoxelObj::volumeInBox(Vec3i leftDown, Vec3i rightUp)
{
	return m_sumTable.volumeInBox(leftDown, rightUp);
}

int VoxelObj::objectVolumei()
{
	return volumeInBox(m_leftDown, m_rightUp);
}
