#if !defined(_VOLUMEDEFORMOBJ_H_)
#define _VOLUMEDEFORMOBJ_H_

#include "DataStruct.h"
#include "Obj.h"

class CVolumeDeformObj : public CObj  
{
private:
	int		m_SurfacePointNum;
	int		m_InnerPointNum;
	int		m_TetraNum;

	vec3d*	m_DisVector;
	vec4i*	m_Tetra;

	matrix	m_GU;
	matrix	m_GF;
	matrix	m_GK;
	matrix	m_GKI;

	//boundary condition
	int m_BoundaryPointNum;
	int* m_BoundaryPoint;

public:
	CVolumeDeformObj()
	{
		m_SurfacePointNum=0;
		m_InnerPointNum=0;
		m_BoundaryPointNum=0;
		m_TetraNum=0;

		m_DisVector=NULL;
		m_InitialPoint=NULL;
		m_Tetra=NULL;
		m_BoundaryPoint=NULL;
	};
	
	~CVolumeDeformObj()
	{
		if(m_BoundaryPoint)
		{
			delete [] m_BoundaryPoint;
			m_BoundaryPoint=NULL;
		}

		if(m_DisVector)
		{
			delete [] m_DisVector;
			m_DisVector=NULL;
		}
		if(m_InitialPoint)
		{
			delete [] m_InitialPoint;
			m_InitialPoint=NULL;
		}
		if(m_Tetra)
		{
			delete [] m_Tetra;
			m_Tetra=NULL;
		}
		DeleteNeiFace();
	};

	int SurfacePointNum() {return m_SurfacePointNum;};

	vec3d* InitialPoint()
	{
		return m_InitialPoint;
	};

	void DeleteNeiFace()
	{
		if(m_PointNeiFace)
		{
			for(int i=0;i<m_SurfacePointNum;i++)
			{
				if(m_PointNeiFace[i].FaceIndex)
				{
					delete [] m_PointNeiFace[i].FaceIndex;
					m_PointNeiFace[i].FaceIndex=NULL;
				}
			}
			delete [] m_PointNeiFace;
			m_PointNeiFace=NULL;
		}
	};
	
	int ReadObjData(char* filename)
	{
		FILE *fp;
		int i;

		fp = fopen(filename,"r");
		
		fscanf(fp,"%d\n",&m_PointNum);
		m_Point = new vec3d[m_PointNum];
		m_PrePoint = new vec3d[m_PointNum];
		m_InitialPoint = new vec3d[m_PointNum];
		
		for(i=0;i<m_PointNum;i++)
		{
			fscanf(fp,"%lf %lf %lf\n",&m_Point[i].x, &m_Point[i].y, &m_Point[i].z);
			m_InitialPoint[i]=m_Point[i];
			m_PrePoint[i]=m_Point[i];
		}
		
		fscanf(fp,"%d\n",&m_FaceNum);
		m_Face = new vec3i[m_FaceNum];

		for(i=0;i<m_FaceNum;i++)
			fscanf(fp,"%d %d %d\n",&m_Face[i].ii, &m_Face[i].jj, &m_Face[i].kk);

		fscanf(fp,"%d\n",&m_TetraNum);
		m_Tetra = new vec4i[m_TetraNum];

		for(i=0;i<m_TetraNum;i++)
			fscanf(fp,"%d %d %d %d\n",&m_Tetra[i].ii,&m_Tetra[i].jj,&m_Tetra[i].kk,&m_Tetra[i].ww);
		
		CountSurfacePointNum();
		ComputeCenterPoint();
		ComputeFaceNormalVector();
		ComputePointNormalVector();

		fclose(fp);
		return 1;
	};

	void ReadCondensedStiffMatrix(char* filename)
	{
		m_GF.init(m_SurfacePointNum*3,1);
		m_GU.init(m_SurfacePointNum*3,1);
		m_GKI.Read(filename);
	};

	void ReadBoundaryCondition(char* filename)
	{
		int i;
		FILE* f=fopen(filename,"r");
		fscanf(f,"%d",&m_BoundaryPointNum);
		m_BoundaryPoint=new int[m_BoundaryPointNum];
		for(i=0;i<m_BoundaryPointNum;i++)
			fscanf(f,"%d",&m_BoundaryPoint[i]);
		fclose(f);
	};

	void CountSurfacePointNum()
	{
		int Max=0;
		for(int i=0;i<m_FaceNum;i++)
		{
			if(m_Face[i].ii>Max)
				Max=m_Face[i].ii;
			if(m_Face[i].jj>Max)
				Max=m_Face[i].jj;
			if(m_Face[i].kk>Max)
				Max=m_Face[i].kk;
		}
		m_SurfacePointNum=Max+1;
		m_InnerPointNum=m_PointNum-m_InnerPointNum;
	};

