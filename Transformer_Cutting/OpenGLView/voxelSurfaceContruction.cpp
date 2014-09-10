#include "stdafx.h"
#include "voxelSurfaceContruction.h"


voxelSurfaceContruction::voxelSurfaceContruction()
{
}


voxelSurfaceContruction::~voxelSurfaceContruction()
{
}

void voxelSurfaceContruction::getSurface(voxelObject *s_obj, arrayInt vIdxs, arrayVec3f &point, arrayVec3i &faces)
{
	s_hashTable = &s_obj->m_hashTable;
	s_boxes = &s_obj->m_boxes;

	// The point also need and hash table
	pointHash hashPtf;
	Vec3f basePointHash = s_hashTable->leftDown;
	float voxelSize = s_hashTable->voxelSize;
	hashPtf.basePt = basePointHash - Vec3f(voxelSize, voxelSize, voxelSize) / 2;
	hashPtf.sizef = s_obj->m_voxelSizef;
	hashPtf.NumXYZ = s_hashTable->NumXYZ + Vec3i(1, 1, 1);
	mapii hashPtIdx;

	// Loop through voxel
	for (int i = 0; i < vIdxs.size(); i++)
	{
		int curIdx = vIdxs[i];
		voxelBox curB = s_boxes->at(curIdx);
		Vec3i posi = curB.xyzIndex;
		int boneIdx = curB.boneIndex;

		// Check every faces
		for (int j = 0; j < DIRECT_TOTAL; j++)
		{
			Vec3i neighbori = getNeighborPos(posi, (direction)j);
			int idx = s_hashTable->getBoxIndexFromVoxelCoord(neighbori);
			if (idx == -1
				|| (s_boxes->at(idx).boneIndex != boneIdx))
			{
				arrayVec3f pointQuad = getQuadFace(curB.leftDown, curB.rightUp, (direction)j);

				mapii vertexIdxMap;
				for (int k = 0; k < 4; k++)
				{
					int h = hashPtf.getHash(pointQuad[k]);
					if (hashPtIdx.find(h) == hashPtIdx.end())
					{
						point.push_back(pointQuad[k]);
						hashPtIdx.insert(pairii(h, point.size() - 1));
					}

					vertexIdxMap.insert(pairii(k, hashPtIdx[h]));
				}

				faces.push_back(Vec3i(vertexIdxMap[0], vertexIdxMap[1], vertexIdxMap[2]));
				faces.push_back(Vec3i(vertexIdxMap[0], vertexIdxMap[2], vertexIdxMap[3]));
			}
		}
	}
}

Vec3i voxelSurfaceContruction::getNeighborPos(Vec3i posi, direction d)
{
	Vec3i nPosi = posi;

	switch (d)
	{
	case voxelSurfaceContruction::X_MINUS:
		nPosi[0]--;
		break;
	case voxelSurfaceContruction::X_PLUS:
		nPosi[0]++;
		break;
	case voxelSurfaceContruction::Y_MINUS:
		nPosi[1]--;
		break;
	case voxelSurfaceContruction::Y_PLUS:
		nPosi[1]++;
		break;
	case voxelSurfaceContruction::Z_MINUS:
		nPosi[2]--;
		break;
	case voxelSurfaceContruction::Z_PLUS:
		nPosi[2]++;
		break;
	default:
		ASSERT(0);
	}

	return nPosi;
}

_inline Vec3f Vec3fL(Vec3f a, Vec3f b, Vec3f c){
	return Vec3f(a[0], b[1], c[2]);
}

