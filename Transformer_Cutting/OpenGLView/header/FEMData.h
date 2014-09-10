#if !defined(_FEMDATA_H_)
#define _FEMDATA_H_

#include "DataStruct.h"

class CTetra  
{
public:
	//tetrahedron의 4개의 vertex 좌표
	vec3d	VertexPos[4];

	//tetrahedron의 4개의 vertex index
	int		VertexIndex[4];

	//element stiffness matrix
	matrix	K;

	//tetrahedron의 볼륨
	double Volume;

public:
	CTetra()
	{

	};

	~CTetra()
	{

	};

	void ConstructStiffMatrix(double E, double Nu)
	{
		int i,j;

		//stiffness matrix 초기화
		K.init(12,12);

		//D matrix 초기화 sigma=[D]epsilon
		matrix D(6,6);
		for(i=0;i<3;i++)
			D(i,i)=1-Nu;
		for(i=3;i<6;i++)
			D(i,i)=(1-2*Nu)/2;
		D(0,1)=D(0,2)=D(1,0)=D(1,2)=D(2,0)=D(2,1)=Nu;

		double coeff=E/(1.0+Nu)/(1.0-2.0*Nu);
		for(i=0;i<6;i++)
		{
			for(j=0;j<6;j++)
				D(i,j)=coeff*D(i,j);
		}

		//shape function
		//u=psi1*u1+psi2*u2+psi3*u3+psi4*u4
		//psi1=1/6V*(a1+b1x+c1y+d1z), psi2=1/6V*(a2+b2x+c2y+d2z), psi3=1/6V*(a3+b3x+c3y+d3z), psi4=1/6V*(a4+b4x+c4y+d4z)
		
		double a[4], b[4], c[4], d[4];
		double volume6;
		vec3d tempPoint[4];

		//tetrahedron의 volume을 계산한다
		volume6=ComputeDeterminant(VertexPos[1],VertexPos[2],VertexPos[3]);
		volume6-=ComputeDeterminant(VertexPos[0],VertexPos[2],VertexPos[3]);
		volume6+=ComputeDeterminant(VertexPos[0],VertexPos[1],VertexPos[3]);
		volume6-=ComputeDeterminant(VertexPos[0],VertexPos[1],VertexPos[2]);
		Volume=volume6/6.0;

		//a[i], b[i], c[i], d[i] 계산
		for(i=0;i<4;i++)
			tempPoint[i]=VertexPos[i];
		a[0]=ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		a[1]=-1*ComputeDeterminant(tempPoint[0],tempPoint[2],tempPoint[3]);
		a[2]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[3]);
		a[3]=-1*ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		for(i=0;i<4;i++)
		{
			tempPoint[i]=VertexPos[i];
			tempPoint[i].x=1;
		}
		b[0]=-1*ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		b[1]=ComputeDeterminant(tempPoint[0],tempPoint[2],tempPoint[3]);
		b[2]=-1*ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[3]);
		b[3]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		for(i=0;i<4;i++)
		{
			tempPoint[i].x=1;
			tempPoint[i].y=VertexPos[i].x;
			tempPoint[i].z=VertexPos[i].z;;
		}
		c[0]=ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		c[1]=-1*ComputeDeterminant(tempPoint[0],tempPoint[2],tempPoint[3]);
		c[2]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[3]);
		c[3]=-1*ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		for(i=0;i<4;i++)
		{
			tempPoint[i].x=1;
			tempPoint[i].y=VertexPos[i].x;
			tempPoint[i].z=VertexPos[i].y;
		}
		d[0]=-1*ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		d[1]=ComputeDeterminant(tempPoint[2],tempPoint[3],tempPoint[0]);
		d[2]=-1*ComputeDeterminant(tempPoint[3],tempPoint[0],tempPoint[1]);
		d[3]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		//shape function의 derivative를 구한다
		double dxdpsi[4];
		double dydpsi[4];
		double dzdpsi[4];

		for(i=0;i<4;i++)
		{
			dxdpsi[i]=b[i]/volume6;
			dydpsi[i]=c[i]/volume6;
			dzdpsi[i]=d[i]/volume6;
		}

		//[B][N] matrix를 구한다
		matrix BN(6,12);
		matrix BNT(12,6);

		for(i=0;i<4;i++)
		{
			BN(0,i*3)=dxdpsi[i];
			BN(1,1+i*3)=dydpsi[i];
			BN(2,2+i*3)=dzdpsi[i];

			BN(3,i*3)=dydpsi[i];
			BN(3,1+i*3)=dxdpsi[i];

			BN(4,1+i*3)=dzdpsi[i];
			BN(4,2+i*3)=dydpsi[i];

			BN(5,i*3)=dzdpsi[i];
			BN(5,2+i*3)=dxdpsi[i];
		}

		//stiffness matrix K를 구한다
		BNT=BN.transpose();
		K=BNT*D*BN*Volume;
	};

	void ConstructStiffMatrix(double E, double Nu, int fileIndex, bool print)
	{
		int i,j;

		//stiffness matrix 초기화
		K.init(12,12);

		//D matrix 초기화 sigma=[D]epsilon
		matrix D(6,6);
		for(i=0;i<3;i++)
			D(i,i)=1-Nu;
		for(i=3;i<6;i++)
			D(i,i)=(1-2*Nu)/2;
		D(0,1)=D(0,2)=D(1,0)=D(1,2)=D(2,0)=D(2,1)=Nu;

		double coeff=E/(1.0+Nu)/(1.0-2.0*Nu);
		for(i=0;i<6;i++)
		{
			for(j=0;j<6;j++)
				D(i,j)=coeff*D(i,j);
		}

		//shape function
		//u=psi1*u1+psi2*u2+psi3*u3+psi4*u4
		//psi1=1/6V*(a1+b1x+c1y+d1z), psi2=1/6V*(a2+b2x+c2y+d2z), psi3=1/6V*(a3+b3x+c3y+d3z), psi4=1/6V*(a4+b4x+c4y+d4z)
		
		double a[4], b[4], c[4], d[4];
		double volume6;
		vec3d tempPoint[4];

		//tetrahedron의 volume을 계산한다
		volume6=ComputeDeterminant(VertexPos[1],VertexPos[2],VertexPos[3]);
		volume6-=ComputeDeterminant(VertexPos[0],VertexPos[2],VertexPos[3]);
		volume6+=ComputeDeterminant(VertexPos[0],VertexPos[1],VertexPos[3]);
		volume6-=ComputeDeterminant(VertexPos[0],VertexPos[1],VertexPos[2]);
		Volume=volume6/6.0;
		
		//a[i], b[i], c[i], d[i] 계산
		for(i=0;i<4;i++)
			tempPoint[i]=VertexPos[i];
		a[0]=ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		a[1]=-1*ComputeDeterminant(tempPoint[0],tempPoint[2],tempPoint[3]);
		a[2]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[3]);
		a[3]=-1*ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		for(i=0;i<4;i++)
		{
			tempPoint[i]=VertexPos[i];
			tempPoint[i].x=1;
		}
		b[0]=-1*ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		b[1]=ComputeDeterminant(tempPoint[0],tempPoint[2],tempPoint[3]);
		b[2]=-1*ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[3]);
		b[3]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		for(i=0;i<4;i++)
		{
			tempPoint[i].x=1;
			tempPoint[i].y=VertexPos[i].x;
			tempPoint[i].z=VertexPos[i].z;;
		}
		c[0]=ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		c[1]=-1*ComputeDeterminant(tempPoint[0],tempPoint[2],tempPoint[3]);
		c[2]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[3]);
		c[3]=-1*ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		for(i=0;i<4;i++)
		{
			tempPoint[i].x=1;
			tempPoint[i].y=VertexPos[i].x;
			tempPoint[i].z=VertexPos[i].y;
		}
		d[0]=-1*ComputeDeterminant(tempPoint[1],tempPoint[2],tempPoint[3]);
		d[1]=ComputeDeterminant(tempPoint[2],tempPoint[3],tempPoint[0]);
		d[2]=-1*ComputeDeterminant(tempPoint[3],tempPoint[0],tempPoint[1]);
		d[3]=ComputeDeterminant(tempPoint[0],tempPoint[1],tempPoint[2]);

		//shape function의 derivative를 구한다
		double dxdpsi[4];
		double dydpsi[4];
		double dzdpsi[4];

		for(i=0;i<4;i++)
		{
			dxdpsi[i]=b[i]/volume6;
			dydpsi[i]=c[i]/volume6;
			dzdpsi[i]=d[i]/volume6;
		}

		//[B][N] matrix를 구한다
		matrix BN(6,12);
		matrix BNT(12,6);

		for(i=0;i<4;i++)
		{
			BN(0,i*3)=dxdpsi[i];
			BN(1,1+i*3)=dydpsi[i];
			BN(2,2+i*3)=dzdpsi[i];

			BN(3,i*3)=dydpsi[i];
			BN(3,1+i*3)=dxdpsi[i];

			BN(4,1+i*3)=dzdpsi[i];
			BN(4,2+i*3)=dydpsi[i];

			BN(5,i*3)=dzdpsi[i];
			BN(5,2+i*3)=dxdpsi[i];
		}

		//stiffness matrix K를 구한다
		BNT=BN.transpose();
		K=BNT*D*BN*Volume;

		//data를 print한다
		if(print)
		{
			CString filename;
			filename.Format("./data/%d.txt",fileIndex);
			FILE* f=fopen(filename,"w");

			fprintf(f,"E=%f, Nu=%f\n\n",E,Nu);
			fprintf(f,"Point 좌표\n");
			for(i=0;i<4;i++)
			{
				fprintf(f,"Point%d: %03.2f %03.2f %03.2f\n",i,VertexPos[i].x,VertexPos[i].y,VertexPos[i].z);
			}

			fprintf(f,"\nVolume=%f\n",Volume);

			fprintf(f,"\na, b, c, d data\n");
			for(i=0;i<4;i++)
			{
				fprintf(f,"a[%d]=%f\n",i,a[i]);
			}
			fprintf(f,"\n");
			for(i=0;i<4;i++)
			{
				fprintf(f,"b[%d]=%f\n",i,b[i]);
			}
			fprintf(f,"\n");
			for(i=0;i<4;i++)
			{
				fprintf(f,"c[%d]=%f\n",i,c[i]);
			}
			fprintf(f,"\n");
			for(i=0;i<4;i++)
			{
				fprintf(f,"d[%d]=%f\n",i,d[i]);
			}
			fprintf(f,"\n");

			fprintf(f,"\ndifferential data\n");
			for(i=0;i<4;i++)
			{
				fprintf(f,"dxdpsi[%d]=%f\n",i,dxdpsi[i]);
				fprintf(f,"dydpsi[%d]=%f\n",i,dydpsi[i]);
				fprintf(f,"dzdpsi[%d]=%f\n",i,dzdpsi[i]);
				fprintf(f,"\n");
			}
		
			fprintf(f,"\n[D] matrix\n");
			for(i=0;i<6;i++)
			{
				for(j=0;j<6;j++)
					fprintf(f,"%f ",D(i,j));
				//	fprintf(f,"%03.2f ",D(i,j));
				fprintf(f,"\n");
			}

			fprintf(f,"\n[BN] matrix\n");
			for(i=0;i<6;i++)
			{
				for(j=0;j<12;j++)
					fprintf(f,"%f ",BN(i,j));
				//	fprintf(f,"%03.2f ",BN(i,j));
				fprintf(f,"\n");
			}

			fprintf(f,"\n[BNT] matrix\n");
			for(i=0;i<12;i++)
			{
				for(j=0;j<6;j++)
					fprintf(f,"%f ",BNT(i,j));
			//		fprintf(f,"%03.2f ",BNT(i,j));
				fprintf(f,"\n");
			}

			fprintf(f,"\n[K] matrix\n");
			for(i=0;i<12;i++)
			{
				for(j=0;j<12;j++)
				{
					fprintf(f,"%f\t",K(i,j));
					if(j%3==2)
						fprintf(f,"\t");
				}
				if(i%3==2)
					fprintf(f,"\n\n\n");
				//	fprintf(f,"%03.2f ",K(i,j));
				fprintf(f,"\n");
			}
		}
	};

	double ComputeDeterminant(vec3d point1, vec3d point2, vec3d point3)
	{
		double val;
		val=point1.x*(point2.y*point3.z-point2.z*point3.y)
			-point2.x*(point1.y*point3.z-point1.z*point3.y)
			+point3.x*(point1.y*point2.z-point1.z*point2.y);

		return val;
	};

	BOOL IsVerInTetra(int verNum)
	{
		int i;
		for(i=0;i<4;i++)
		{
			if(VertexIndex[i]==verNum)
			{
				return true;
			}
		}
		return false;
	};

	matrix ReturnVertexStiffMatrix(int verNum)
	{
		int i;
		int index;
		matrix mat;

		//몇번째 vertex인지 찾는다
		for(i=0;i<4;i++)
		{
			if(VertexIndex[i]==verNum)
				index=i;
		}

		mat=K.returnSubMatrix(index*3,index*3,3,3);
		return mat;
	};

	matrix ReturnEdgeStiffMatrix(int PointIndex1, int PointIndex2)
	{
		int i;
		int index1,index2;
		matrix mat;

		//몇번째 vertex인지 찾는다
		for(i=0;i<4;i++)
		{
			if(VertexIndex[i]==PointIndex1)
				index1=i;
			if(VertexIndex[i]==PointIndex2)
				index2=i;
		}

		mat=K.returnSubMatrix(index1*3,index2*3,3,3);
		return mat;
	};
};