	void ComputeCenterPoint()
	{
		int i;
		vec3d sum;
		for(i=0;i<m_SurfacePointNum;i++)
			sum=sum+m_Point[i];
		sum=sum/m_SurfacePointNum;
		m_MidPoint=sum;
	};

	int ComputePointNormalVector()
	{
		int i,j;
		m_PointNeiFace=new NeighborFace[m_SurfacePointNum];
		m_PointNormal=new vec3d[m_SurfacePointNum];

		//initialize
		for(i=0;i<m_SurfacePointNum;i++)
		{
			m_PointNeiFace[i].FaceNum=0;
			m_PointNeiFace[i].FaceIndex=NULL;
		}

		//point 주변의 면들의 개수를 저장한다
		for(i=0;i<m_FaceNum;i++)
		{
			m_PointNeiFace[m_Face[i].ii].FaceNum++;
			m_PointNeiFace[m_Face[i].jj].FaceNum++;
			m_PointNeiFace[m_Face[i].kk].FaceNum++;
		}

		//메모리 할당
		for(i=0;i<m_SurfacePointNum;i++)
		{
			m_PointNeiFace[i].FaceIndex=new int[m_PointNeiFace[i].FaceNum];
			m_PointNeiFace[i].FaceNum=0;
		}

		//한 point 주변의 면들의 face index를 저장한다
		for(i=0;i<m_FaceNum;i++)
		{
			m_PointNeiFace[m_Face[i].ii].FaceIndex[m_PointNeiFace[m_Face[i].ii].FaceNum]=i;
			m_PointNeiFace[m_Face[i].ii].FaceNum++;

			m_PointNeiFace[m_Face[i].jj].FaceIndex[m_PointNeiFace[m_Face[i].jj].FaceNum]=i;
			m_PointNeiFace[m_Face[i].jj].FaceNum++;

			m_PointNeiFace[m_Face[i].kk].FaceIndex[m_PointNeiFace[m_Face[i].kk].FaceNum]=i;
			m_PointNeiFace[m_Face[i].kk].FaceNum++;
		}

		//주변면들의 normal vector를 평균하여 point normal vector를 계산한다
		for(i=0;i<m_SurfacePointNum;i++)
		{
			for(j=0;j<m_PointNeiFace[i].FaceNum;j++)
				m_PointNormal[i]=m_PointNormal[i]+m_FaceNormal[m_PointNeiFace[i].FaceIndex[j]];
			m_PointNormal[i]=m_PointNormal[i]/(float)m_PointNeiFace[i].FaceNum;
			m_PointNormal[i].normalize();
		}
		return 1;
	};

