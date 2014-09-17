#pragma once
#include "Utility.h"
#include "DataTypes/vec.h"
#include <vector>
#include "DataTypes/define.h"


typedef std::vector<Vec3i> Vec3is;
typedef std::vector<Vec3f> Vec3fs;

namespace Util_w
{	
	Vec3i vec3iMul(Vec3i a, float bb)
	{
		return Vec3i(a[0]*bb, a[1]*bb, a[2]*bb);
	}

	Vec3f Vec3iXfloat(Vec3i a, float bb)
	{
		return Vec3f((float)a[0] * bb, (float)a[1] * bb, (float)a[2] * bb);
	}

	// Return center position of the box
	Vec3f IJK2XYZ(Vec3i &IJK, float &cubeSize){
		Vec3f XYZ(0,0,0);
		for (int i = 0; i < 3; i++){
			XYZ[i] = IJK[i]*cubeSize + cubeSize/2.0;
		}
		return XYZ;
	}
	Vec3i XYZ2IJK(Vec3f &XYZ, float &cubeSize)
	{
		Vec3i IJK;
		for (int i = 0; i < 3; i++){
			IJK[i] = floor(XYZ[i]/cubeSize);
		}
		return IJK;
	}

	void drawBoxWireFrame(Vec3f LeftDown, Vec3f upRight)
	{
		Util::drawBox(LeftDown[0], LeftDown[1], LeftDown[2], upRight[0], upRight[1], upRight[2]);
	}
	void drawBoxSurface(Vec3f LeftDown, Vec3f upRight)
	{
		Util::drawBoxFace(LeftDown[0], LeftDown[1], LeftDown[2], upRight[0], upRight[1], upRight[2]);
	}

	void drawIntCoordBox(Vec3i leftDown, Vec3i rightUp, float voxelSize)
	{
		Vec3f diag(voxelSize/2, voxelSize/2, voxelSize/2);
		Vec3f leftDownf = IJK2XYZ(leftDown, voxelSize) - diag;
		Vec3f rightUpf = IJK2XYZ(rightUp, voxelSize) + diag;

		drawBoxWireFrame(leftDownf, rightUpf);
	}

	void log3DArray(int *** arr, Vec3i sizei)
	{
		for (int i = 0; i < sizei[0]; i++)
		{
			TRACE1("%d --------------\n", i);
			for (int j = 0; j < sizei[1]; j++)
			{
				for (int k = 0; k < sizei[2]; k++)
				{
					TRACE1(" %d", arr[i][j][k]);
				}
				TRACE("\n");
			}

		}
	}

	void drawBoxWire(Vec3f pt, float halfSize)
	{
		Util::drawBoxWire(pt[0], pt[1], pt[2], halfSize);
	}

	void sort3_descending( Vec3i &arr )
	{
		float  arr12[ 2 ];

		// sort first two values
		if( arr[ 0 ] > arr[ 1 ] )
		{
			arr12[ 0 ] = arr[ 0 ];
			arr12[ 1 ] = arr[ 1 ];
		} // if
		else
		{
			arr12[ 0 ] = arr[ 1 ];
			arr12[ 1 ] = arr[ 0 ];
		} // else

		// decide where to put arr12 and the third original value arr[ 3 ]
		if( arr12[ 1 ] > arr[ 2 ] )
		{
			arr[ 0 ] = arr12[ 0 ];
			arr[ 1 ] = arr12[ 1 ];
		} // if
		else if( arr[ 2 ] > arr12[ 0 ] )
		{
			arr[ 0 ] = arr  [ 2 ];
			arr[ 1 ] = arr12[ 0 ];
			arr[ 2 ] = arr12[ 1 ];
		} // if
		else
		{
			arr[ 0 ] = arr12[ 0 ];
			arr[ 1 ] = arr  [ 2 ];
			arr[ 2 ] = arr12[ 1 ];
		} // else
	}

	void drawTriFace(std::vector<Vec3f> *points, std::vector<Vec3i>* face)
	{
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < face->size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				glVertex3fv((*points)[(*face)[i][j]].data());
			}
		}
		glEnd();
	}

	static void drawTriWire(std::vector<Vec3f>* points, std::vector<Vec3i>* faces)
	{
		glBegin(GL_LINES);
		for (int i = 0; i < faces->size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				glVertex3fv((*points)[(*faces)[i][j]].data());
				glVertex3fv((*points)[(*faces)[i][(j+1)%3]].data());
			}
		}
		glEnd();
	}

	Vec3i SMLIndexSizeOrder(Vec3f sizef)
	{
		Vec3i SMLEdgeIdx;
		// Order of the edge lengths
		if (sizef[0]>sizef[1])
		{
			if (sizef[1] > sizef[2])
				SMLEdgeIdx = Vec3i(2,1,0);
			else if (sizef[2]<sizef[0])
				SMLEdgeIdx = Vec3i(1,2,0);
			else
				SMLEdgeIdx = Vec3i(1,0,2);
		}
		else
		{
			if (sizef[1] < sizef[2])
				SMLEdgeIdx = Vec3i(0,1,2);
			else if (sizef[2]>sizef[0])
				SMLEdgeIdx = Vec3i(0,2,1);
			else
				SMLEdgeIdx = Vec3i(2,0,1);
		}

		return SMLEdgeIdx;
	}

	static Box getSymetrixBox(Box b, int axis, float coord)
	{
		Box o = b;
		Vec3f offs;
		offs[axis] = 2*coord - b.leftDown[axis] - b.rightUp[axis];
		
		o.leftDown = o.leftDown + offs;
		o.rightUp = o.rightUp + offs;

		return o;
	}

	std::vector<Vec3f> randColor(int num)
	{
		std::vector<Vec3f> color;
		for (int i = 0; i < num; i++)
		{
			Vec3f newC(((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)));
			color.push_back(newC);
		}

		return color;
	}

	arrayInt substractArrayInt(arrayInt minusEnd, arrayInt subtrahend, int maxContentValue)
	{
		int * hash = new int[maxContentValue];
		std::fill(hash, hash + maxContentValue, 0);
		for (int i = 0; i < subtrahend.size(); i++)
		{
			hash[subtrahend[i]] = 1;
		}

		arrayInt out;
		for (int i = 0; i < minusEnd.size(); i++)
		{
			if (hash[minusEnd[i]] == 0)
			{
				out.push_back(minusEnd[i]);
			}
		}

		return out;
	}

	static void drawBoxSolidCenter(Vec3f coordf, float halfSize)
	{
		Util::drawBoxSurface(coordf[0], coordf[1], coordf[2], halfSize);
	}





}
