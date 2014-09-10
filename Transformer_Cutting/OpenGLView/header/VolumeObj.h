#if !defined(_VOLUMEOBJ_H_)
#define _VOLUMEOBJ_H_

#include "DataStruct.h"

#include "Obj.h"
#include "FEMData.h"

class CVolumeObj : public CObj  
{
private:

	CTetra* m_Tetra;
	CEdge* m_Edge;
	CVertex* m_Vertex;

	int m_TetraNum;
	int m_EdgeNum;

	int m_BoundaryPointNum;
	int* m_BoundaryPoint;

	double m_MassCoeff;
	double m_DampCoeff;

	//global stiffness matrix
	matrix m_GK;
	matrix m_GF;
	matrix m_GU;
	matrix m_GKI;

public:
	CVolumeObj()
	{
		m_Tetra=NULL;
		m_Vertex=NULL;
		m_Edge=NULL;
		m_BoundaryPoint=NULL;

		m_BoundaryPointNum=0;
		m_TetraNum=0;
		m_EdgeNum=0;

		m_MassCoeff=0.01;
		m_DampCoeff=0.05;
	};

	~CVolumeObj()
	{
		if(m_Tetra)
		{
			delete [] m_Tetra;
			m_Tetra=NULL;
		}

		if(m_Vertex)
		{
			delete [] m_Vertex;
			m_Vertex=NULL;
		}

		if(m_Edge)
		{
			delete [] m_Edge;
			m_Edge=NULL;
		}

		if(m_BoundaryPoint)
		{
			delete [] m_BoundaryPoint;
			m_BoundaryPoint=NULL;
		}
	};

	CTetra* Tetra() {return m_Tetra;};
	CEdge* Edge() {return m_Edge;};
	CVertex* Vertex() {return m_Vertex;};

	int TetraNum() {return m_TetraNum;};
	int EdgeNum() {return m_EdgeNum;};
	double MassCoeff() {return m_MassCoeff;};
	double DampingCoeff() {return m_DampCoeff;};

	int ReadObjData(char* filename)
	{
		FILE *fp;
		int i,j;

		fp = fopen(filename,"r");
		
		fscanf(fp,"%d\n",&m_PointNum);
		m_Point = new vec3d[m_PointNum];
		m_Vertex=new CVertex[m_PointNum];
		
		for(i=0;i<m_PointNum;i++)
		{
			fscanf(fp,"%lf %lf %lf\n",&m_Point[i].x, &m_Point[i].y, &m_Point[i].z);
			m_Vertex[i].p=m_Point[i];
		}
		
		fscanf(fp,"%d\n",&m_FaceNum);
		m_Face = new vec3i[m_FaceNum];

		for(i=0;i<m_FaceNum;i++)
			fscanf(fp,"%d %d %d\n",&m_Face[i].ii, &m_Face[i].jj, &m_Face[i].kk);

		fscanf(fp,"%d\n",&m_TetraNum);
		m_Tetra = new CTetra[m_TetraNum];

		for(i=0;i<m_TetraNum;i++)
		{
			fscanf(fp,"%d %d %d %d\n",&m_Tetra[i].VertexIndex[0],&m_Tetra[i].VertexIndex[1],&m_Tetra[i].VertexIndex[2],&m_Tetra[i].VertexIndex[3]);
			for(j=0;j<4;j++)
				m_Tetra[i].VertexPos[j]=m_Point[m_Tetra[i].VertexIndex[j]];
		}
		
		ComputeCenterPoint();
		ComputeFaceNormalVector();
		ComputePointNormalVector();
		fclose(fp);
		return 1;
	};

