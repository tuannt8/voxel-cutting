#include "stdafx.h"
#include "sumTable.h"
#include "Voxel.h"
#include "vec.h"


sumTable::sumTable()
{
	m_table = nullptr;
}

sumTable::~sumTable()
{
	if (m_table)
	{
		for (int i = 0; i<m_tableSize[0]; i++)
		{
			for (int j = 0; j<m_tableSize[1]; j++)
			{
				delete []m_table[i][j];
			}
			delete []m_table[i];
		}
		delete []m_table;
	}
}

void sumTable::initSumTable(VoxelObj *obj)
{
	m_leftDown = obj->m_leftDown;
	arrayVec3i points = obj->points();

	// Init array
	Vec3i boxSize = obj->m_sizei;
	m_tableSize = boxSize + Vec3i(1,1,1);
	m_table = new int **[m_tableSize[0]];
	for (int i = 0; i < m_tableSize[0]; i++)
	{
		m_table[i] = new int *[m_tableSize[1]];
		for (int j = 0; j < m_tableSize[1]; j++)
		{
			m_table[i][j] = new int[m_tableSize[2]];
			std::fill(m_table[i][j], m_table[i][j] + m_tableSize[2], ZERO_INT);
		}
	}

	// Assign occupied voxels
	for (int i = 0; i < points.size(); i++)
	{
		Vec3i curPt = points[i];
		Vec3i idxInTable = curPt - m_leftDown;
		setSumValue(idxInTable, 1);
	}


}

int sumTable::sumValueAt(int i, int j, int k)
{
	if (i < 0 || j < 0 || k < 0)
		return 0;

	if (i >= m_tableSize[0])
		i = m_tableSize[0]-1;
	if (j >= m_tableSize[1])
		j = m_tableSize[1]-1;
	if (k >= m_tableSize[2])
		k = m_tableSize[2]-1;

	return m_table[i][j][k];
}

void sumTable::setSumValue(int i, int j, int k, int val)
{
	ASSERT(i >= 0 && j >= 0 && k >= 0);
	m_table[i][j][k] = val;
}

void sumTable::setSumValue(Vec3i p, int val)
{
	setSumValue(p[0], p[1], p[2], val);
}

int sumTable::volumeInBox(Boxi b)
{
	return volumeInBox(b.leftDown, b.rightUp);
}

int sumTable::volumeInBox(Vec3i leftDown, Vec3i rightUp)
{
	Vec3i L = rightUp - m_leftDown;
	Vec3i o = leftDown - Vec3i(1,1,1)- m_leftDown;

	int a111 = sumValueAt(L);
	int a011 = sumValueAt(o[0], L[1], L[2]);
	int a101 = sumValueAt(L[0], o[1], L[2]);
	int a110 = sumValueAt(L[0], L[1], o[2]);
	int a100 = sumValueAt(L[0], o[1], o[2]);
	int a010 = sumValueAt(o[0], L[1], o[2]);
	int a001 = sumValueAt(o[0], o[1], L[2]);
	int a000 = sumValueAt(o[0], o[1], o[2]);

	int vol = a111 + a100 + a010 + a001 - a110 - a101 - a011 - a000;
	return vol;
}

int sumTable::sumValueAt(Vec3i p)
{
	return sumValueAt(p[0], p[1], p[2]);
}

void sumTable::computeSum()
{
	// Init sum 
	for (int i = 0; i < m_tableSize[0]; i++)
	{
		for (int j = 0; j < m_tableSize[1]; j++)
		{
			for (int k = 0; k < m_tableSize[2]; k++)
			{
				// Be sure that all element with smaller index are computed
				Vec3i L = Vec3i(i,j,k);
				Vec3i o = L - Vec3i(1,1,1);

				int volume = sumValueAt(L); // volume, old value
				int a011 = sumValueAt(o[0], L[1], L[2]);
				int a101 = sumValueAt(L[0], o[1], L[2]);
				int a110 = sumValueAt(L[0], L[1], o[2]);
				int a100 = sumValueAt(L[0], o[1], o[2]);
				int a010 = sumValueAt(o[0], L[1], o[2]);
				int a001 = sumValueAt(o[0], o[1], L[2]);
				int a000 = sumValueAt(o[0], o[1], o[2]);

				int a111 = volume + a110 + a101 + a011 + a000 - a100 - a010 - a001;

				setSumValue(i,j,k, a111);
			}
		}
	}
}

void sumTable::initFromNode(nodeGeneration &nodeGen)
{
	m_tableSize = nodeGen.sizei;
	m_table = nodeGen.vTable;
	m_leftDown = nodeGen.leftDowni;
}

float sumTable::readVoxelData(char* filePath)
{
	FILE *f = fopen(filePath, "r");
	ASSERT(f);

	fscanf(f, "%d %d %d\n", &m_leftDown[0], &m_leftDown[1], &m_leftDown[2]);
	fscanf(f, "%d %d %d\n", &m_tableSize[0], &m_tableSize[1], &m_tableSize[2]);
	float voxelSize;
	fscanf(f, "%f", &voxelSize);

	// Init pointer
	m_table = new int **[m_tableSize[0]];
	for (int i = 0; i < m_tableSize[0]; i++)
	{
		m_table[i] = new int *[m_tableSize[1]];
		for (int j = 0; j < m_tableSize[1]; j++)
		{
			m_table[i][j] = new int[m_tableSize[2]];
			std::fill(m_table[i][j], m_table[i][j] + m_tableSize[2], ZERO_INT);
		}
	}

	// Load value
	for (int i = 0; i < m_tableSize[0]; i++)
	{
		for (int j = 0; j < m_tableSize[1]; j++)
		{
			for (int k = 0; k < m_tableSize[2]; k++)
			{
				fscanf(f, "%d ", &m_table[i][j][k]);
			}
		}
	}

	fclose(f);

	return voxelSize;
}


