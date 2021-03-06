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

	static CString objPath;
	static CString simulationDataPath;

	static CString relativePath(CString name);

	//Get set
	static CString getObjectPath(){return objPath;};
	static CString getSimDataPath(){return simulationDataPath;};
	
	static void setObjPath(char* newObjPath);
	static void setSimDataPath(char* newDataPath);

	static void saveIntSetting(int setting, CString key);
	static int readIntSetting(CString key);

	static bool readBoolSetting(CString key);
	static void saveBoolSetting(bool setting, CString key);

	static vec3d readVectorSetting(CString key, vec3d defalut = vec3d());
	static void writeVectorReg(CString key, vec3d v);

	static CCamera loadcamera();
	static void saveCamera(CCamera cam);
};


