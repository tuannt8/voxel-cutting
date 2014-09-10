// Camera.h: interface for the CObj class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(_CAMERA_H_)
#define _CAMERA_H_

#include "DataStruct.h"

class CCamera  
{
public:
	vec3d m_Pos;
	vec3d m_Up;
	vec3d m_Center;
	vec3d m_Direction;

	vec3d m_RotAxis;
	float m_Distance;
	float m_RotAngle;
	matrix m_RotMatrix;

	CCamera()
	{
		m_Pos.z=100;
		m_Distance=100;
		m_Up.y=1;
		m_RotAngle=0;
		m_RotMatrix.init(3,3);
		m_RotMatrix.identity();
	};

	CCamera(double dis)
	{
		m_Pos.z=dis;
		m_Distance=dis;
		m_Up.y=1;
		m_RotAngle=0;
		m_RotMatrix.init(3,3);
		m_RotMatrix.identity();
	};
	
	~CCamera()
	{

	};

	void Init(double dis)
	{
		m_Pos.z=dis;
		m_Distance=dis;
	};

	void Init(vec3d camPos, vec3d centerPos)
	{
		m_Pos=camPos;
		m_Center=centerPos;
		m_Distance=(m_Center-m_Pos).mag();

		vec3d initialPos;
		initialPos.z=m_Distance;
		ComputeAxisAngle(initialPos,m_Pos-m_Center);
		UpdateCamPos();
	};

	void MoveCamPos(vec3d trans)
	{
		trans=m_RotMatrix.mulVector(trans);
		m_Center=m_Center-trans*0.01*m_Distance;
		m_Pos=m_Pos-trans*0.01*m_Distance;
	};

	//center point를 중심으로 camera의 위치를 회전시킨다
	void RotCamPos(vec3d DMousePos)
	{
		vec3d camPos;
		vec3d camUp;
		camPos.z=m_Distance;
		camUp.y=1;

		ComputeAxisAngle(DMousePos);
		
		//회전된 각도에 따라서 rotation matrix를 정의한다.
		matrix rot(3,3);

		//rotation matrix를 계산한다.
		rot=ComputeRotationMatrix(m_RotAxis,m_RotAngle);
		if(m_RotAngle==0)
			rot.identity();

		m_RotMatrix=m_RotMatrix*rot;

		//camera의 위치를 update한다.
		m_Pos=m_RotMatrix.mulVector(camPos)+m_Center;

		//camera의 방향을 update한다.
		m_Up=m_RotMatrix.mulVector(camUp);
		m_RotMatrix.normalizeColVector();
	};

	void UpdateCamPos()
	{
		vec3d camPos;
		vec3d camUp;
		camPos.z=m_Distance;
		camUp.y=1;
		
		//회전된 각도에 따라서 rotation matrix를 정의한다.
		matrix rot(3,3);

		//rotation matrix를 계산한다.
		rot=ComputeRotationMatrix(m_RotAxis,m_RotAngle);
		if(m_RotAngle==0)
			rot.identity();

		m_RotMatrix=m_RotMatrix*rot;

		//camera의 위치를 update한다.
		m_Pos=m_RotMatrix.mulVector(camPos)+m_Center;

		//camera의 방향을 update한다.
		m_Up=m_RotMatrix.mulVector(camUp);
		m_RotMatrix.normalizeColVector();
	};

/*	void UpdateCamPos(rotAxis, rotAngle)
	{
		vec3d camPos;
		vec3d camUp;
		camPos.z=m_Distance;
		camUp.y=1;

		m_RotAxis=rotAxis;
		m_RotAngle=rotAngle;
		
		//회전된 각도에 따라서 rotation matrix를 정의한다.
		matrix rot(3,3);

		//rotation matrix를 계산한다.
		rot=ComputeRotationMatrix(m_RotAxis,m_RotAngle);
		if(m_RotAngle==0)
			rot.identity();

		m_RotMatrix=m_RotMatrix*rot;

		//camera의 위치를 update한다.
		m_Pos=m_RotMatrix.mulVector(camPos);

		//camera의 방향을 update한다.
		m_Up=m_RotMatrix.mulVector(camUp);
		m_RotMatrix.normalizeColVector();
	};*/

	

	void ComputeAxisAngle(vec3d DMousePos)
	{
		//회전축을 정의한다.
		m_RotAxis.x=-DMousePos.y;
		m_RotAxis.y=DMousePos.x;
		m_RotAxis.normalize();

		//회전각도를 정의 한다.
		m_RotAngle=DMousePos.mag()/80.0;
	};

	void ComputeAxisAngle(vec3d initialPos, vec3d finalPos)
	{
		initialPos.normalize(); finalPos.normalize();
		vec3d axis=initialPos.cross(finalPos);
		axis.normalize();
		double angle=acos(initialPos.dot(finalPos));

		m_RotAxis=axis; m_RotAngle=angle;
	}

	matrix ComputeRotationMatrix(vec3d axis,double angle)
	{
		matrix ref(3,3);
		double x,y,z;
		x=axis.x;
		y=axis.y;
		z=axis.z;

		ref(0,0)=x*x+(y*y+z*z)*cos(angle);
		ref(1,1)=y*y+(x*x+z*z)*cos(angle);
		ref(2,2)=z*z+(x*x+y*y)*cos(angle);
		ref(0,1)=(1-cos(angle))*x*y+z*sin(angle);
		ref(1,0)=(1-cos(angle))*x*y-z*sin(angle);
		ref(0,2)=(1-cos(angle))*x*z-y*sin(angle);
		ref(2,0)=(1-cos(angle))*z*x+y*sin(angle);
		ref(1,2)=(1-cos(angle))*y*z+x*sin(angle);
		ref(2,1)=(1-cos(angle))*z*y-x*sin(angle);

		return ref;
	};

	void PrintCamParameter(char* filename)
	{
		FILE* f;
		f=fopen(filename,"w");
		fprintf(f,"pos.x=%f pos.y=%f pos.z=%f\n center.x=%f center.y=%f center.z=%f\n up.x=%f up.y=%f up.z=%f\n",m_Pos.x,m_Pos.y,m_Pos.z,m_Center.x,m_Center.y,m_Center.z,m_Up.x,m_Up.y,m_Up.z);
		fclose(f);
	};
};

#endif