#include "stdafx.h"
#include "MeshfreeNodeGenerator.h"


MeshfreeNodeGenerator::MeshfreeNodeGenerator(void)
{
}

MeshfreeNodeGenerator::~MeshfreeNodeGenerator(void)
{
}

void MeshfreeNodeGenerator::setSurfObj(SurfaceObj* obj)
{
	Obj=obj;
}

void MeshfreeNodeGenerator::generateUniformNode(int res)
{
	// 1. Bounding box generation
	generateBoundingBox(Obj->point());

	// 2. Set resolution
	Vec3f box=(RightUpPoint-LeftDownPoint);
	float min=100000000;
	int idx=-1;
	for(int i=0;i<3;i++)
	{
		if(box[i]<min)
		{
			min=box[i];
			idx=i;
		}
	}
	Vec3i _res;
	float length=box[idx]/(float)res;
	for(int i=0;i<3;i++)
	{
		if(!(i==idx))
			_res[i]=box[i]/length;
	}
	_res[idx]=res;
	
	//3. Node generation
	generateNodeInBoundingBox(_res);

// 	//.a. Add node identical to surface vertex
// 	addNodeOnSurfaceVertex();

	//4. Compute node volume
	computeNodeVolume();

	//5. Remove nodes outside object
	removeNodeOutsideObject();


}

float MeshfreeNodeGenerator::rayTracingToGenVoxel(int res)
{
	// 1. Compute size of voxel
	LeftDownPoint = Obj->getBVH()->root()->LeftDown;
	RightUpPoint = Obj->getBVH()->root()->RightUp;

	// divide resolution base on shortest size
	Vec3f box=(RightUpPoint-LeftDownPoint);
	float min=100000000;
	int idx=-1;
	for(int i=0;i<3;i++)
	{
		if(box[i]<min)
		{
			min=box[i];
			idx=i;
		}
	}
	voxelSize =box[idx]/(float)res;
	// Box coordinate in voxel coordinate
	Vec3i Pmin = XYZ2IJK(LeftDownPoint, voxelSize);
	Vec3i Pmax = XYZ2IJK(RightUpPoint, voxelSize);

	// 2. Tracing x-direction
	Vec3f direct(1, 0, 0);
	CollisionManager colMngr;
	GeometricFunc geoFunc;
	float xmin = LeftDownPoint[0];

	for (int j = Pmin[1]; j <= Pmax[1]; j++)
	{
		for (int k = Pmin[2]; k <= Pmax[2]; k++)
		{
			Vec3f pt = IJK2XYZ(Vec3i(xmin,j,k), voxelSize);
			arrayVec3f intersectedPts;
			// Find the intersection

			arrayVec3f newNode;
			Scanline(pt, direct, newNode);

			NodePos.insert(NodePos.end(), newNode.begin(), newNode.end());
		}
	}
	// We may need to trace along y and z in order to achieve better quality of the mesh

	// 3.

	return voxelSize;
}

void MeshfreeNodeGenerator::generateUniformNodeWithStressPoint(int res)
{
	// 1. Bounding box generation
	generateBoundingBox(Obj->point());

	// 2. Set resolution
	Vec3f box=(RightUpPoint-LeftDownPoint);
	float min=100000000;
	int idx=-1;
	for(int i=0;i<3;i++)
	{
		if(box[i]<min)
		{
			min=box[i];
			idx=i;
		}
	}
	Vec3i _res;
	float length=box[idx]/(float)res;
	for(int i=0;i<3;i++)
	{
		if(!(i==idx))
			_res[i]=box[i]/length;
	}
	_res[idx]=res;

	//3. Node generation
	generateNodeInBoundingBoxWithStressPoint(_res);

	//4. Compute node volume
	computeNodeVolumeWithStressPoint();

	//5. Remove nodes outside object
	removeNodeOutsideObjectWithStressPoint();
}

void MeshfreeNodeGenerator::generateNodeInBoundingBox(Vec3i res)
{
	Vec3f ds;
	for(int i=0;i<3;i++)
		ds[i]=(RightUpPoint[i]-LeftDownPoint[i])/float(res[i]);

	for(int i=0;i<=res[1];i++)
	{
		for(int j=0;j<=res[0];j++)
		{
			for(int k=0;k<=res[2];k++)
			{
				Vec3d pos=LeftDownPoint+Vec3d(ds[0]*j,ds[1]*i,ds[2]*k);
				NodePos.push_back(pos+ds/2.0);
			}
		}
	}
	BoxSize=ds;
}

