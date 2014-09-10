#include "StdAfx.h"
#include "errorHash.h"


errorHash::errorHash(void)
{
	m_hashTable = nullptr;
}


errorHash::~errorHash(void)
{
	if (m_hashTable)
	{
		for (int i = 0; i < m_tableSizei[0]; i++)
		{
			for (int j = 0; j < m_tableSizei[1]; j++)
			{
				delete []m_hashTable[i][j];
			}
			delete []m_hashTable[i];
		}
		delete []m_hashTable;
	}
}

void errorHash::initTable(Vec3i tableSize)
{
	m_tableSizei = tableSize;
	for (int i = 0; i < 3; i++)
	{
		m_distancef[i] = 1.0/(m_tableSizei[i]);
	}

	m_hashTable = new errorHashElement**[m_tableSizei[0]];
	for (int i = 0; i < m_tableSizei[0]; i++)
	{
		m_hashTable[i] = new errorHashElement *[m_tableSizei[1]];
		for (int j = 0; j < m_tableSizei[1]; j++)
		{
			m_hashTable[i][j] = new errorHashElement[m_tableSizei[1]];
		}
	}
}

void errorHash::addNode(cutTreefNode* node)
{
	Vec3i indexi = floatToIntIndex(Vec3f(node->neightborError, node->aspectError, node->volumeError));

	errorHashElement *e = &m_hashTable[indexi[0]][indexi[1]][indexi[2]];
	e->cutNodes.push_back(node);
}

cutTreefNode* errorHash::findSmallestNode(float neighborW, float aspectW, float volW)
{
	// Find the cell first
	Vec3i idxi;
	float smallest = MAX;
	errorHashElement* node = nullptr;

	for (int i = 0; i < m_tableSizei[0]; i++)
	{
		for (int j = 0; j < m_tableSizei[1]; j++)
		{
			for (int k = 0; k < m_tableSizei[2]; k++)
			{
				if (getNode(i,j,k)->cutNodes.size() == 0)
					continue;

				float err = neighborW*(i+0.5)*m_distancef[0] + 
					aspectW*(j+0.5)*m_distancef[1] + 
					volW*(k+0.5)*m_distancef[2];
				if (err < smallest)
				{
					node = getNode(i,j,k);
					smallest = err;
					idxi = Vec3i(i,j,k);
				}
			}
		}
	}

	if (node)
	{
		std::vector<cutTreefNode*> errNode = node->cutNodes;
		float sError = MAX;
		cutTreefNode* cutN = nullptr;
		for (int i = 0; i < errNode.size(); i++)
		{
			cutTreefNode* curNode = errNode[i];

			float err = neighborW*curNode->neightborError + 
				aspectW*curNode->aspectError + 
				volW*curNode->volumeError;

			if (err < sError)
			{
				sError = err;
				cutN = curNode;
			}
		}

		return cutN;
	}

	return nullptr;
}

cutTreefNode* errorHash::findSmallestNode(Vec3f errorW)
{
	return findSmallestNode(errorW[0], errorW[1], errorW[2]);
}

Vec3i errorHash::floatToIntIndex(Vec3f pt)
{
	Vec3i idx;
	for (int i = 0; i < 3; i++)
	{
		idx[i] = floor(pt[i]/m_distancef[i]);
	}
	return idx;
}

errorHashElement* errorHash::getNode(int i, int j, int k)
{
	return &m_hashTable[i][j][k];
}

errorHashElement* errorHash::getNode(Vec3i indexi)
{
	return &m_hashTable[indexi[0]][indexi[1]][indexi[2]];
}

errorHashElement::errorHashElement()
{

}

errorHashElement::~errorHashElement()
{

}
