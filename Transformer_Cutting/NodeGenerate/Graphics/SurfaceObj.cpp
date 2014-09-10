#include "stdafx.h"
#include "SurfaceObj.h"
#include "stl.h"

//#include "Utility.h"

SurfaceObj::SurfaceObj(void)
{
	Container=NULL;
	Modifier=NULL;
	BVHAABB=NULL;
}

SurfaceObj::~SurfaceObj(void)
{
	if(Container){delete Container; Container=NULL;}
	if(Modifier){delete Modifier; Modifier=NULL;}
	if(BVHAABB){delete BVHAABB; BVHAABB=NULL;}
}

void SurfaceObj::readObjData(char* filename)
{
	FILE *fp;
	int i;
	fp = fopen(filename,"r");
	int pointNum=0;
	int faceNum=0;

	fscanf(fp,"%d\n",&pointNum);
	Point0.resize(pointNum);
	Point.resize(pointNum);
	Dis.resize(pointNum);
	PointNormal.resize(pointNum);

	for(i=0;i<pointNum;i++)
	{
		fscanf(fp,"%f %f %f\n",&Point[i][0], &Point[i][1], &Point[i][2]);
		Point0[i]=Point[i];
	}

	fscanf(fp,"%d\n",&faceNum);
	Face.resize(faceNum);
	FaceNormal.resize(faceNum);

	for(i=0;i<faceNum;i++)
	{
		fscanf(fp,"%d %d %d\n",&Face[i][0], &Face[i][1], &Face[i][2]);
	}

	//Initialize topology information
	Container=new TopologyContainer;
	Container->init(&Point0, &Point, &Face, &Edge);
	Modifier=new TopologyModifier;

	//face normal vector °è»ê
	computeFaceNormal();
	computeCenterPoint();
	fclose(fp);
}

void SurfaceObj::writeObjData(char* filename)
{
	FILE *fp;
	int i;

	fp = fopen(filename,"w");

	fprintf(fp,"%d\n",Point.size());

	for(i=0;i<Point.size();i++)
		fprintf(fp,"%f %f %f\n",Point[i][0], Point[i][1], Point[i][2]);

	fprintf(fp,"%d\n",Face.size());

	for(i=0;i<Face.size();i++)
		fprintf(fp,"%d %d %d\n",Face[i][0], Face[i][1], Face[i][2]);

	fclose(fp);
}

void SurfaceObj::drawObject() 
{
	int i;

	glBegin(GL_TRIANGLES);
	for(i=0;i<Face.size();i++)
	{
		//glNormal3f((GLfloat)FaceNormal[i][0],(GLfloat)FaceNormal[i][1],(GLfloat)FaceNormal[i][2]);
		glNormal3f((GLfloat)PointNormal[Face[i][0]][0], (GLfloat)PointNormal[Face[i][0]][1], (GLfloat)PointNormal[Face[i][0]][2]);
		glVertex3f((GLfloat)Point[Face[i][0]][0], (GLfloat)Point[Face[i][0]][1], (GLfloat)Point[Face[i][0]][2]);
		glNormal3f((GLfloat)PointNormal[Face[i][1]][0], (GLfloat)PointNormal[Face[i][1]][1], (GLfloat)PointNormal[Face[i][1]][2]);
		glVertex3f((GLfloat)Point[Face[i][1]][0], (GLfloat)Point[Face[i][1]][1], (GLfloat)Point[Face[i][1]][2]);
		glNormal3f((GLfloat)PointNormal[Face[i][2]][0], (GLfloat)PointNormal[Face[i][2]][1], (GLfloat)PointNormal[Face[i][2]][2]);
		glVertex3f((GLfloat)Point[Face[i][2]][0], (GLfloat)Point[Face[i][2]][1], (GLfloat)Point[Face[i][2]][2]);
	}
	glEnd();
}