void MeshfreeNodeGenerator::generateNodeInBoundingBoxWithStressPoint(Vec3i res)
{
	Vec3f ds;
	for(int i=0;i<3;i++)
		ds[i]=(RightUpPoint[i]-LeftDownPoint[i])/float(res[i]);

	for(int i=0;i<=(res[1]+1);i++)
	{
		for(int j=0;j<=(res[0]+1);j++)
		{
			for(int k=0;k<=(res[2]+1);k++)
			{
				Vec3f pos=LeftDownPoint+Vec3d(ds[0]*j,ds[1]*i,ds[2]*k);
				if(!(i==(res[1]+1)))
				{
					if(!(j==(res[0]+1)))
					{
						if(!(k==(res[2]+1)))
							NodePos.push_back(pos+ds/2.0);
					}
				}
				StressPoint.push_back(pos);
			}
		}
	}
	BoxSize=ds;
}

void MeshfreeNodeGenerator::generateBoundingBox(std::vector<Vec3f>* point)
{
	float offset=1.001;
	Vec3d minPoint(MAX, MAX, MAX);
	Vec3d maxPoint(MIN, MIN, MIN);

	for(int i=0;i<point->size();i++)
	{
		for(int j=0;j<3;j++)
		{
			if((*point)[i][j]<minPoint[j])
				minPoint[j]=(*point)[i][j];
			if((*point)[i][j]>maxPoint[j])
				maxPoint[j]=(*point)[i][j];
		}
	}
	LeftDownPoint=minPoint-Vec3d(offset,offset,offset);
	RightUpPoint=maxPoint+Vec3d(offset,offset,offset);
}

void MeshfreeNodeGenerator::removeNodeOutsideObject()
{
	if(!Obj->getBVH())
		Obj->constructAABBTree();
	Vec3i direc(0,0,1);
	CollisionManager collision;
	for(int i=0;i<NodePos.size();i++)
	{
		if(!collision.isPointInSurfObj(Obj, NodePos[i]))
		{
			NodePos[i]=NodePos[NodePos.size()-1];
			NodePos.pop_back();
			i--;
		}
	}
}

void MeshfreeNodeGenerator::removeNodeOutsideObjectWithStressPoint()
{
	if(!Obj->getBVH())
		Obj->constructAABBTree();
	Vec3i direc(0,0,1);
	CollisionManager collision;
	for(int i=0;i<NodePos.size();i++)
	{
		if(!collision.isPointInSurfObj(Obj, NodePos[i]))
		{
			NodePos[i]=NodePos[NodePos.size()-1];
			NodePos.pop_back();
			i--;
		}
	}

	for(int i=0;i<StressPoint.size();i++)
	{
		if(!collision.isPointInSurfObj(Obj, StressPoint[i]))
		{
			StressPoint[i]=StressPoint[StressPoint.size()-1];
			StressPoint.pop_back();
			i--;
		}
	}
}

void MeshfreeNodeGenerator::computeNodeVolume()
{
	float volume=(RightUpPoint[0]-LeftDownPoint[0])*(RightUpPoint[1]-LeftDownPoint[1])*(RightUpPoint[2]-LeftDownPoint[2]);
	int nbNode=NodePos.size();
	NodeVolume=volume/(float)nbNode;
}

void MeshfreeNodeGenerator::computeNodeVolumeWithStressPoint()
{
	float volume=(RightUpPoint[0]-LeftDownPoint[0])*(RightUpPoint[1]-LeftDownPoint[1])*(RightUpPoint[2]-LeftDownPoint[2]);
	int nbNode=NodePos.size()+StressPoint.size();
	NodeVolume=volume/(float)nbNode;
}

int MeshfreeNodeGenerator::findNodeNearSurface(float radius)
{
	VectorFunc func;
	std::vector<Vec3f> nodeNearSurf;
	std::vector<Vec3f>* pos=Obj->point();
	for(int i=0;i<Obj->nbPoint();i++)
	{
		for(int j=0;j<NodePos.size();j++)
		{
			if(((*pos)[i]-NodePos[j]).norm()<radius)
			{
				nodeNearSurf.push_back(NodePos[j]);
				func.removeElement(NodePos, j);
				j--;
			}
		}
	}
	int nbNodeInside=NodePos.size();
	for(int i=0;i<nodeNearSurf.size();i++)
		NodePos.push_back(nodeNearSurf[i]);
	return nbNodeInside;
}

