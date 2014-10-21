#include "StdAfx.h"
#include "voxelObject.h"
#include "Utility_wrap.h"


voxelBox::voxelBox()
{
	boneIndex = -1;
	state = -1;
}

voxelBox::voxelBox(Vec3f ld, Vec3f ru)
: voxelBox()
{
	leftDown = ld;
	rightUp = ru;
	center = (leftDown+rightUp)/2;

	boneIndex = -1;
}

voxelBox::~voxelBox()
{

}

void voxelBox::draw(int mode)
{
	if (mode == 0) // draw edge
	{
		Util_w::drawBoxWireFrame(leftDown, rightUp);
	}
	if (mode == 1) // draw solid box
	{
		Util_w::drawBoxSurface(leftDown, rightUp);
	}
}

hashVoxel::~hashVoxel()
{

}

int hashVoxel::getBoxIndexFromCoord(Vec3f coordf)
{
	Vec3i xyzIdx = getVoxelCoord(coordf);
	return getBoxIndexFromVoxelCoord(xyzIdx);
}

int hashVoxel::getBoxIndexFromCoord(Vec3f leftDown, Vec3f rightUp)
{
	return getBoxIndexFromCoord((leftDown+rightUp)/2);
}

Vec3i hashVoxel::getVoxelCoord(Vec3f coordf)
{
	return Util_w::XYZ2IJK(coordf-leftDown, voxelSize);
}

hashVoxel::hashVoxel()
{

}

int hashVoxel::getBoxIndexFromVoxelCoord(Vec3i coordi)
{
	for (int i = 0; i < 3; i++)
	{
		if (coordi[i] < 0 || coordi[i] >= NumXYZ[i])
		{
			return -1;
		}
	}

	int hashF = coordi[2]*NumXYZ[0]*NumXYZ[1] + coordi[1]*NumXYZ[0] + coordi[0];

	if (hashF < 0 || hashF >= voxelHash.size())
	{
		return -1;
	}

	return voxelHash[hashF];
}

int hashVoxel::getSymmetricBox(int idx)
{
	voxelBox vOrigin = boxes->at(idx);
	Vec3f centerOri = vOrigin.center;
	Vec3f centerSym = centerOri;

	// Symmetric through X axis
	Vec3f centerPoint = (leftDown + rightUp)/2;
	centerSym[0] = 2*centerPoint[0] - centerSym[0];

	return getBoxIndexFromCoord(centerSym);
}

std::vector<int> hashVoxel::getNeighbor(int idx)
{
	static bool inited = false;
	if (!inited)
	{
		inited = true;
		neighborOfBox.resize(boxes->size());

		for(int i = 0; i < NumXYZ[0]; i++)
		{
			for (int j = 0; j < NumXYZ[1]; j++)
			{
				for(int k = 0; k < NumXYZ[2]; k++)
				{
					int idx = getBoxIndexFromVoxelCoord(Vec3i(i,j,k));
					if (idx != -1)
					{
						for (int xx = 0; xx < 2; xx++)
						{
							for (int yy = 0; yy < 2; yy++)
							{
								for (int zz = 0; zz < 2; zz++)
								{
									int idxN = getBoxIndexFromVoxelCoord(Vec3i(i+xx, j+yy, k+zz));
									if (idxN != -1 && idx != idxN)
									{
										neighborOfBox[idx].push_back(idxN);
										neighborOfBox[idxN].push_back(idx);
									}
								}
							}
						}
					}
				}
			}
		}
	}


	return neighborOfBox[idx];

}

Vec3f hashVoxel::IJK2XYZ(Vec3i voxelCi)
{
	return leftDown + Util_w::IJK2XYZ(voxelCi, voxelSize);
}

Vec3f hashVoxel::IJK2XYZLower(Vec3i coordi)
{
	Vec3f pt = IJK2XYZ(coordi);
	return pt - Vec3f(voxelSize, voxelSize, voxelSize) / 2;
}

Vec3f hashVoxel::IJK2XYZUpper(Vec3i coordi)
{
	Vec3f pt = IJK2XYZ(coordi);
	return pt + Vec3f(voxelSize, voxelSize, voxelSize) / 2;
}


bool voxelObject::init(SurfaceObj *obj, int res)
{
	s_surObj = obj;

	// Octree
	m_octree.init(s_surObj, res);
	m_centerf = m_octree.centerMesh;
	m_voxelSizef = m_octree.boxSize;

	// Voxel hash
	constructVolxeHash();

	// Boxes
	constructNeighbor();



	return true;
}