	int DrawObject(int mode, vec3d color)
	{
		//mode=1: surface rendering
		//mode=2; wireframe rendering
		//mode=3: surface rendering+wireframe rendering
		//mode=4: volume rendering(wireframe rendering)
		int i;

		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		if(mode==1 || mode==3)
		{
			glColor3f(color.x, color.y, color.z);
			glBegin(GL_TRIANGLES);
				for(i=0;i<m_FaceNum;i++)
				{
					glNormal3f(m_FaceNormal[i].x,m_FaceNormal[i].y,m_FaceNormal[i].z);
					glVertex3f(m_Vertex[m_Face[i].ii].p.x, m_Vertex[m_Face[i].ii].p.y, m_Vertex[m_Face[i].ii].p.z);
					glVertex3f(m_Vertex[m_Face[i].jj].p.x, m_Vertex[m_Face[i].jj].p.y, m_Vertex[m_Face[i].jj].p.z);
					glVertex3f(m_Vertex[m_Face[i].kk].p.x, m_Vertex[m_Face[i].kk].p.y, m_Vertex[m_Face[i].kk].p.z);
				}
			glEnd();
		}

		if(mode==2)
		{
			glColor3f(1,1,1);
			for(i=0;i<m_FaceNum;i++)
			{
				glLineWidth(3);
				glBegin(GL_LINE_STRIP);
				glVertex3f(m_Vertex[m_Face[i].ii].p.x, m_Vertex[m_Face[i].ii].p.y, m_Vertex[m_Face[i].ii].p.z);
				glVertex3f(m_Vertex[m_Face[i].jj].p.x, m_Vertex[m_Face[i].jj].p.y, m_Vertex[m_Face[i].jj].p.z);
				glVertex3f(m_Vertex[m_Face[i].kk].p.x, m_Vertex[m_Face[i].kk].p.y, m_Vertex[m_Face[i].kk].p.z);
				glVertex3f(m_Vertex[m_Face[i].ii].p.x, m_Vertex[m_Face[i].ii].p.y, m_Vertex[m_Face[i].ii].p.z);
				glEnd();
			}
		}

		if(mode==3)
		{
			glColor3f(0,0,0);
			for(i=0;i<m_FaceNum;i++)
			{
				glBegin(GL_LINE_STRIP);
				glVertex3f(m_Vertex[m_Face[i].ii].p.x, m_Vertex[m_Face[i].ii].p.y, m_Vertex[m_Face[i].ii].p.z);
				glVertex3f(m_Vertex[m_Face[i].jj].p.x, m_Vertex[m_Face[i].jj].p.y, m_Vertex[m_Face[i].jj].p.z);
				glVertex3f(m_Vertex[m_Face[i].kk].p.x, m_Vertex[m_Face[i].kk].p.y, m_Vertex[m_Face[i].kk].p.z);
				glVertex3f(m_Vertex[m_Face[i].ii].p.x, m_Vertex[m_Face[i].ii].p.y, m_Vertex[m_Face[i].ii].p.z);
				glEnd();
			}
		}

		if(mode==4)
		{
			glLineWidth(3);
			glColor3f(1,1,1);
			for(i=0;i<m_TetraNum;i++)
			{
				glBegin(GL_LINE_STRIP);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[0]].p.x, m_Vertex[m_Tetra[i].VertexIndex[0]].p.y, m_Vertex[m_Tetra[i].VertexIndex[0]].p.z);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[1]].p.x, m_Vertex[m_Tetra[i].VertexIndex[1]].p.y, m_Vertex[m_Tetra[i].VertexIndex[1]].p.z);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[2]].p.x, m_Vertex[m_Tetra[i].VertexIndex[2]].p.y, m_Vertex[m_Tetra[i].VertexIndex[2]].p.z);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[3]].p.x, m_Vertex[m_Tetra[i].VertexIndex[3]].p.y, m_Vertex[m_Tetra[i].VertexIndex[3]].p.z);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[0]].p.x, m_Vertex[m_Tetra[i].VertexIndex[0]].p.y, m_Vertex[m_Tetra[i].VertexIndex[0]].p.z);
				glEnd();

				glBegin(GL_LINES);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[0]].p.x, m_Vertex[m_Tetra[i].VertexIndex[0]].p.y, m_Vertex[m_Tetra[i].VertexIndex[0]].p.z);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[2]].p.x, m_Vertex[m_Tetra[i].VertexIndex[2]].p.y, m_Vertex[m_Tetra[i].VertexIndex[2]].p.z);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[3]].p.x, m_Vertex[m_Tetra[i].VertexIndex[3]].p.y, m_Vertex[m_Tetra[i].VertexIndex[3]].p.z);
				glVertex3f(m_Vertex[m_Tetra[i].VertexIndex[1]].p.x, m_Vertex[m_Tetra[i].VertexIndex[1]].p.y, m_Vertex[m_Tetra[i].VertexIndex[1]].p.z);
				glEnd();
			}
		}
		glPopAttrib();

		return 1;
	};

	int ReadInverseStiffMatrix(char* filename)
	{
		m_GU.init(m_PointNum*3,1);
		m_GKI.Read(filename);
		return 1;
	};

	int DrawBoundaryPoint(vec3d pointColor)
	{
		GLUquadricObj *qobj = 0;
		qobj = gluNewQuadric();
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glColor3f(pointColor.x,pointColor.y,pointColor.z);
		for(int i=0;i<m_BoundaryPointNum;i++)
		{
			glPushMatrix();
			glTranslatef(m_Vertex[m_BoundaryPoint[i]].p.x,m_Vertex[m_BoundaryPoint[i]].p.y,m_Vertex[m_BoundaryPoint[i]].p.z);
			gluSphere(qobj,1,20,20);
			glPopMatrix();
		}
		return 1;
	};

	int DrawPointIndex(vec3d pointColor)
	{
		GLUquadricObj *qobj = 0;
		qobj = gluNewQuadric();
		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glColor3f(pointColor.x,pointColor.y,pointColor.z);
		for(int i=0;i<m_PointNum;i++)
		{
			glPushMatrix();
			glTranslatef(m_Vertex[i].p.x,m_Vertex[i].p.y,m_Vertex[i].p.z);
			gluSphere(qobj,0.1,20,20);
			glPopMatrix();
			CString pointNum;
			pointNum.Format("%d",i);
			glRasterPos3f(m_Vertex[i].p.x,m_Vertex[i].p.y,m_Vertex[i].p.z);
			glPrint(pointNum);
		}
		glPopAttrib();
		return 1;
	};
	
	void ConstructFemData(double E, double Nu)
	{
		int i,j,k;

		//각 tetrahedron element의 stiffness matrix를 계산
		for(i=0;i<m_TetraNum;i++)
			m_Tetra[i].ConstructStiffMatrix(E,Nu);

		//vertex에 이웃하는 tetrahedron을 찾는다
		for(i=0;i<m_TetraNum;i++)
		{
			for(j=0;j<4;j++)
				m_Vertex[m_Tetra[i].VertexIndex[j]].AddNeiTetra(&m_Tetra[i]);
		}

		//vertex에 이웃하는 point를 찾는다
		for(i=0;i<m_TetraNum;i++)
		{
			for(j=0;j<4;j++)
			{
				for(k=0;k<4;k++)
				{
					if(!(k==j))
						m_Vertex[m_Tetra[i].VertexIndex[j]].AddNeiPoint(m_Tetra[i].VertexIndex[k]);
				}
			}
		}

		//Edge의 개수를 센다
		for(i=0;i<m_PointNum;i++)
		{
			for(j=0;j<m_Vertex[i].neiPointNum;j++)
			{
				if(i<m_Vertex[i].neiPoint[j])
					m_EdgeNum++;
			}
		}

		//Edge에 메모리를 할당한다
		m_Edge=new CEdge[m_EdgeNum];

		//vertex에 연결된 edge를 저장한다
		int edgeNum=0;
		for(i=0;i<m_PointNum;i++)
		{
			for(j=m_Vertex[i].neiEdgeNum;j<m_Vertex[i].neiPointNum;j++)
			{
				int verNum=m_Vertex[i].AddEdge(&m_Edge[edgeNum],-1);
				m_Vertex[verNum].AddEdge(&m_Edge[edgeNum],i);
				m_Edge[edgeNum].pointIndex[0]=i;
				m_Edge[edgeNum].pointIndex[1]=verNum;
				edgeNum++;
			}
		}

		//edge에 연결된 tetrahedron을 찾는다
		for(i=0;i<m_EdgeNum;i++)
		{
			int index[2];
			index[0]=m_Edge[i].pointIndex[0];
			index[1]=m_Edge[i].pointIndex[1];
			for(j=0;j<m_Vertex[index[0]].neiTetraNum;j++)
			{
				CTetra* tetra=m_Vertex[index[0]].neiTetra[j];
				if(tetra->IsVerInTetra(index[1]))
					m_Edge[i].AddTetra(tetra);
			}
		}

		//vertex의 stiffness matrix K를 저장한다
		for(i=0;i<m_PointNum;i++)
		{
			for(j=0;j<m_Vertex[i].neiTetraNum;j++)
			{
				matrix K;
				CTetra* tetra=m_Vertex[i].neiTetra[j];
				K=tetra->ReturnVertexStiffMatrix(i);
				m_Vertex[i].AddStiffMatrix(&K);
			}
			m_Vertex[i].ComputeMass(m_MassCoeff, m_DampCoeff);
		}

		//edge의 stiffness matrix K를 저장한다
		for(i=0;i<m_EdgeNum;i++)
		{
			for(j=0;j<m_Edge[i].neiTetraNum;j++)
			{
				matrix K;
				int index[2];
				index[0]=m_Edge[i].pointIndex[0];
				index[1]=m_Edge[i].pointIndex[1];
				CTetra* tetra=m_Edge[i].neiTetra[j];
				K=tetra->ReturnEdgeStiffMatrix(index[0], index[1]);
				m_Edge[i].AddStiffMatrix(&K);
			}
		}
	};

	void ConstructGlobalStiffMatrix()
	{
		int i,j,k;

		//global stiffness matrix 초기화
		m_GK.init(3*m_PointNum,3*m_PointNum);

		//global load vector 초기화
		m_GF.init(3*m_PointNum,1);

		//global displacement vector 초기화
		m_GU.init(3*m_PointNum,1);

/*		for(i=0;i<m_PointNum;i++)
			m_GK.plusSubMatrix(m_Vertex[i].K,i*3,i*3,3,3);
		for(i=0;i<m_EdgeNum;i++)
		{
			int index1,index2;
			index1=m_Edge[i].pointIndex[0];
			index2=m_Edge[i].pointIndex[1];

			m_GK.plusSubMatrix(m_Edge[i].K,index1*3,index2*3,3,3);
			m_GK.plusSubMatrix(m_Edge[i].K.transpose(),index2*3,index1*3,3,3);
		}*/

		//element stiffness matrix assemble
		for(i=0;i<m_TetraNum;i++)
		{
			int index[4];
			for(j=0;j<4;j++)
				index[j]=m_Tetra[i].VertexIndex[j];
			for(j=0;j<4;j++)
			{
				for(k=0;k<4;k++)
				{
					for(int l=0;l<3;l++)
					{
						for(int m=0;m<3;m++)
							m_GK(index[j]*3+l,index[k]*3+m)+=m_Tetra[i].K(j*3+l,k*3+m);
					}
				}
			}
		}

		//boundary condition을 적용한다
		for(i=0;i<m_BoundaryPointNum;i++)
		{
			for(j=0;j<m_PointNum*3;j++)
			{
				m_GK(j,m_BoundaryPoint[i]*3)=0;
				m_GK(m_BoundaryPoint[i]*3,j)=0;

				m_GK(j,m_BoundaryPoint[i]*3+1)=0;
				m_GK(m_BoundaryPoint[i]*3+1,j)=0;

				m_GK(j,m_BoundaryPoint[i]*3+2)=0;
				m_GK(m_BoundaryPoint[i]*3+2,j)=0;
			}
			m_GK(m_BoundaryPoint[i]*3,m_BoundaryPoint[i]*3)=1;
			m_GK(m_BoundaryPoint[i]*3+1,m_BoundaryPoint[i]*3+1)=1;
			m_GK(m_BoundaryPoint[i]*3+2,m_BoundaryPoint[i]*3+2)=1;
		}
	};

	void UpdatePosition(int* conPointIndex,vec3d* disVector, int conPointNum, double dt)
	{
		int i;

		//constraint된 vertex의 displacement를 업데이트한다
		for(i=0;i<conPointNum;i++)
		{
			m_Vertex[conPointIndex[i]].u=disVector[i];
			m_Vertex[conPointIndex[i]].constraint=true;
		}

		//force를 업데이트한다
		UpdateForce();

		//position을 업데이트한다
		double dtsquare=dt*dt;
		double dt2=dt*2;

		for(i=0;i<m_PointNum;i++)
		{
			if((!m_Vertex[i].boundary) && (!m_Vertex[i].constraint))
			{
				vec3d f=m_Vertex[i].F;
				vec3d preu=m_Vertex[i].preu;
				double m=m_Vertex[i].M;
				double c=m_Vertex[i].C;
				m_Vertex[i].preu=m_Vertex[i].u;
				m_Vertex[i].u=f*-1+m_Vertex[i].u*2*m/dtsquare-preu*(m/dtsquare-c/dt2);
				m_Vertex[i].u=m_Vertex[i].u/(m/dtsquare+c/dt2);
			}
			m_Vertex[i].p=m_Point[i]+m_Vertex[i].u;
		}
	};

	void UpdateForce()
	{
		int i,j;
		for(i=0;i<m_PointNum;i++)
		{
			vec3d f1, f2;
			f1=m_Vertex[i].ComputeVertexForce();
			for(j=0;j<m_Vertex[i].neiEdgeNum;j++)
			{
				vec3d dis=m_Vertex[m_Vertex[i].neiPoint[j]].u;
				CEdge* edge=m_Vertex[i].neiEdge[j];
				f2=f2+edge->ComputeForce(dis);
			}
			m_Vertex[i].F=f1+f2;
		}
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

	void ApplyForce(int vertexIndex, vec3d force)
	{
		m_GF(vertexIndex*3,0)=force.x;
		m_GF(vertexIndex*3+1,0)=force.y;
		m_GF(vertexIndex*3+2,0)=force.z;
	};

	void UpdateVirtexPosition()
	{
		int i;
//		matrix GKI;
//		GKI=m_GK.inverse();
//		m_GU=GKI*m_GF;
//		m_GK.GaussElimination(&m_GF,&m_GU);
		for(i=0;i<m_PointNum;i++)
		{
			m_Vertex[i].p.x=m_Point[i].x+m_GU[i*3];
			m_Vertex[i].p.y=m_Point[i].y+m_GU[i*3+1];
			m_Vertex[i].p.z=m_Point[i].z+m_GU[i*3+2];
		}
	};

	void ComputeForceDisplacement(int* ConPointIndex, vec3d* DisVector, int ConPointNum, vec3d* ConForce)
	{
		int i,j,k,l;

		//displacement vector initialize
		m_GU.init();

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
		for(i=0;i<m_PointNum;i++)
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

		//DisVector에 contact force를 넣는다
		for(i=0;i<ConPointNum;i++)
		{
			ConForce[i].x=Fcontact(i*3,0);
			ConForce[i].y=Fcontact(i*3+1,0);
			ConForce[i].z=Fcontact(i*3+2,0);
		}

		delete [] copyConPoint;
	}

	void PrintGKMatrix(char* filename)
	{
		int i,j;
		FILE* f=fopen(filename,"w");
		for(i=0;i<m_PointNum*3;i++)
		{
			for(j=0;j<m_PointNum*3;j++)
			{
				fprintf(f,"%f\t",m_GK(i,j));
				if(j%3==2)
				{
					fprintf(f,"\t\t");
				}
			}
			fprintf(f,"\n");
			if(i%3==2)
			{
				fprintf(f,"(%d,%d)",i/3,j/3);
				fprintf(f,"\n\n\n\n");
			}
		}
		fclose(f);
	};

	void PrintGFMatrix(char* filename)
	{
		int i;
		FILE* f=fopen(filename,"w");
		for(i=0;i<m_PointNum*3;i++)
		{
			fprintf(f,"%f\n",m_GF(i,0));
			if(i%3==2)
			{
				fprintf(f,"%d\n",i/3);
				fprintf(f,"\n");
			}
		}
	};

	void PrintFemData(char* filename)
	{
		int i,j;
		FILE* f;
		f=fopen(filename,"w");

		fprintf(f,"Neibor TetraNum\n\n");
		for(i=0;i<m_PointNum;i++)
		{
			fprintf(f,"%d: %d개\n",i,m_Vertex[i].neiTetraNum);
		}

		fprintf(f,"\nNeibor Vertex data\n\n");
		for(i=0;i<m_PointNum;i++)
		{
			fprintf(f,"%d: ",i);
			for(j=0;j<m_Vertex[i].neiPointNum;j++)
				fprintf(f,"%d ",m_Vertex[i].neiPoint[j]);
			fprintf(f,"\n");
		}

		fprintf(f,"\nNeivor vertex & edge Num\n\n");
		for(i=0;i<m_PointNum;i++)
		{
			fprintf(f,"pointNum:%d edgeNum:%d\n",m_Vertex[i].neiPointNum,m_Vertex[i].neiEdgeNum);
		}

		fprintf(f,"\nNeibor edge data\n\n");
		for(i=0;i<m_PointNum;i++)
		{
			fprintf(f,"%d: ",i);
			for(j=0;j<m_Vertex[i].neiEdgeNum;j++)
				fprintf(f,"%d--%d ",m_Vertex[i].neiEdge[j]->pointIndex[0],m_Vertex[i].neiEdge[j]->pointIndex[1]);
			fprintf(f,"\n");
		}

		fprintf(f,"\nEdge Neibor tetra num\n\nEdgeNum=%d\n",m_EdgeNum);
		for(i=0;i<m_EdgeNum;i++)
		{
			fprintf(f,"%d: %d---%d: %d\n",i,m_Edge[i].pointIndex[0],m_Edge[i].pointIndex[1],m_Edge[i].neiTetraNum);
		}

		fclose(f);
	};
};

#endif