void SurfaceObj::drawPoints()
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();

	glColor3f(1,0,0);
	for(int i=0;i<Point.size();i++)
	{
		glPushMatrix();
		glTranslatef(Point[i][0],Point[i][1],Point[i][2]);
		gluSphere(qobj,2,20,20);
		glPopMatrix();
	}
}
void SurfaceObj::drawPointIdx()
{
	for (int i = 0; i < Point.size(); i++)
	{
		Vec3f curP = Point[i];
		
		//Utility::printw(curP[0], curP[1], curP[2], "  %d", i);
	}
}
void SurfaceObj::drawWireFrame() 
{
	glBegin(GL_LINES);

	for(int i=0;i<Edge.size();i++)
	{
		glVertex3f((GLfloat)Point[Edge[i][0]][0],(GLfloat)Point[Edge[i][0]][1],(GLfloat)Point[Edge[i][0]][2]);
		glVertex3f((GLfloat)Point[Edge[i][1]][0],(GLfloat)Point[Edge[i][1]][1],(GLfloat)Point[Edge[i][1]][2]);
	}
	glEnd();
}

void SurfaceObj::drawPointsAroundPoint(int idx)
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();

	glColor3f(1,0,0);
	glPushMatrix();
	glTranslatef(Point[idx][0],Point[idx][1],Point[idx][2]);
	gluSphere(qobj,1,20,20);
	glPopMatrix();

	std::vector<std::vector<int>>* pointsAroundPoint=Container->pointsAroundPoint();
	for(int i=0;i<(*pointsAroundPoint)[idx].size();i++)
	{
		glColor3f(0,0,1);
		glPushMatrix();
		glTranslatef(Point[(*pointsAroundPoint)[idx][i]][0],Point[(*pointsAroundPoint)[idx][i]][1],Point[(*pointsAroundPoint)[idx][i]][2]);
		gluSphere(qobj,1,20,20);
		glPopMatrix();
	}
}

void SurfaceObj::drawEdgesAroundPoint(int idx)
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();

	glColor3f(1,0,0);
	glPushMatrix();
	glTranslatef(Point[idx][0],Point[idx][1],Point[idx][2]);
	gluSphere(qobj,1,20,20);
	glPopMatrix();

	std::vector<std::vector<int>>* edgesAroundPoint=Container->edgesAroundPoint();
	glLineWidth(2.0);
	glBegin(GL_LINES);
	for(int i=0;i<(*edgesAroundPoint)[idx].size();i++)
	{
		Vec2i edge=Edge[(*edgesAroundPoint)[idx][i]];
		glVertex3f(Point[edge[0]][0],Point[edge[0]][1],Point[edge[0]][2]);
		glVertex3f(Point[edge[1]][0],Point[edge[1]][1],Point[edge[1]][2]);
	}
	glEnd();
}

void SurfaceObj::drawFacesAroundPoint(int idx)
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();

	glColor3f(1,0,0);
	glPushMatrix();
	glTranslatef(Point[idx][0],Point[idx][1],Point[idx][2]);
	gluSphere(qobj,3,20,20);
	glPopMatrix();

	std::vector<std::vector<int>>* facesAroundPoint=Container->facesAroundPoint();
	glBegin(GL_TRIANGLES);
	for(int i=0;i<(*facesAroundPoint)[idx].size();i++)
	{
		Vec3i face=Face[(*facesAroundPoint)[idx][i]];
		glVertex3f(Point[face[0]][0],Point[face[0]][1],Point[face[0]][2]);
		glVertex3f(Point[face[1]][0],Point[face[1]][1],Point[face[1]][2]);
		glVertex3f(Point[face[2]][0],Point[face[2]][1],Point[face[2]][2]);

		glVertex3f(Point[face[0]][0],Point[face[0]][1],Point[face[0]][2]);
		glVertex3f(Point[face[2]][0],Point[face[2]][1],Point[face[2]][2]);
		glVertex3f(Point[face[1]][0],Point[face[1]][1],Point[face[1]][2]);
	}
	glEnd();
}