bool voxelObject::init(SurfaceObj *obj, int res, float scale)
{
	s_surObj = obj;

	// Octree
	m_octree.init(s_surObj, res);
	m_centerf = m_octree.centerMesh;
	m_voxelSizef = m_octree.boxSize;

	// Voxel hash
	constructVolxeHash(scale);

	// Boxes
	constructNeighbor();

	return true;

}

bool voxelObject::init(voxelObject *highRes, int voxelRes)
{
	s_surObj = highRes->s_surObj;

	// Octree
	m_octree.init(s_surObj, voxelRes);
	m_centerf = m_octree.centerMesh;
	m_voxelSizef = m_octree.boxSize;
	// Remove high empty box
	m_octree.removeLowOccupationBox(&highRes->m_octree);

	// Voxel hash
	constructVolxeHash();

	// Boxes
	constructNeighbor();


	return false;
}

bool voxelObject::init(voxelObject *highRes, float voxelSize)
{
	s_surObj = highRes->s_surObj;

	// Octree
	m_octree.init(s_surObj, voxelSize);
	m_centerf = m_octree.centerMesh;
	m_voxelSizef = m_octree.boxSize;
	// Remove high empty box
	m_octree.removeLowOccupationBox(&highRes->m_octree);

	// Voxel hash
	constructVolxeHash();

	// Boxes
	constructNeighbor();


	return true;
}

bool voxelObject::initWithSize(SurfaceObj *obj, float voxelSize)
{
	s_surObj = obj;

	// Octree
	m_octree.init(s_surObj, voxelSize);
	m_centerf = m_octree.centerMesh;
	m_voxelSizef = m_octree.boxSize;

	// Voxel hash
	constructVolxeHash();

	// Boxes
	constructNeighbor();



	return true;
}

void voxelObject::constructVolxeHash()
{
	float voxelSize = m_octree.boxSize;
	Vec3f gap(voxelSize, voxelSize, voxelSize);
		 
	Vec3f leftDownVoxel = m_octree.m_root->leftDownTight - gap;
	Vec3f rightUpVoxel = m_octree.m_root->rightUpTight + gap;
	Vec3f sizef = rightUpVoxel - leftDownVoxel;

	Vec3i NumXYZ;
	for (int i = 0; i < 3; i++)
	{
		NumXYZ[i] = (sizef[i] / m_voxelSizef);
	}

	m_hashTable.leftDown = leftDownVoxel;
	m_hashTable.rightUp = rightUpVoxel;
	m_hashTable.voxelSize = m_voxelSizef;
	m_hashTable.NumXYZ = NumXYZ;
	m_hashTable.boxes = &m_boxes;

	// voxel index and hash
	std::vector<int> voxelHash;
	voxelHash.resize(NumXYZ[0] * NumXYZ[1] * NumXYZ[2]);
	std::fill(voxelHash.begin(), voxelHash.end(), -1);

	std::vector<octreeSNode*> *leaves = &m_octree.leaves;
	m_boxes.resize(leaves->size());
	for (int i = 0; i < leaves->size(); i++)
	{
		octreeSNode* node = leaves->at(i);
		// List
		voxelBox newBox(node->leftDownf, node->rightUpTight);
		Vec3f xyzIdx = m_hashTable.getVoxelCoord(newBox.center);
		newBox.xyzIndex = xyzIdx;

		m_boxes[i] = newBox;

		// Hash
		int hashF = xyzIdx[2] * NumXYZ[0] * NumXYZ[1] + xyzIdx[1] * NumXYZ[0] + xyzIdx[0];

		voxelHash[hashF] = i;
		node->idxInLeaf = i;
	}

	m_hashTable.voxelHash = voxelHash;
}

Vec3f voxelObject::floorV(Vec3f v, float d)
{
	Vec3f ld = m_hashTable.leftDown;
	Vec3f dv = v - ld;
	for (int i = 0; i < 3; i++)
	{
		dv[i] = d*floor(dv[i] / d);
	}
	return dv+ld;
}

