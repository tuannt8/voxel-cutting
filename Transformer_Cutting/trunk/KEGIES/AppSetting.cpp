// AppSetting.cpp : implementation file
//
#include "stdafx.h"
#include "AppSetting.h"


static LPCTSTR INVALID_STRING_VALUE = __T("Invalid String Value");

bool AppSetting::showNavigation = true;

bool AppSetting::bShowAxis = false;
bool AppSetting::bShowHelpText = true;
CString AppSetting::objPath = _T("");
CString AppSetting::simulationDataPath = _T("");

AppSetting::AppSetting()
{

}

AppSetting::~AppSetting()
{
}

void AppSetting:: loadApplicationSetting()
{
	CWinApp* pApp = AfxGetApp();

	showNavigation = pApp->GetProfileInt(REGKEY_APP_NAME, REGKEY_SHOW_NAVIGATION, 1) != 0;//0 -> false; 1 -> true

	bShowHelpText = pApp->GetProfileInt(REGKEY_APP_NAME, REGKEY_DISPLAY_TEXT, 1) != 0;
	bShowAxis = pApp->GetProfileInt(REGKEY_APP_NAME, REGKEY_DISPLAY_AXIS, 1) != 0;
	objPath = pApp->GetProfileString(REGKEY_APP_NAME, REGKEY_OBJECT_PATH);
	simulationDataPath = pApp->GetProfileString(REGKEY_APP_NAME, REGKEY_SIM_DATA_PATH);

	CHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	CString documentPath(my_documents);
	documentPath.AppendFormat("\\%s", SETTING_NAME);
	if (GetFileAttributes(documentPath) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectory(documentPath,NULL);
	}
}

void AppSetting::saveSetting()
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(REGKEY_APP_NAME, REGKEY_SHOW_NAVIGATION, showNavigation? 1:0);

	pApp->WriteProfileInt(REGKEY_APP_NAME, REGKEY_DISPLAY_TEXT, bShowHelpText? 1:0);
	pApp->WriteProfileInt(REGKEY_APP_NAME, REGKEY_DISPLAY_AXIS, bShowAxis? 1:0);
	pApp->WriteProfileStringA(REGKEY_APP_NAME,REGKEY_OBJECT_PATH,objPath);
	pApp->WriteProfileStringA(REGKEY_APP_NAME,REGKEY_SIM_DATA_PATH,simulationDataPath);
}

void AppSetting:: saveIntSetting(int setting, CString key)
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(REGKEY_APP_NAME, key.GetBuffer(), setting);
}

int AppSetting::readIntSetting(CString key)
{
	CWinApp* pApp = AfxGetApp();
	return pApp->GetProfileInt(REGKEY_APP_NAME, key.GetBuffer(), 0);
}

bool AppSetting::readBoolSetting(CString key)
{
	CWinApp* pApp = AfxGetApp();
	return pApp->GetProfileInt(REGKEY_APP_NAME, key.GetBuffer(), 0) == 0;
}
void AppSetting::saveBoolSetting(bool setting, CString key)
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(REGKEY_APP_NAME, key.GetBuffer(), setting? 0:1);
}
//////////////////////////////////////////////////////////////////////////
//Get set
void AppSetting::setObjPath(char* newObjPath)
{
	objPath = CString(newObjPath);
}
void AppSetting::setSimDataPath(char* newDataPath)
{
	simulationDataPath = CString(newDataPath);
}

vec3d AppSetting::readVectorSetting( CString key , vec3d defalut)
{
	vec3d v = defalut;
	CString path = relativePath(key);
	FILE* f = fopen(path.GetBuffer(), "r");
	if(f)
	{
		double x,y,z;
		fscanf(f, "%lf %lf %lf", &x, &y, &z);
		v.x = x; v.y = y; v.z = z;
		fclose(f);
	}
	return v;
}

void AppSetting::writeVectorReg( CString key, vec3d v )
{
	CString path = relativePath(key);
	FILE* f = fopen(path.GetBuffer(), "w");
	if(f)
	{
		fprintf(f, "%lf %lf %lf", v.x, v.y, v.z);
		fclose(f);
	}
}

CString AppSetting::relativePath( CString name )
{
	CHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	CString documentPath(my_documents);
	documentPath.AppendFormat("\\%s\\%s.txt", SETTING_NAME, name.GetBuffer());
	return documentPath;
}

CCamera AppSetting::loadcamera()
{
	CString path = relativePath(CAMERA_SETTING);
	FILE *f = fopen(path.GetBuffer(), "r");
	CCamera cam;
	if (f)
	{
		double x,y,z;
		//Position
		fscanf(f," %lf %lf %lf ", &x, &y,&z);
		cam.m_Pos = vec3d(x,y,z);
		//Up
		fscanf(f," %lf %lf %lf ", &x, &y,&z);
		cam.m_Up = vec3d(x,y,z);
		//Center
		fscanf(f," %lf %lf %lf ", &x, &y,&z);
		cam.m_Center = vec3d(x,y,z);
		// direction
		fscanf(f," %lf %lf %lf ", &x, &y,&z);
		cam.m_Direction = vec3d(x,y,z);
		//rot axis
		fscanf(f," %lf %lf %lf ", &x, &y,&z);
		cam.m_RotAxis = vec3d(x,y,z);
		//distance
		fscanf(f," %lf", &x);
		cam.m_Distance = x;
		// rot angle
		fscanf(f," %lf", &x);
		cam.m_RotAngle = x;

		CString rotM = relativePath("cam_rot_mattrix");
		if (GetFileAttributes(rotM) != INVALID_FILE_ATTRIBUTES)
		{
			cam.m_RotMatrix.Read(rotM.GetBuffer());
		}
		

		fclose(f);
	}
	
	return cam;
}

void AppSetting::saveCamera( CCamera cam )
{
	CString path = relativePath(CAMERA_SETTING);
	FILE *f = fopen(path.GetBuffer(), "w");
	if (f)
	{
		vec3d v;
		//Position
		v = cam.m_Pos;
		fprintf(f," %lf %lf %lf ", v.x, v.y,v.z);
		//Up
		v = cam.m_Up;
		fprintf(f," %lf %lf %lf ", v.x, v.y,v.z);
		//Center
		v = cam.m_Center;
		fprintf(f," %lf %lf %lf ", v.x, v.y,v.z);
		// direction
		v = cam.m_Direction;
		fprintf(f," %lf %lf %lf ", v.x, v.y,v.z);
		//rot axis
		v = cam.m_RotAxis;
		fprintf(f," %lf %lf %lf ", v.x, v.y,v.z);
		//distance
		fprintf(f," %lf ", cam.m_Distance);
		// rot angle
		fprintf(f," %lf ", cam.m_RotAngle);
		fclose(f);

		cam.m_RotMatrix.Print(relativePath("cam_rot_mattrix").GetBuffer());
	}
}