arrayVec3f voxelSurfaceContruction::getQuadFace(Vec3f leftDown, Vec3f rightUp, direction d)
{
	arrayVec3f pts;

	Vec3f U = rightUp, o = leftDown;

	arrayVec3f _points;
	_points.push_back(Vec3fL(o, o, o));
	_points.push_back(Vec3fL(o, U, o));
	_points.push_back(Vec3fL(U, U, o));
	_points.push_back(Vec3fL(U, o, o));
	_points.push_back(Vec3fL(o, o, U));
	_points.push_back(Vec3fL(o, U, U));
	_points.push_back(Vec3fL(U, U, U));
	_points.push_back(Vec3fL(U, o, U));

	Vec4i faces;
	switch (d)
	{
	case voxelSurfaceContruction::X_MINUS:
		faces = Vec4i(0, 4, 5, 1);
		break;
	case voxelSurfaceContruction::X_PLUS:
		faces = Vec4i(2, 6, 7, 3);
		break;
	case voxelSurfaceContruction::Y_MINUS:
		faces = Vec4i(0, 3, 7, 4);
		break;
	case voxelSurfaceContruction::Y_PLUS:
		faces = Vec4i(1, 5, 6, 2);
		break;
	case voxelSurfaceContruction::Z_MINUS:
		faces = Vec4i(0, 1, 2, 3);
		break;
	case voxelSurfaceContruction::Z_PLUS:
		faces = Vec4i(4, 7, 6, 5);
		break;
	default:
		ASSERT(0);
		break;
	}

	for (int i = 0; i < 4; i++)
	{
		pts.push_back(_points[faces[i]]);
	}

	return pts;
}

void voxelSurfaceContruction::shrinkPoint(voxelObject * s_voxelObj, arrayVec3f& pts, float shrinkRatio)
{
	hashVoxel *hashTable = &s_voxelObj->m_hashTable;
	std::vector<voxelBox> * boxes = &s_voxelObj->m_boxes;
	float voxelSize = hashTable->voxelSize;
	for (int i = 0; i < pts.size(); i++)
	{
		arrayInt voxelAround = getVoxelAroundPoint(pts[i], hashTable);
		Vec3f direct;
		for (int j = 0; j < voxelAround.size(); j++)
		{
			Vec3f vPt = boxes->at(voxelAround[j]).center;
			direct = direct + (vPt - pts[i]);
		}

		if (direct.norm() > 0.01*voxelSize*voxelSize)
		{
			direct.normalize();
			pts[i] = pts[i] - direct*voxelSize*shrinkRatio;
		}
	}
}

arrayInt voxelSurfaceContruction::getVoxelAroundPoint(Vec3f pt, hashVoxel * hashTable)
{
	float voxelSize = hashTable->voxelSize;

	// There are 8 voxel around
	arrayVec3f centerVoxel = pointAroundPoint8(pt, voxelSize/2);
	arrayInt idxs;
	for (int i = 0; i < centerVoxel.size(); i++)
	{
		int idx = hashTable->getBoxIndexFromCoord(centerVoxel[i]);
		if (idx != -1)
		{
			idxs.push_back(idx);
		}
	}

	return idxs;
}

arrayVec3f voxelSurfaceContruction::pointAroundPoint8(Vec3f pt, double dis)
{
	arrayVec3f centerVoxel;
	centerVoxel.resize(8);
	centerVoxel[0] = pt + Vec3f(dis, dis, dis);
	centerVoxel[1] = pt + Vec3f(dis, dis, -dis);
	centerVoxel[2] = pt + Vec3f(dis, -dis, dis);
	centerVoxel[3] = pt + Vec3f(dis, -dis, -dis);
	centerVoxel[4] = pt + Vec3f(-dis, dis, dis);
	centerVoxel[5] = pt + Vec3f(-dis, dis, -dis);
	centerVoxel[6] = pt + Vec3f(-dis, -dis, dis);
	centerVoxel[7] = pt + Vec3f(-dis, -dis, -dis);

	return centerVoxel;
}

int voxelSurfaceContruction::pointHash::getHash(Vec3f pt)
{
	Vec3f rPtf = pt - basePt;
	Vec3i pti = Util_w::XYZ2IJK(rPtf, sizef);
	return pti[0] * NumXYZ[1] * NumXYZ[2] + pti[1] * NumXYZ[2] + pti[2];
}