void voxelObject::constructVolxeHash(float scale)
{
	// Scale the bounding box
	float voxelSize = m_octree.boxSize;
	Vec3f unitV(voxelSize, voxelSize, voxelSize);

	Vec3f leftDownVoxel = m_octree.m_root->leftDownTight;
	Vec3f rightUpVoxel = m_octree.m_root->rightUpTight;
	Vec3f boundSize = rightUpVoxel - leftDownVoxel;
	leftDownVoxel = leftDownVoxel - boundSize*(scale / 2);
	rightUpVoxel = rightUpVoxel + boundSize*(scale / 2);

	// Snap the bounding box to voxel conner
	leftDownVoxel = floorV(leftDownVoxel, m_voxelSizef);
	rightUpVoxel = floorV(rightUpVoxel, m_voxelSizef);

	// Construct
	Vec3f sizef = rightUpVoxel - leftDownVoxel;

	Vec3i NumXYZ;
	for (int i = 0; i < 3; i++)
	{
		NumXYZ[i] = (sizef[i] / m_voxelSizef);
	}

	m_hashTable.leftDown = leftDownVoxel;
	m_hashTable.rightUp = rightUpVoxel;
	m_hashTable.voxelSize = m_voxelSizef;
	m_hashTable.NumXYZ = NumXYZ;
	m_hashTable.boxes = &m_boxes;

	// voxel index and hash
	std::vector<int> voxelHash;
	voxelHash.resize(NumXYZ[0] * NumXYZ[1] * NumXYZ[2]);
	std::fill(voxelHash.begin(), voxelHash.end(), -1);

	std::vector<octreeSNode*> *leaves = &m_octree.leaves;
	m_boxes.resize(leaves->size());
	for (int i = 0; i < leaves->size(); i++)
	{
		octreeSNode* node = leaves->at(i);
		// List
		voxelBox newBox(node->leftDownf, node->rightUpTight);
		Vec3i xyzIdx = m_hashTable.getVoxelCoord(newBox.center);
		newBox.xyzIndex = xyzIdx;

		m_boxes[i] = newBox;

		// Hash
		int hashF = xyzIdx[2] * NumXYZ[0] * NumXYZ[1] + xyzIdx[1] * NumXYZ[0] + xyzIdx[0];

		voxelHash[hashF] = i;
		node->idxInLeaf = i;
	}

	m_hashTable.voxelHash = voxelHash;
}

void voxelObject::constructNeighbor()
{
	// Temp var
	std::vector<Vec3i> allRelativeNeighbor;
	Vec3i indexGap(-1, 0, 1);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (i == 0 && j == 0 && k == 0) // itself
				{
					continue;
				}
				allRelativeNeighbor.push_back(Vec3i(indexGap[i], indexGap[j], indexGap[k]));
			}
		}
	}

	// Construct all neighbor information
	m_allBoxAroundBox.clear();
	for (int i = 0; i < m_boxes.size(); i++)
	{
		Vec3i curBoxCoordi = m_boxes[i].xyzIndex;
		arrayInt neighborIdxs;
		for (int j = 0; j < allRelativeNeighbor.size(); j++)
		{
			Vec3i neighbori = curBoxCoordi + allRelativeNeighbor[j];
			int nbIdx = m_hashTable.getBoxIndexFromVoxelCoord(neighbori);
			if (nbIdx != -1)
			{
				neighborIdxs.push_back(nbIdx);
			}
		}
		m_allBoxAroundBox.push_back(neighborIdxs);
	}

	// Construct neighbor that share face
	m_boxShareFaceWithBox.clear();
	for (int i = 0; i < m_boxes.size(); i++)
	{
		Vec3i curBoxCoordi = m_boxes[i].xyzIndex;
		arrayInt neighborIdxs;
		for (int j = 0; j < 3; j++)
		{
			for (int dd = -1; dd < 2; dd += 2)
			{
				Vec3i neighbori = curBoxCoordi;
				neighbori[j] += dd;
				int nbIdx = m_hashTable.getBoxIndexFromVoxelCoord(neighbori);
				if (nbIdx != -1)
				{
					neighborIdxs.push_back(nbIdx);
				}
			}
		}
		m_boxShareFaceWithBox.push_back(neighborIdxs);
	}
}

void voxelObject::drawVoxel(int mode)
{
	m_octree.drawWireOctree(mode);
}

voxelObject::voxelObject()
{

}

voxelObject::~voxelObject()
{

}

void voxelObject::drawVoxelLeaf(int mode)
{
	for (int i = 0; i < m_boxes.size(); i++)
	{
		m_boxes[i].draw(mode);
	}
}

