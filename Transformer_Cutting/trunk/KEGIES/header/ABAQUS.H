#if !defined(_ABAQUS_H_)
#define _ABAQUS_H_

#include "DataStruct.h"
#include <afx.h>

class CABAQUS  
{

private:
	int m_PointNum;
	int m_TetraNum;
	int m_FaceNum;

	vec3d* m_Point;
	vec3i* m_Face;
	vec4i* m_Tetra;

public:
	CABAQUS()
	{
		m_Point=NULL;
		m_Tetra=NULL;
		m_Face=NULL;
	};

	~CABAQUS()
	{
		if(m_Point)
		{
			delete [] m_Point;
			m_Point=NULL;
		}
		if(m_Tetra)
		{
			delete [] m_Tetra;
			m_Tetra=NULL;
		}
		if(m_Face)
		{
			delete [] m_Face;
			m_Face=NULL;
		}
	};

	int CABAQUS::ReadData(char* filename)
	{
		FILE *fp;
		fp = fopen(filename,"r");

		//file의 문자들을 읽어오는 변수
		char str[100];
		CString temp;
		CString Koma=",";
		CString NODE="*NODE";
		CString ELEMENT="*ELEMENT,TYPE=C3D4,ELSET=auto2";

		int pointCount=0;
		int tetraCount=0;
		int faceCount=0;

		//point의 data가 나올때 까지 skip
		while(1)
		{
			fscanf(fp, "%s\n", str);
			temp=str;
			if(temp==NODE)
				break;
		}

		//point의 개수를 센다
		while(1)
		{
			int index;
			vec3d point;
			fscanf(fp, "%s", str); temp=str;
			index=atoi(temp);
			if(index==0)
				break;
			fscanf(fp, "%s", str); temp=str;
			point.x=atof(temp);
			fscanf(fp, "%s", str); temp=str;
			if(temp==Koma)
				fscanf(fp, "%s", str); temp=str;
			point.y=atof(temp);
			fscanf(fp, "%s\n", str); temp=str;
			if(temp==Koma)
				fscanf(fp, "%s", str); temp=str;
			point.z=atof(temp);
			pointCount++;
		}

		//face의 개수를 센다
		while(1)
		{
			int index;
			int point1, point2, point3;
			fscanf(fp, "%s", str); temp=str;
			if(temp==ELEMENT)
				break;
			index=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point1=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point2=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point3=atoi(temp);
			faceCount++;
		}

		//testra 개수를 센다
		while(1)
		{
			int index;
			int point1, point2, point3, point4;
			fscanf(fp, "%s", str); temp=str;
			index=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point1=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point2=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point3=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point4=atoi(temp);
			if(point1==point2 && point2==point3 && point3==point4)
				break;
			tetraCount++;
		}
		fclose(fp);

		//메모리 할당
		m_PointNum=pointCount;
		m_FaceNum=faceCount;
		m_TetraNum=tetraCount;

		m_Point=new vec3d[m_PointNum];
		m_Face=new vec3i[m_FaceNum];
		m_Tetra=new vec4i[m_TetraNum];

		fp = fopen(filename,"r");

		//point의 data가 나올때 까지 skip
		while(1)
		{
			fscanf(fp, "%s\n", str);
			temp=str;
			if(temp==NODE)
				break;
		}

		//point의 data를 저장
		pointCount=0;
		while(1)
		{
			int index;
			vec3d point;
			fscanf(fp, "%s", str); temp=str;
			index=atoi(temp);
			if(index==0)
				break;
			fscanf(fp, "%s", str); temp=str;
			point.x=atof(temp);
			fscanf(fp, "%s", str); temp=str;
			if(temp==Koma)
				fscanf(fp, "%s", str); temp=str;
			point.y=atof(temp);
			fscanf(fp, "%s\n", str); temp=str;
			if(temp==Koma)
				fscanf(fp, "%s", str); temp=str;
			point.z=atof(temp);
			m_Point[pointCount]=point;
			pointCount++;
		}

		//face의 data를 저장
		faceCount=0;
		while(1)
		{
			int index;
			int point1, point2, point3;
			fscanf(fp, "%s", str); temp=str;
			if(temp==ELEMENT)
				break;
			index=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point1=atoi(temp)-1;
			fscanf(fp, "%s", str); temp=str;
			point2=atoi(temp)-1;
			fscanf(fp, "%s", str); temp=str;
			point3=atoi(temp)-1;
			m_Face[faceCount].ii=point1; m_Face[faceCount].jj=point2; m_Face[faceCount].kk=point3; 
			faceCount++;
		}

		//tetra data를 저장
		tetraCount=0;
		while(1)
		{
			int index;
			int point1, point2, point3, point4;
			fscanf(fp, "%s", str); temp=str;
			index=atoi(temp);
			fscanf(fp, "%s", str); temp=str;
			point1=atoi(temp)-1;
			fscanf(fp, "%s", str); temp=str;
			point2=atoi(temp)-1;
			fscanf(fp, "%s", str); temp=str;
			point3=atoi(temp)-1;
			fscanf(fp, "%s", str); temp=str;
			point4=atoi(temp)-1;
			if(point1==point2 && point2==point3 && point3==point4)
				break;
			m_Tetra[tetraCount].ii=point1; m_Tetra[tetraCount].jj=point2; m_Tetra[tetraCount].kk=point3; m_Tetra[tetraCount].ww=point4;
			tetraCount++;
		}
		fclose(fp);

		//겹치는 포인트를 찾아서 없애준다
		FindDuplicatePoint();
		return 1;
	};