void SurfaceObj::drawFacesAroundEdge(int idx)
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();

	glColor3f(1,0,0);
	for(int i=0;i<2;i++)
	{
		glPushMatrix();
		glTranslatef(Point[Edge[idx][i]][0],Point[Edge[idx][i]][1],Point[Edge[idx][i]][2]);
		gluSphere(qobj,5,20,20);
		glPopMatrix();
	}

	std::vector<std::vector<int>>* facesAroundEdge=Container->facesAroundEdge();
	glBegin(GL_TRIANGLES);
	for(int i=0;i<(*facesAroundEdge)[idx].size();i++)
	{
		Vec3i face=Face[(*facesAroundEdge)[idx][i]];
		glVertex3f(Point[face[0]][0],Point[face[0]][1],Point[face[0]][2]);
		glVertex3f(Point[face[1]][0],Point[face[1]][1],Point[face[1]][2]);
		glVertex3f(Point[face[2]][0],Point[face[2]][1],Point[face[2]][2]);

		glVertex3f(Point[face[0]][0],Point[face[0]][1],Point[face[0]][2]);
		glVertex3f(Point[face[2]][0],Point[face[2]][1],Point[face[2]][2]);
		glVertex3f(Point[face[1]][0],Point[face[1]][1],Point[face[1]][2]);
	}
	glEnd();
}

void SurfaceObj::drawEdgesInFace(int idx)
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();

	glColor3f(1,0,0);
	std::vector<std::vector<int>>* edgesInFace=Container->edgesInFace();
	glLineWidth(3.0);
	glBegin(GL_LINES);
	for(int i=0;i<(*edgesInFace)[idx].size();i++)
	{
		Vec2i edge=Edge[(*edgesInFace)[idx][i]];
		glVertex3f(Point[edge[0]][0],Point[edge[0]][1],Point[edge[0]][2]);
		glVertex3f(Point[edge[1]][0],Point[edge[1]][1],Point[edge[1]][2]);
	}
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_TRIANGLES);
	for(int i=0;i<3;i++)
	{
		glVertex3f(Point[Face[idx][0]][0],Point[Face[idx][0]][1],Point[Face[idx][0]][2]);
		glVertex3f(Point[Face[idx][1]][0],Point[Face[idx][1]][1],Point[Face[idx][1]][2]);
		glVertex3f(Point[Face[idx][2]][0],Point[Face[idx][2]][1],Point[Face[idx][2]][2]);
	}
	glEnd();
}

void SurfaceObj::drawFace(int idx)
{
	glBegin(GL_TRIANGLES);
	glVertex3f(Point[Face[idx][0]][0],Point[Face[idx][0]][1],Point[Face[idx][0]][2]);
	glVertex3f(Point[Face[idx][1]][0],Point[Face[idx][1]][1],Point[Face[idx][1]][2]);
	glVertex3f(Point[Face[idx][2]][0],Point[Face[idx][2]][1],Point[Face[idx][2]][2]);

	glVertex3f(Point[Face[idx][0]][0],Point[Face[idx][0]][1],Point[Face[idx][0]][2]);
	glVertex3f(Point[Face[idx][2]][0],Point[Face[idx][2]][1],Point[Face[idx][2]][2]);
	glVertex3f(Point[Face[idx][1]][0],Point[Face[idx][1]][1],Point[Face[idx][1]][2]);
	glEnd();
}

void SurfaceObj::drawBVH(int depth)
{
	BVHAABB->drawBoundingBoxTo(depth);
}

void SurfaceObj::drawBVH()
{
	if (BVHAABB)
	{
		BVHAABB->drawBoundingBox();
	}

}

void SurfaceObj::computeFaceNormal()
{
	int i;
	Vec3f vector1, vector2;

	for(i=0;i<FaceNormal.size();i++)
	{
		vector1=Point[Face[i][1]]-Point[Face[i][0]]; 
		vector2=Point[Face[i][2]]-Point[Face[i][0]]; 
		FaceNormal[i] = vector1.cross(vector2);
		FaceNormal[i].normalize();
	}
	computePointNormal();
}

Vec3f SurfaceObj::computeFaceNormal(Vec3f p1, Vec3f p2, Vec3f p3)
{
	Vec3f vector1, vector2;
	Vec3f normal;
	vector1=p2-p1; 
	vector2=p3-p1; 
	normal = vector1.cross(vector2);
	normal.normalize();
	return normal;
}

void SurfaceObj::updatePoint()
{
	for(int i=0;i<Point.size();i++)
	{
		Point[i]=Point0[i]+Dis[i];
	}
	updateNormal();
}

