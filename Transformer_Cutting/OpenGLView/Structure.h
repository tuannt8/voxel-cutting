#pragma once

#include "stdafx.h"
#include "Utility.h"
typedef std::vector<vec3d> arrayvec3d;
typedef std::vector<vec3i> arrayvec3i;
/************************************************************************/
/* Edge -> Data structure edege                                         */
/************************************************************************/
struct edge
{
	int p1, p2;

	edge()
	{
		p1 = 0; p2 = 0;
	}
	edge(int p1, int p2)
	{
		this->p1 = p1;
		this->p2 = p2;
	}
	int operator [](int i)
	{
		assert(i==0||i==1);

		if(i==0)	return p1;
		else		return p2;
	}
	bool operator ==(edge e)
	{
		if((p1 == e.p1 && p2 == e.p2) ||
			(p1 == e.p2 && p2 == e.p1))
			return true;
		else
			return false;
	}
};

/************************************************************************/
/* Data structure - keep the info of parallel surface in preprocessing
 * Orthogonal to z-axis
 * Used to determine center point
/************************************************************************/
struct latticeSlice
{
	vec3d origin;				// coordinate of original point // Preprocessing

	vec3d centerNorm;					// Normal vector
	vec3d centerPoint;					// Center point
	arrayvec3d latticePoints;			// 6 lattice points

	latticeSlice(){};

	void CalculateCenterPoint(arrayvec3d intersectPoints)
	{
		assert(intersectPoints.size() > 0);

		float minX, maxX, minY, maxY, minZ, maxZ;
		minX = maxX = intersectPoints[0].x;
		minY = maxY = intersectPoints[0].y;
		maxZ = minZ = intersectPoints[0].z;
		for (int i = 0; i < intersectPoints.size(); i++)
		{
			if(minX > intersectPoints[i].x)minX = intersectPoints[i].x;
			if(maxX < intersectPoints[i].x)maxX = intersectPoints[i].x;
			if(minY > intersectPoints[i].y)minY = intersectPoints[i].y;
			if(maxY < intersectPoints[i].y)maxY = intersectPoints[i].y;
			if(minZ > intersectPoints[i].z)minZ = intersectPoints[i].z;
			if(maxZ < intersectPoints[i].z)maxZ = intersectPoints[i].z;
		}
		centerPoint = vec3d((minX+maxX)/2, (minY + maxY)/2, (minZ+maxZ)/2);
	}
	void drawCenterLine()
	{
		glColor3f(0.0,0.0,1.0);
		glBegin(GL_LINES);
		for (int i =0; i< latticePoints.size(); i++)
		{
			glVertex3f(centerPoint.x, centerPoint.y, centerPoint.z);
			glVertex3f(latticePoints.at(i).x, latticePoints.at(i).y,latticePoints.at(i).z);
		}
		glEnd();

		glColor3f(0.0,0.0,1.0);
		glPointSize(4);
		glBegin(GL_POINTS);
		for (int i =0; i< latticePoints.size(); i++)
		{
			glVertex3f(latticePoints.at(i).x, latticePoints.at(i).y,latticePoints.at(i).z);
		}
		glEnd();

		glLineWidth(2.0);
		glColor3f(0.0,0.0,1.0);
		glBegin(GL_LINE_LOOP);
		for (int i =0; i< latticePoints.size(); i++)
		{
			glVertex3f(latticePoints.at(i).x, latticePoints.at(i).y,latticePoints.at(i).z);
		}
		glEnd();
		glLineWidth(1.0);
	}

	void calculateNormFromLattice()
	{
		vec3d p1 = latticePoints[1] - latticePoints[0];
		vec3d p2 = latticePoints[2] - latticePoints[1];

		vec3d n = p1.cross(p2);
		n.normalize();

		centerNorm = n;
	}

	void write(FILE* f)
	{
		Utility::writeVector(f, centerPoint);
		Utility::writeVector(f, centerNorm);
		Utility::writeArray(f, latticePoints);
	}
	void read(FILE *f)
	{
		centerPoint = Utility::readVector(f);
		centerNorm = Utility::readVector(f);
		latticePoints = Utility::readArray(f);
	}

	void writeMaple(FILE* f)
	{
		Utility::writeVector(f, centerPoint);
		for (int i = 0; i < latticePoints.size(); i++)
		{
			Utility::writeVector(f,latticePoints[i]);
		}
	}
};

/************************************************************************/
/* Center line data                                                    */
/************************************************************************/
struct slice
{
	//Centerline data - This is ray-casting data
	vec3d centerPoint;			// Coordinate of center point
	vec3d centerNorm;			// Normal vector of slice surface
	std::vector<int> pointIndex; // index of points lie in this slice
	arrayvec3d interSectRay;	// Set of points on object surface
	arrayvec3d localCoordSet;	// Local coordinate uvw set
	arrayvec3i latticeIndexSet; // index of lattice cell where the point lie in

	int index[2]; //Bound index of lattice


	std::vector<int> texPointIdxs; // Index of texture point on Ring

	slice(){}

	void write(FILE* f)
	{
		Utility::writeVector(f, centerPoint);
		Utility::writeVector(f,centerNorm);
		Utility::writeArray(f, interSectRay);
		fprintf(f,"%d %d\n", index[0], index[1]);
	}

	void read(FILE* f)
	{
		centerPoint = Utility::readVector(f);
		centerNorm = Utility::readVector(f);
		interSectRay = Utility::readArray(f);
		fscanf(f, "%d %d", &index[0], &index[1]);
	}

	void readLocalCoord(FILE* f)
	{
		this->localCoordSet = Utility::readArray(f);
		this->latticeIndexSet = Utility::readVec3iArray(f);
	}
	void writeMaple(FILE* f)
	{
		Utility::writeVector(f, centerPoint);
		Utility::writeVector(f,vec3d(index[0], index[1], 0.0));
		for (int i = 0; i < interSectRay.size(); i++)
		{
			Utility::writeVector(f,interSectRay[i]);
		}
	}

	void writeERCP(FILE* f)
	{
		for (int i = 0; i< localCoordSet.size(); i++)
		{
			int ii, jj;
			vec3i index = latticeIndexSet[i]; 
			vec3d coordC= localCoordSet[i];

			ii = index[0]*7; //Center point coord
			jj = index[0]*7 + index[1]; 

			fprintf(f, "%d %d %lf %lf %lf \n", index[0], index[1], coordC[0], coordC[1], coordC[2]);
		}
	}
};