	void ScaleObj(float scale)
	{
		int i;
		for(i=0;i<m_PointNum;i++)
		{
			m_Point[i]=m_Point[i]*scale;
			m_InitialPoint[i]=m_Point[i];
		}

		ComputeCenterPoint();
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	void Transformation(matrix transform)
	{
		//이전 스텝의 point 좌표 저장
		UpdatePreviousPoint();

		for(int i=0;i<m_SurfacePointNum;i++)
		{
			m_Point[i].x=transform(0,0)*m_InitialPoint[i].x+transform(1,0)*m_InitialPoint[i].y+transform(2,0)*m_InitialPoint[i].z+transform(3,0);
			m_Point[i].y=transform(0,1)*m_InitialPoint[i].x+transform(1,1)*m_InitialPoint[i].y+transform(2,1)*m_InitialPoint[i].z+transform(3,1);
			m_Point[i].z=transform(0,2)*m_InitialPoint[i].x+transform(1,2)*m_InitialPoint[i].y+transform(2,2)*m_InitialPoint[i].z+transform(3,2);
		}
		ComputeCenterPoint();
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};
	
	int UpdatePointNormalVector()
	{
		//주변면들의 normal vector를 평균하여 point normal vector를 계산한다
		for(int i=0;i<m_SurfacePointNum;i++)
		{
			m_PointNormal[i].setValue(0,0,0);
			for(int j=0;j<m_PointNeiFace[i].FaceNum;j++)
				m_PointNormal[i]=m_PointNormal[i]+m_FaceNormal[m_PointNeiFace[i].FaceIndex[j]];
			m_PointNormal[i]=m_PointNormal[i]/(float)m_PointNeiFace[i].FaceNum;
			m_PointNormal[i].normalize();
		}
		return 1;
	};

	int DrawObject(int mode, vec3d color) 
	{
		//mode=1: surface rendering
		//mode=2; wireframe rendering
		//mode=3: surface rendering+wireframe rendering
		//mode=4; wireframe volume rendering
		int i;
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glColor3f(color.x,color.y,color.z);
		if(mode==1 || mode==3)
		{
			glBegin(GL_TRIANGLES);
			for(i=0;i<m_FaceNum;i++)
			{
			//	glNormal3f(m_FaceNormal[i].x,m_FaceNormal[i].y,m_FaceNormal[i].z);
				glNormal3f(m_PointNormal[m_Face[i].ii].x,m_PointNormal[m_Face[i].ii].y,m_PointNormal[m_Face[i].ii].z);
				glVertex3f(m_Point[m_Face[i].ii].x, m_Point[m_Face[i].ii].y, m_Point[m_Face[i].ii].z);
				glNormal3f(m_PointNormal[m_Face[i].jj].x,m_PointNormal[m_Face[i].jj].y,m_PointNormal[m_Face[i].jj].z);
				glVertex3f(m_Point[m_Face[i].jj].x, m_Point[m_Face[i].jj].y, m_Point[m_Face[i].jj].z);
				glNormal3f(m_PointNormal[m_Face[i].kk].x,m_PointNormal[m_Face[i].kk].y,m_PointNormal[m_Face[i].kk].z);
				glVertex3f(m_Point[m_Face[i].kk].x, m_Point[m_Face[i].kk].y, m_Point[m_Face[i].kk].z);
			}
			glEnd();
		}
		
		if(mode==2 || mode==3)
		{
			if(mode==2)
				glColor3f(1,1,1);
			if(mode==3)
				glColor3f(1,1,1);
			for(i=0;i<m_FaceNum;i++)
			{
				glBegin(GL_LINE_STRIP);
				glVertex3f(m_Point[m_Face[i].ii].x, m_Point[m_Face[i].ii].y, m_Point[m_Face[i].ii].z);
				glVertex3f(m_Point[m_Face[i].jj].x, m_Point[m_Face[i].jj].y, m_Point[m_Face[i].jj].z);
				glVertex3f(m_Point[m_Face[i].kk].x, m_Point[m_Face[i].kk].y, m_Point[m_Face[i].kk].z);
				glVertex3f(m_Point[m_Face[i].ii].x, m_Point[m_Face[i].ii].y, m_Point[m_Face[i].ii].z);
				glEnd();
			}
		}

		if(mode==4)
		{
			glColor3f(1,1,1);
			for(i=0;i<m_TetraNum;i++)
			{
				glBegin(GL_LINE_STRIP);
				glVertex3f(m_Point[m_Tetra[i].ii].x, m_Point[m_Tetra[i].ii].y, m_Point[m_Tetra[i].ii].z);
				glVertex3f(m_Point[m_Tetra[i].jj].x, m_Point[m_Tetra[i].jj].y, m_Point[m_Tetra[i].jj].z);
				glVertex3f(m_Point[m_Tetra[i].kk].x, m_Point[m_Tetra[i].kk].y, m_Point[m_Tetra[i].kk].z);
				glVertex3f(m_Point[m_Tetra[i].ww].x, m_Point[m_Tetra[i].ww].y, m_Point[m_Tetra[i].ww].z);
				glVertex3f(m_Point[m_Tetra[i].ii].x, m_Point[m_Tetra[i].ii].y, m_Point[m_Tetra[i].ii].z);
				glEnd();

				glBegin(GL_LINES);
				glVertex3f(m_Point[m_Tetra[i].ii].x, m_Point[m_Tetra[i].ii].y, m_Point[m_Tetra[i].ii].z);
				glVertex3f(m_Point[m_Tetra[i].kk].x, m_Point[m_Tetra[i].kk].y, m_Point[m_Tetra[i].kk].z);
				glVertex3f(m_Point[m_Tetra[i].ww].x, m_Point[m_Tetra[i].ww].y, m_Point[m_Tetra[i].ww].z);
				glVertex3f(m_Point[m_Tetra[i].jj].x, m_Point[m_Tetra[i].jj].y, m_Point[m_Tetra[i].jj].z);
				glEnd();
			}
		}
		glPopAttrib();
		return 1;
	};

	void DrawBoundaryCondition(double radius)
	{
		GLUquadricObj *qobj = 0;
		qobj = gluNewQuadric();
		int i;

		glColor3f(1,0,0);
		for(i=0;i<m_BoundaryPointNum;i++)
		{
			glPushMatrix();
			glTranslatef(m_Point[m_BoundaryPoint[i]].x,m_Point[m_BoundaryPoint[i]].y,m_Point[m_BoundaryPoint[i]].z);
			gluSphere(qobj,radius,20,20);
			glPopMatrix();
		}
	};

	void ComputeCondensedForceDisplacement(int* ConPointIndex, vec3d* DisVector, int ConPointNum, vec3d* ConForce)
	{
		int i,j,k,l;

		//contact point index를 sorting 한다
		int* copyConPoint=new int[ConPointNum];
		std::copy (ConPointIndex, ConPointIndex+ConPointNum, copyConPoint);
		sort indexSort;
		indexSort.Sort(copyConPoint,ConPointNum);
		
		//contact point의 force
		matrix Fcontact(ConPointNum*3,1);

		//contact point의 displacement
		matrix Ucontact(ConPointNum*3,1);
		for(i=0;i<ConPointNum;i++)
		{
			Ucontact(i*3,0)=DisVector[i].x;
			Ucontact(i*3+1,0)=DisVector[i].y;
			Ucontact(i*3+2,0)=DisVector[i].z;

			m_GU(ConPointIndex[i]*3,0)=DisVector[i].x;
			m_GU(ConPointIndex[i]*3+1,0)=DisVector[i].y;
			m_GU(ConPointIndex[i]*3+2,0)=DisVector[i].z;
		}

		//f(contact)=Kbb-1*u(contact)
		matrix Kbb(ConPointNum*3, ConPointNum*3);

		//Kbb matrix construction
		for(i=0;i<ConPointNum;i++)
		{
			int rowIndex=ConPointIndex[i]*3;
			for(j=0;j<ConPointNum;j++)
			{
				int colIndex=ConPointIndex[j]*3;
				for(k=0;k<3;k++)
				{
					for(l=0;l<3;l++)
						Kbb(i*3+k,j*3+l)=m_GKI(rowIndex+k,colIndex+l);
				}
			}
		}

		matrix KbbI=Kbb.inverse();
		Fcontact=KbbI*Ucontact;

		//no contact displacement를 계산한다
		int currentIndex=0;
		for(i=0;i<m_SurfacePointNum;i++)
		{
			//no contact point에 대해서만 계산
			if(copyConPoint[currentIndex]==i)
				currentIndex++;
			else
			{
				for(l=0;l<3;l++)
				{
					double sum=0;
					for(j=0;j<ConPointNum;j++)
					{
						int index=ConPointIndex[j];
						for(k=0;k<3;k++)
							sum+=m_GKI(i*3+l,index*3+k)*Fcontact(j*3+k,0);
					}
					m_GU[i*3+l]=sum;
				}
			}
		}

		//contact force를 넣는다
		for(i=0;i<ConPointNum;i++)
		{
			ConForce[i].x=Fcontact(i*3,0);
			ConForce[i].y=Fcontact(i*3+1,0);
			ConForce[i].z=Fcontact(i*3+2,0);
		}

		delete [] copyConPoint;
	};

	void UpdatePosition()
	{
		int i;
		for(i=0;i<m_SurfacePointNum;i++)
		{
			m_Point[i].x=m_InitialPoint[i].x+m_GU[i*3];
			m_Point[i].y=m_InitialPoint[i].y+m_GU[i*3+1];
			m_Point[i].z=m_InitialPoint[i].z+m_GU[i*3+2];
		}
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};
	
	void ResetPosition()
	{
		int i;
	/*	for(i=0;i<m_SurfacePointNum;i++)
		{
			m_Point[i].x=m_InitialPoint[i].x;
			m_Point[i].y=m_InitialPoint[i].y;
			m_Point[i].z=m_InitialPoint[i].z;
		}*/

		for(i=0;i<m_SurfacePointNum;i++)
		{
			m_Point[i].x=m_Point[i].x+m_GU[i*3];
			m_Point[i].y=m_Point[i].y+m_GU[i*3+1];
			m_Point[i].z=m_Point[i].z+m_GU[i*3+2];
		}
		UpdateFaceNormalVector();
		UpdatePointNormalVector();
	};

	matrix* GetInverseStiffMatrix()
	{
		return &m_GKI;
	}

	matrix* GetForceVector()
	{
		return &m_GF;
	}

	matrix* GetDisVector()
	{
		return &m_GU;
	}

	void ComputeDisplacement(matrix nodalForce, int* nodalPointIndex, int nodalPointNum)
	{
		int i,j;
		m_GF.init();
		for(i=0;i<nodalPointNum;i++)
		{
			for(j=0;j<3;j++)
				m_GF(nodalPointIndex[i]*3+j,0)=nodalForce(i*3+j,0);
		}
		m_GU=m_GKI*m_GF;
	};

	void ComputeDisplacement()
	{
		m_GU=m_GKI*m_GF;
	};
};

#endif