void MeshfreeNodeGenerator::addNodeOnSurfaceVertex()
{
	// Need optimize
	// Add to cutting area only
	arrayVec3f* vertexs = Obj->point();
	NodePos.insert(NodePos.end(), vertexs->begin(), vertexs->end());
}

Vec3f MeshfreeNodeGenerator::IJK2XYZ(Vec3i IJK, float length)
{
	Vec3f xyz;
	for (int i = 0; i < 3; i++)
	{
		xyz[i] = IJK[i]*length + length/2.0;
	}
	return xyz;
}

Vec3i MeshfreeNodeGenerator::XYZ2IJK(Vec3f XYZ, float length)
{
	Vec3i ijk;
	for (int i = 0; i < 3; i++)
	{
		ijk[i] = floor(XYZ[i]/length);
	}
	return ijk;
}

void MeshfreeNodeGenerator::Scanline(Vec3f lpt, Vec3f ldirect, arrayVec3f &newNode)
{
	//colMngr.collisionBtwSurfAndAxisLine(Obj, pt, direct, intersectedPts);
	
	GeometricFunc func;
	VectorFunc vecFunc;
	AABBNode* root=Obj->getBVH()->root();
	std::vector<Vec3f>* point=Obj->point();
	std::vector<Vec3i>* face=Obj->face();
	std::vector<Vec2i>* edge = Obj->edge();
	std::vector<Vec3f>* normal = Obj->faceNormal();

	// 1. Find potential triangle in collision
	std::vector<int> pCollisionTriIdx;
	CollisionManager colMng;
	colMng.collisionBtwAABBAndAxisLine(root, lpt, ldirect, pCollisionTriIdx);

	// 2. Compute intersection 
	arrayVec3f intersectedPts;
	std::vector<BOOL> goInMesh;
	for(int i=0;i<pCollisionTriIdx.size();i++)
	{
		Tri tri;
		tri.p[0]=(*point)[(*face)[pCollisionTriIdx[i]][0]];
		tri.p[1]=(*point)[(*face)[pCollisionTriIdx[i]][1]];
		tri.p[2]=(*point)[(*face)[pCollisionTriIdx[i]][2]];
		tri.normal=(*normal)[pCollisionTriIdx[i]];
		Vec3f _intersection;

		if(func.collisionBtwTriAndLine(lpt, ldirect, tri, _intersection))
		{
			intersectedPts.push_back(_intersection);
			goInMesh.push_back(ldirect*tri.normal < 0);
		}
	}

//	ASSERT(!(intersectedPts.size() >= 4));

	// Fill voxel inside intersected points
	if (intersectedPts.size() > 0)
	{
		// Sort the intersection point by X
		arrayVec3f sortedArray;
		std::vector<BOOL> goIn;
		for (int i = 0; i < intersectedPts.size(); i++)
		{
			Vec3f curP = intersectedPts[i];
			int idx;
			for (idx = 0; idx < sortedArray.size(); idx++)
			{
				if (curP[0] <= sortedArray[idx][0])
				{
				//	idx++;
					break;
				}
			}
			sortedArray.insert(sortedArray.begin()+idx, curP);
			goIn.insert(goIn.begin()+idx, goInMesh[i]);
		}

		// remove identical point (Cases: edge intersection)
		arrayVec3f finalArray;
		finalArray.push_back(sortedArray[0]); ASSERT(goIn[0]);
		BOOL isGoIn = TRUE;
		for (int i = 1; i < sortedArray.size(); i++)
		{
			// Check identical
			if ((sortedArray[i]-sortedArray[i-1]).norm() <= ZERO_F
				&& goIn[i] == goIn[i-1])
			{
				break;
			}
			finalArray.push_back(sortedArray[i]);
		}
		ASSERT(finalArray.size()%2 == 0);
		
		// There may be more than 2 intersections
		int nbSegment = finalArray.size()/2;
		for (int ii = 0; ii < nbSegment; ii++)
		{
			Vec3i ijkMin = XYZ2IJK(finalArray[ii*2], voxelSize);
			Vec3i ijkMax = XYZ2IJK(finalArray[ii*2 + 1], voxelSize);
			// Fill voxel
			for (int iIdx = ijkMin[0]; iIdx <= ijkMax[0]; iIdx++)
			{
				Vec3f VoxelPt = IJK2XYZ(Vec3i(iIdx, ijkMin[1], ijkMin[2]), voxelSize);
				newNode.push_back(VoxelPt);
			}
		}

	}
}