void SurfaceObj::updateNormal()
{
	FaceNormal.resize(Face.size());
	PointNormal.resize(Point.size());
	computeFaceNormal();
}

void SurfaceObj::updateBVH()
{
	BVHAABB->updateAABBTreeBottomUp();
}

void SurfaceObj::computePointNormal()
{
	Vec3f sum;
	std::vector<std::vector<int>>* facesAroundPoint=Container->facesAroundPoint();

	for(int i=0;i<Point.size();i++)
	{
		sum.clear();
		for(int j=0;j<(*facesAroundPoint)[i].size();j++)
		{
			sum+=FaceNormal[(*facesAroundPoint)[i][j]];
		}

		//sum.normalize();
		sum=sum/(*facesAroundPoint)[i].size();
		PointNormal[i]=sum;
	}
}

std::vector<Vec3f>*	SurfaceObj::point0()
{
	return &Point0;
}
std::vector<Vec3f>*	SurfaceObj::point()
{
	return &Point;
}
std::vector<Vec3f>*	SurfaceObj::dis()
{
	return &Dis;
}
std::vector<Vec3i>*	SurfaceObj::face()
{
	return &Face;
}
std::vector<Vec2i>*	SurfaceObj::edge()
{
	return &Edge;
}
std::vector<Vec3f>*	SurfaceObj::faceNormal()
{
	return &FaceNormal;
}
std::vector<Vec3f>* SurfaceObj::pointNormal()
{
	return &PointNormal;
}
Vec3f SurfaceObj::midPoint()
{
	return MidPoint;
}
TopologyContainer* SurfaceObj::container()
{
	return Container;
}
TopologyModifier* SurfaceObj::modifier()
{
	return Modifier;
}

AABBTree* SurfaceObj::getBVH()
{
	return BVHAABB;
}

int SurfaceObj::nbPoint()
{
	return Point.size();
}
int SurfaceObj::nbFace()
{
	return Face.size();
}

void SurfaceObj::rotate(Vec3f axis, float angle)
{
	Mat3x3f rot;
	computeRotationMatrix(axis, angle, rot);

	for(int i=0;i<Point.size();i++)
		Point[i]=rot*Point0[i];
	computeFaceNormal();
	computeCenterPoint();
}

void SurfaceObj::rotateC(Vec3f axis, float angle)
{
	Mat3x3f rot;
	computeRotationMatrix(axis, angle, rot);

	for(int i=0;i<Point.size();i++)
		Point[i]=rot*Point[i];
	computeFaceNormal();
	computeCenterPoint();
};

void SurfaceObj::rotate(float* rot)
{
	Mat3x3f Rot;
	Rot(0,0)=rot[0]; Rot(0,1)=rot[1]; Rot(0,2)=rot[2];
	Rot(1,0)=rot[4]; Rot(1,1)=rot[5]; Rot(1,2)=rot[6];
	Rot(2,0)=rot[8]; Rot(2,1)=rot[9]; Rot(2,2)=rot[10];
	rotate(Rot);
	computeCenterPoint();
	computeFaceNormal();
};

void SurfaceObj::rotate(Quat q)
{
	for(int i=0;i<Point.size();i++)
		Point[i]=q.rotate(Point0[i]);
	computeCenterPoint();
	computeFaceNormal();
};

void SurfaceObj::rotateC(Quat q)
{
	for(int i=0;i<Point.size();i++)
		Point[i]=q.rotate(Point[i]);
	computeCenterPoint();
	computeFaceNormal();
};

void SurfaceObj::rotateC(Mat3x3f rot)
{
	for(int i=0;i<Point.size();i++)
		Point[i]=rot*Point[i];
	computeCenterPoint();
	computeFaceNormal();
};

void SurfaceObj::rotate(Mat3x3f& rot)
{
	for(int i=0;i<Point.size();i++)
		Point[i]=rot*Point0[i];
	computeFaceNormal();
	computeCenterPoint();
};

void SurfaceObj::scale(float x, float y, float z)
{
	int i;
	for(i=0;i<Point.size();i++)
	{
		Point[i][0]=Point0[i][0]*x;
		Point[i][1]=Point0[i][1]*y;
		Point[i][2]=Point0[i][2]*z;
	}
	computeFaceNormal();
	computeCenterPoint();
};

