#pragma once
// AppSetting


#define REGKEY_APP_NAME			_T("OpenGL Display")

#define REGKEY_SHOW_NAVIGATION				_T("Show Navigation")
#define REGKEY_MAIN_WINDOW_REPLACEMENT		_T("Main window replacement")

#define  REGKEY_OBJECT_PATH					_T("Display object path")
#define  REGKEY_SIM_DATA_PATH				_T("Simulation data path")
#define	REGKEY_DISPLAY_AXIS					_T("Display Axis")
#define  REGKEY_DISPLAY_TEXT				_T("Display text")

#define  CAMERA_SETTING						_T("Camera_Setting")

#define  SETTING_NAME				_T("VS_App")

class AppSetting
{
public:
	AppSetting();
	virtual ~AppSetting();

public:
	static void loadApplicationSetting();
	static void saveSetting();

	static bool showNavigation;

	static bool bShowAxis;
	static bool bShowHelpText;

	static CStringA objPath;
	static CStringA simulationDataPath;

	static CStringA relativePath(CStringA name);

	//Get set
	static CStringA getObjectPath(){return objPath;};
	static CStringA getSimDataPath(){return simulationDataPath;};
	
	static void setObjPath(char* newObjPath);
	static void setSimDataPath(char* newDataPath);

	static void saveIntSetting(int setting, CStringA key);
	static int readIntSetting(CStringA key);

	static bool readBoolSetting(CStringA key);
	static void saveBoolSetting(bool setting, CStringA key);

	static vec3d readVectorSetting(CStringA key, vec3d defalut = vec3d());
	static void writeVectorReg(CStringA key, vec3d v);

	static CCamera loadcamera();
	static void saveCamera(CCamera cam);
};