class CEdge  
{

public:
	int pointIndex[2];
	matrix K;
	CTetra* neiTetra[100];
	int neiTetraNum;

public:
	CEdge()
	{
		neiTetraNum=0;
		K.init(3,3);
	};

	~CEdge()
	{

	};
	void AddTetra(CTetra* tetra)
	{
		neiTetra[neiTetraNum]=tetra;
		neiTetraNum++;
	};

	void AddStiffMatrix(matrix* local)
	{
		K+=(*local);
	};

	vec3d ComputeForce(vec3d dis)
	{
		vec3d force;
		force=K.mulVector(dis);
		return force;
	};
};

class CVertex  
{

public:
	//point 좌표
	vec3d p;

	//변위 벡터
	vec3d u;

	//이전 step의 변위 벡터
	vec3d preu;

	//연결된 tetrahedron
	CTetra* neiTetra[100];

	//연결된 tetrahedron 개수
	int neiTetraNum;

	//연결된 edge
	CEdge* neiEdge[100];

	//연결된 edge의 개수
	int neiEdgeNum;

	//연결된 point index
	int neiPoint[100];

	//연결된 point의 개수
	int neiPointNum;

	//vertex의 stiffness matrix
	matrix K;

	//point에 작용하는 force
	vec3d F;

	//vertex가 contraint되었는지 표시한다
	BOOL constraint;