// void voxelObject::decomposeConvexes()
// {
// 	// Construct voxel bit set
// 	// test by hard code first
// 	std::vector<Box> boxDecompose;
// 	boxDecompose.push_back(Box(Vec3f(-20, -20, -20), Vec3f(0, 20, 20)));
// 	boxDecompose.push_back(Box(Vec3f(0, -20, 0), Vec3f(20, 20, 20)));
// 	boxDecompose.push_back(Box(Vec3f(0, -20, -20), Vec3f(20, 20, 0)));
// 
// 	// Check voxel inside these boxes.
// 	// Assign to decomposed box
// 	m_voxelBitSet.hashTable = &m_hashTable;
// 	m_voxelBitSet.m_leftDownf = m_octree.m_root->leftDownf;
// 	m_voxelBitSet.m_rightUpf = m_octree.m_root->rightUpf;
// 
// 	GeometricFunc geoFunc;
// 	for (int i = 0; i < boxDecompose.size(); i++)
// 	{
// 		voxelBitConvex newCVBitset;
// 		for (int j = 0; j < m_boxes.size(); j++)
// 		{
// 			if (geoFunc.isPointInBox(boxDecompose[i].leftDown, boxDecompose[i].rightUp, m_boxes[j].center))
// 			{
// 				Vec3i coordi = m_boxes[j].xyzIndex;
// 				newCVBitset.setValue1(coordi);
// 			}
// 		}
// 		newCVBitset.leftDowni = m_hashTable.leftDowni();
// 		newCVBitset.rightUpi = m_hashTable.rightUpi();
// 		newCVBitset.tightBoundingBox(&m_hashTable);
// 
// 		m_voxelBitSet.groupVoxels.push_back(newCVBitset);
// 	}
// 
// 	m_voxelBitSet.computeBoundingBox();
// }
// 
// void voxelObject::drawBitSet(voxelSplitObj* vBitSet)
// {
// 	vBitSet->drawVoxelBox();
// }
// 
// void voxelObject::drawVoxelBitDecomposed()
// {
// 	voxelBitConvex_array *bitSetA = m_voxelBitSet.getBitSetArray();
// 
// 	static arrayVec3f colors = Util_w::randColor(bitSetA->size()+1);
// 
// 	for (int i = 0; i < bitSetA->size(); i++)
// 	{
// 		glColor3fv(colors[i+1].data());
// 		bitSetA->at(i).drawBoxWire(&m_hashTable);
// 
// 		glColor3fv(colors[i].data());
// 		bitSetA->at(i).drawBoxSolid(&m_hashTable);
// 	}
// }
// 
// void voxelObject::drawBitSetBoundingBox()
// {
// 	voxelBitConvex_array *bitSetA = m_voxelBitSet.getBitSetArray();
// 
// 	static arrayVec3f colors = Util_w::randColor(bitSetA->size() + 1);
// 
// 	for (int i = 0; i < bitSetA->size(); i++)
// 	{
// 		glColor3fv(colors[i].data());
// 		bitSetA->at(i).drawBoudingBox(&m_hashTable);
// 	}
// }

// void voxelObject::constructBitSetMesh()
// {
// 	using namespace energy;
// 
// 	meshBitSet = bitSetObjectPtr(new bitSetObject(m_hashTable.NumXYZ));
// 	for (auto b : m_boxes)
// 	{
// 		Vec3i posi = b.xyzIndex;
// 		meshBitSet->setAtPos(posi);
// 	}
// }

float voxelObject::volumef() const
{
	return m_boxes.size() * std::pow(m_voxelSizef, 3);
}

// void voxelObject::updateSphereOccupy(energyMngerPtr curEnergyObj)
// {
// 	std::vector<skeSpherePtr> sArray = curEnergyObj->sphereArray();
// 	
// 	arrayInt voxeHash = m_hashTable.voxelHash;
// 	// Clear voxel state
// 	for (auto b:m_boxes)
// 	{
// 		b.state = 0;
// 	}
// 
// 	for (auto s:sArray)
// 	{
// 
// 	}
// }

void voxelObject::drawVoxelIndex()
{
	for (int i = 0; i < m_boxes.size(); i++)
	{
		Vec3f c = m_boxes[i].center;
		Util::printw(c[0], c[1], c[2], "%d", i);
	}
}