void SurfaceObj::scale(float scale)
{
	int i;
	for(i=0;i<Point.size();i++)
	{
		Point[i]=Point0[i]*scale;
	}
	computeFaceNormal();
	computeCenterPoint();
};

void SurfaceObj::scaleC(float scale)
{
	int i;
	for(i=0;i<Point.size();i++)
	{
		Point[i]=Point[i]*scale;
	}
	computeFaceNormal();
	computeCenterPoint();
};

void SurfaceObj::scaleC(float x, float y, float z)
{
	int i;
	for(i=0;i<Point.size();i++)
	{
		Point[i][0]=Point[i][0]*x;
		Point[i][1]=Point[i][1]*y;
		Point[i][2]=Point[i][2]*z;
	}
	computeCenterPoint();
};

void SurfaceObj::scaleI(float x, float y, float z)
{
	int i;
	for(i=0;i<Point.size();i++)
	{
		Point0[i][0]=Point0[i][0]*x;
		Point0[i][1]=Point0[i][1]*y;
		Point0[i][2]=Point0[i][2]*z;
	}
	computeCenterPoint();
};

void SurfaceObj::scaleI(float scale)
{
	int i;
	for(i=0;i<Point.size();i++)
	{
		Point0[i]=Point0[i]*scale;
	}
	computeCenterPoint();
};

void SurfaceObj::translate(Vec3f trans)
{
	int i;

	for(i=0;i<Point.size();i++)
		Point[i]=Point0[i]+trans;
	computeCenterPoint();
};

void SurfaceObj::translateC(Vec3f trans)
{
	int i;

	for(i=0;i<Point.size();i++)
		Point[i]=Point[i]+trans;
	computeCenterPoint();
};

void SurfaceObj::translate(float x, float y, float z)
{
	int i;
	Vec3f trans(x,y,z);

	for(i=0;i<Point.size();i++)
		Point[i]=Point0[i]+trans;
	computeCenterPoint();
};

void SurfaceObj::translateI(Vec3f trans)
{
	int i;

	for(i=0;i<Point.size();i++)
	{
		Point0[i]=Point0[i]+trans;
		Point[i]=Point0[i];
	}
};

void SurfaceObj::translateI(float x, float y, float z)
{
	int i;
	Vec3f trans(x,y,z);

	for(i=0;i<Point.size();i++)
	{
		Point0[i]=Point0[i]+trans;
		Point[i]=Point0[i];
	}
};

void SurfaceObj::moveToCenter()
{
	translate(MidPoint*-1);
	MidPoint.clear();
}

void SurfaceObj::computeRotationMatrix(Vec3f axis, float angle, Mat3x3f& rot)
{
	float x,y,z;
	x=axis[0];
	y=axis[1];
	z=axis[2];

	rot(0,0)=x*x+(y*y+z*z)*cos(angle);
	rot(1,1)=y*y+(x*x+z*z)*cos(angle);
	rot(2,2)=z*z+(x*x+y*y)*cos(angle);
	rot(0,1)=(1-cos(angle))*x*y+z*sin(angle);
	rot(1,0)=(1-cos(angle))*x*y-z*sin(angle);
	rot(0,2)=(1-cos(angle))*x*z-y*sin(angle);
	rot(2,0)=(1-cos(angle))*z*x+y*sin(angle);
	rot(1,2)=(1-cos(angle))*y*z+x*sin(angle);
	rot(2,1)=(1-cos(angle))*z*y-x*sin(angle);

	rot.transpose();
};

void SurfaceObj::computeCenterPoint()
{
	int i;
	Vec3f ld(MAX, MAX, MAX);
	Vec3f ru(MIN, MIN, MIN);
	for (i = 0; i < Point.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (ld[j] > Point[i][j])
			{
				ld[j] = Point[i][j];
			}
			if (ru[j] < Point[i][j])
			{
				ru[j] = Point[i][j];
			}
		}
	}
	
	MidPoint = (ld + ru) / 2;
}

void SurfaceObj::constructAABBTree()
{
	BVHAABB=new AABBTreeTri(&Point, &Face, &Edge);
	BVHAABB->constructAABBTree();
}

