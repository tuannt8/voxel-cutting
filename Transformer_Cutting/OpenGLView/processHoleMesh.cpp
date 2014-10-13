#include "stdafx.h"
#include "processHoleMesh.h"
#include <queue>
#include "Utility_wrap.h"

using namespace std;

processHoleMesh::processHoleMesh()
{
}


processHoleMesh::~processHoleMesh()
{
}

void processHoleMesh::processMeshSTL(char* path)
{
	originalSurface = SurfaceObjPtr(new SurfaceObj);
	originalSurface->readObjDataSTL(path);

	TopologyContainer * topo = originalSurface->container();
	arrayVec3i * tris = originalSurface->face();
	vector<arrayInt> * faceAroundEdge = topo->facesAroundEdge();
	
	// Hash 
	arrayInt faceHash(tris->size(), 0);

	arrayInt remainTriIdx;
	for (int i = 0; i < tris->size(); i++)
	{
		remainTriIdx.push_back(i);
	}

	// Analyze the mesh
	independentObj.clear();
	while (remainTriIdx.size() > 0)
	{
		int idx = remainTriIdx[0];
		queue<int> qIdxs;
		qIdxs.push(idx);

		arrayInt newObjIdxs;
		while (!qIdxs.empty())
		{
			int idxInQ = qIdxs.front();
			qIdxs.pop();

			newObjIdxs.push_back(idxInQ);
			faceHash[idxInQ] = 1;

			arrayInt nbIdx = topo->faceShareEdgeWithFace(idxInQ); // Neighbor share edege with this current triangle
			for (auto fIdx : nbIdx)
			{
				if (faceHash[fIdx] == 0)
				{
					qIdxs.push(fIdx);
					faceHash[fIdx] = 1;
				}
			}
		}

		independentObj.push_back(newObjIdxs);
		remainTriIdx = Util_w::substractArrayInt(remainTriIdx, newObjIdxs, tris->size());
	}

	// Water tight
	std::vector<arrayInt> *edgeOnFace = originalSurface->container()->edgesInFace();
	for (auto idxs : independentObj)
	{
		bool isWaterTight = true;
		for (auto id: idxs)
		{
			arrayInt edges = edgeOnFace->at(id);
			for (auto eIdx : edges)
			{
				if (faceAroundEdge->at(eIdx).size() != 2)
				{
					isWaterTight = false;
					break;
				}
			}
			if (!isWaterTight)
			{
				break;
			}
		}

		isWaterTightArray.push_back(isWaterTight);
	}
}

SurfaceObj * processHoleMesh::getBiggestWaterTightPart()
{
	int idxBig = -1;
	int biggestSize = 0;
	for (int i = 0; i < independentObj.size(); i++)
	{
		if (isWaterTightArray[i])
		{
			if (independentObj[i].size() > biggestSize)
			{
				biggestSize = independentObj[i].size();
				idxBig = i;
			}
		}
	}

	if (idxBig != -1)
	{
		arrayVec3f * pts = originalSurface->point();
		arrayVec3i * tris = originalSurface->face();
		arrayInt idxs = independentObj[idxBig];

		arrayVec3i newTris;
		for (auto id : idxs)
		{
			newTris.push_back(tris->at(id));
		}

		// Create a new surface
		return new SurfaceObj(*pts, newTris);
	}



	return nullptr;
}

void processHoleMesh::drawSeparatePart() const
{
	if (!originalSurface)
		return;

	static arrayVec3f colors = Util_w::randColor(20);
	for (int i = 0; i < independentObj.size(); i++)
	{
		arrayInt a = independentObj[i];
		glColor3fv(colors[i].data());
		Util_w::drawTriFace(originalSurface->point(), a, originalSurface->face());
	}

	glColor3fv(colors[19].data());
	Util_w::drawTriWire(originalSurface->point(), originalSurface->face());
}