	int WriteToObj(char *filename)
	{
		int i;
		FILE* fp;
		fp=fopen(filename,"w");
		
		if(m_Point && m_Tetra)
		{
			fprintf(fp,"%d\n",m_PointNum);
			for(i=0;i<m_PointNum;i++)
				fprintf(fp,"%f %f %f\n",m_Point[i].x,m_Point[i].y,m_Point[i].z);
			fprintf(fp,"%d\n",m_FaceNum);
			for(i=0;i<m_FaceNum;i++)
				fprintf(fp,"%d %d %d\n",m_Face[i].ii,m_Face[i].jj,m_Face[i].kk);
			fprintf(fp,"%d\n",m_TetraNum);
			for(i=0;i<m_TetraNum;i++)
				fprintf(fp,"%d %d %d %d\n",m_Tetra[i].ii,m_Tetra[i].jj,m_Tetra[i].kk,m_Tetra[i].ww);

			fclose(fp);
			return 1;
		}
		else
		{
			fclose(fp);
			return 0;
		}
	};

private:
	void FindDuplicatePoint()
	{
		int i,j;
		for(i=0;i<m_PointNum;i++)
		{
			vec3d refPoint=m_Point[i];
			for(j=i+1;j<m_PointNum;j++)
			{
				if(IsPointSame(refPoint, m_Point[j]))
				{
					DeletePoint(j);
					AdjustFaceNum(i,j);
				}
			}
		}
	};

	BOOL IsPointSame(vec3d point1, vec3d point2)
	{
		double dx=point1.x-point2.x;
		double dy=point1.y-point2.y;
		double dz=point1.z-point2.z;
		if(dx<0.001 && dx>-0.001)
		{
			if(dy<0.001 && dy>-0.001)
			{
				if(dz<0.001 && dz>-0.001)
					return true;
			}
		}

		return false;
	};

	void AdjustFaceNum(int refPoint, int wrongPoint)
	{
		int i;
		for(i=0;i<m_FaceNum;i++)
		{
			if(wrongPoint==m_Face[i].ii)
				m_Face[i].ii=refPoint;
			if(wrongPoint==m_Face[i].jj)
				m_Face[i].jj=refPoint;
			if(wrongPoint==m_Face[i].kk)
				m_Face[i].kk=refPoint;
		}

		for(i=0;i<m_FaceNum;i++)
		{
			if(wrongPoint<m_Face[i].ii)
				m_Face[i].ii--;
			if(wrongPoint<m_Face[i].jj)
				m_Face[i].jj--;
			if(wrongPoint<m_Face[i].kk)
				m_Face[i].kk--;
		}

		for(i=0;i<m_TetraNum;i++)
		{
			if(wrongPoint==m_Tetra[i].ii)
				m_Tetra[i].ii=refPoint;
			if(wrongPoint==m_Tetra[i].jj)
				m_Tetra[i].jj=refPoint;
			if(wrongPoint==m_Tetra[i].kk)
				m_Tetra[i].kk=refPoint;
			if(wrongPoint==m_Tetra[i].ww)
				m_Tetra[i].ww=refPoint;
		}

		for(i=0;i<m_TetraNum;i++)
		{
			if(wrongPoint<m_Tetra[i].ii)
				m_Tetra[i].ii--;
			if(wrongPoint<m_Tetra[i].jj)
				m_Tetra[i].jj--;
			if(wrongPoint<m_Tetra[i].kk)
				m_Tetra[i].kk--;
			if(wrongPoint<m_Tetra[i].ww)
				m_Tetra[i].ww--;
		}
	};

	void DeletePoint(int point)
	{
		int i;
		for(i=point+1;i<m_PointNum;i++)
		{
			m_Point[i-1]=m_Point[i];
		}
		m_PointNum--;
	};
};

#endif