void SurfaceObj::increaseResolution()
{
	std::vector<Vec3i> addedFace;
	for(int i=0;i<Face.size();i++)
	{
		Vec3f point=(Point[Face[i][0]]+Point[Face[i][1]]+Point[Face[i][2]])/3.0;
		int pointIdx=Point.size();
		Point.push_back(point);

		Vec3i face[3];
		face[0]=Vec3i(Face[i][0], Face[i][1], pointIdx);
		face[1]=Vec3i(Face[i][1], Face[i][2], pointIdx);
		face[2]=Vec3i(Face[i][2], Face[i][0], pointIdx);
		addedFace.push_back(face[0]);
		addedFace.push_back(face[1]);
		addedFace.push_back(face[2]);
	}
	for(int i=0;i<addedFace.size();i++)
		Face.push_back(addedFace[i]);
}

void SurfaceObj::drawFaceIdx()
{
	for (int i = 0; i < Face.size(); i++)
	{
		Vec3i curF = Face[i];
		Vec3f curP = (Point[curF[0]] + Point[curF[1]] + Point[curF[2]] )/3;
	//	Utility::printw(curP[0], curP[1], curP[2], "  %d", i);
	}
}

void SurfaceObj::displacePoint( arrayVec3f &pointDisplace, arrayInt &ptIdx )
{
	for (int i=0; i<ptIdx.size(); i++)
	{
		Point[ptIdx[i]] += pointDisplace[i];
	}
}

// Draw object surface in original coordinate
void SurfaceObj::drawObject0()
{
	int i;
	glBegin(GL_TRIANGLES);
	for(i=0;i<Face.size();i++)
	{
		for (int j=0; j<3; j++)
		{
			glVertex3fv((GLfloat*)Point0[Face[i][j]].ptr() );
		}
	}
	glEnd();
}

// Draw object's wire frame in original coordinate
void SurfaceObj::drawWireFrame0()
{
	glLineWidth(1.0);

	glBegin(GL_LINES);

	for(unsigned int i=0;i<Edge.size();i++)
	{
		glVertex3fv((GLfloat*)Point0[Edge[i][0]].ptr());
		glVertex3fv((GLfloat*)Point0[Edge[i][1]].ptr());
	}
	glEnd();
}

void SurfaceObj::readObjDataSTL(char * filePath)
{
	// Load STL
	CSTL stl;
	stl.ReadData(filePath);

	// Assign Points
	Point0.clear();
	vec3d* pointSTL = stl.GetPoint();
	for (int i = 0; i < stl.GetPointNum(); i++)
	{
		Point0.push_back(Vec3f(pointSTL[i][0], pointSTL[i][1], pointSTL[i][2]));
	}
	Point = Point0;

	// Assign Faces
	Face.clear();
	vec3i* faceSTL = stl.GetFace();
	for (int i = 0; i < stl.GetFaceNum(); i++)
	{
		Face.push_back(Vec3i(faceSTL[i][0], faceSTL[i][1], faceSTL[i][2]));
	}

	//Initialize topology information
	Container=new TopologyContainer;
	Container->init(&Point0, &Point, &Face, &Edge);
	Modifier=new TopologyModifier;

	//face normal vector 
	FaceNormal.resize(Face.size());
	PointNormal.resize(Point.size());

	computeFaceNormal();
	computeCenterPoint();
}

void SurfaceObj::init(std::vector<Vec3f> _point, std::vector<Vec3i> _face)
{
	Point = _point;
	Face = _face;

	//Initialize topology information
	Container=new TopologyContainer;
	Container->init(&Point0, &Point, &Face, &Edge);
	Modifier=new TopologyModifier;

	//face normal vector 
	FaceNormal.resize(Face.size());
	PointNormal.resize(Point.size());

	computeFaceNormal();
	computeCenterPoint();
}

bool SurfaceObj::isIntersectWithBox(Box box, bool isInside)
{
	return false;
}

void SurfaceObj::writeSTLData(char * filePath)
{
	ASSERT(0);
}

void SurfaceObj::centerlize()
{
	translate(-MidPoint[0], -MidPoint[1], -MidPoint[2]);
}