	//vectex가 boundary condition에 의해 고정되었는지 표시한다
	BOOL boundary;

	//mass
	double M;

	//damping coeff
	double C;

public:
	CVertex()
	{
		neiTetraNum=0;
		neiEdgeNum=0;
		neiPointNum=0;
		constraint=false;
		boundary=false;
		M=0;
		C=0;
		K.init(3,3);
	};

	~CVertex()
	{

	};

	void AddNeiTetra(CTetra* tetra)
	{
		neiTetra[neiTetraNum]=tetra;
		neiTetraNum++;
	};

	void AddNeiPoint(int index)
	{
		int i;

		//이미 저장된 point가 아닌지 검사한다
		for(i=0;i<neiPointNum;i++)
		{
			if(index==neiPoint[i])
				return;
			
		}
		neiPoint[neiPointNum]=index;
		neiPointNum++;
	};

	int AddEdge(CEdge* edge, int verNum)
	{
		int i;
		neiEdge[neiEdgeNum]=edge;
		if(!(verNum==-1))
		{
			int index1,index2;
			index1=neiEdgeNum;
			for(i=0;i<neiPointNum;i++)
			{
				if(neiPoint[i]==verNum)
					index2=i;
			}
			SwapPoint(index1, index2);
		}
		neiEdgeNum++;
		return neiPoint[neiEdgeNum-1];
	};

	void SwapPoint(int index1, int index2)
	{
		if(index1==index2)
			return;
		int temp=neiPoint[index1];
		neiPoint[index1]=neiPoint[index2];
		neiPoint[index2]=temp;
	};

	void AddStiffMatrix(matrix* local)
	{
		K+=(*local);
	};

	vec3d ComputeVertexForce()
	{
		//point의 displacement에 의한 force
		vec3d f;
		f=K.mulVector(f);
		return f;
	};

	void ComputeMass(double massCoeff, double dampCoeff)
	{
		int i;
		double vertexVolume=0;
		for(i=0;i<neiTetraNum;i++)
			vertexVolume+=neiTetra[i]->Volume/4.0;
		M=vertexVolume*massCoeff;
		C=vertexVolume*dampCoeff;
//		C=M*dampCoeff;
	};
};

#endif