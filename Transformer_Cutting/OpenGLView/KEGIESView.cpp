// KEGIESView.cpp : CKEGIESView 
//

#include "stdafx.h"
#include "KEGIES.h"

#include "KEGIESDoc.h"
#include "KEGIESView.h"
#include "MainFrm.h"

#include "GL\glut.h"
#include "Dialog.h"
#include "..\NodeGenerate\DataTypes\vec.h"
#include "..\NodeGenerate\Cube.h"
#include "controlDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TEST_MESSAGE WM_USER+1000

// CKEGIESView

IMPLEMENT_DYNCREATE(CKEGIESView, CView)

BEGIN_MESSAGE_MAP(CKEGIESView, CView)
	// 
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_RENDERMODE_LINE, &CKEGIESView::OnRendermodeLine)
	ON_COMMAND(ID_RENDERMODE_LINEANDSURFACE, &CKEGIESView::OnRendermodeLineandsurface)
	ON_COMMAND(ID_RENDERMODE_SURFACE, &CKEGIESView::OnRendermodeSurface)
	ON_COMMAND(ID_VIEW_DISPLAYAXIS, &CKEGIESView::OnViewDisplayaxis)
	ON_COMMAND(ID_VIEW_DISPLAYTEXT, &CKEGIESView::OnViewDisplaytext)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_COLOR_BACKGROUND, &CKEGIESView::OnColorBackground)
	ON_COMMAND(ID_SIMULATION_START, &CKEGIESView::OnSimulationStart)
	ON_COMMAND(ID_VIEW_RESETCAMERAVIEW, &CKEGIESView::OnViewResetcameraview)
	ON_COMMAND(ID_TOOL_TEST, &CKEGIESView::test)
	ON_COMMAND(ID_UP_DATE, &CKEGIESView::update)
	ON_MESSAGE(TEST_MESSAGE, testTimer)
	ON_BN_CLICKED(IDOK, &CKEGIESView::OnBnVoxelOk)
	ON_MESSAGE(WM_TEST, &CKEGIESView::OnData)
	ON_BN_CLICKED(IDC_BUTTON1, &CKEGIESView::OnBnClickedButtontest)
END_MESSAGE_MAP()

// CKEGIESView 

CKEGIESView::CKEGIESView():
renderMode(1)
,m_bRunningSimulation(FALSE)
,beraseBackGrnd(FALSE)
{
	// TODO:
	dtime = 10;
	curTime = 0;
	backGroundColor = 2;
	yIdx = -1; zIdx = 0;

	mode = 1;
	curView = 1;

	if (mode == 1) // Define cut box
	{
		// integer
	// 	testCut.loadVoxelAndSkeleton();
	// 	testCut.ParseSkeletonOrderSym();
	//  	testCut.decideDistanceStepByRatioSym();
	//  	testCut.estimateBonePos();
	//	testCut.cutAndMapMeshToBone();

		// float
// 		testCut.loadVoxelAndSkeleton();
// 		testCut.ParseSkeletonOrderSymf();
//   		testCut.decideOcTreeSize();
//  		testCut.estimateBonePosUsingVoxel();
	}
	if (mode == 2)
	{
		detailSwap.initTest();
	}


	for (int i = 0; i < 10; i++)
	{
		m_displayMode[i] = TRUE;
	}

//	octreet.initTest();
//	meshCut.init();

}
void CKEGIESView::setTextDisplay(CString text)
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	timeText.Format(_T("Date: %d/%d time: %d:%d"),st.wDay, st.wMonth, st.wHour, st.wMinute);

	helpText = text;
}


CKEGIESView::~CKEGIESView()
{
	AppSetting::saveCamera(m_Cam1);
}

#pragma OPEN_GL

void CKEGIESView::InitGL()
{
	COpenGL Initgl;
	
	//Init
	Initgl.SetHWND(m_hWnd);
	Initgl.SetupPixelFormat();
	//Initgl.SetupPixelFormatAA(1);
	base=Initgl.base;
	
	m_hDC=Initgl.m_hDC;
	m_hRC=Initgl.m_hRC;

	m_Cam1 = AppSetting::loadcamera();
}

void CKEGIESView::OnDraw(CDC* /*pDC*/)
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 
	wglMakeCurrent(m_hDC,m_hRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(backGroundColor/2.0, backGroundColor/2.0, backGroundColor/2.0, 1.0);
	DrawView();

	SwapBuffers(m_hDC);
	beraseBackGrnd = FALSE;
}

void CKEGIESView::DrawView()
{
	glPushMatrix();
	UpdateView();
	SetupView();

	if(m_bDisplayAxis)
		drawAxis(true, &m_Cam1);

	if (m_bDisplayText)
		DrawText();

// 	if (mode == 1)
// 	{
// 		testCut.draw(m_displayMode);
// 		testCut.drawLeaf(yIdx);
// 	}
// 	if (mode == 2)
// 	{
// 		detailSwap.draw(m_displayMode);
// 	}


	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (pDoc->m_mode == MODE_FINDING_CUT_SURFACE)
	{
		pDoc->testCut.draw(m_displayMode);
		pDoc->testCut.drawLeaf(yIdx);
	}
	else if (pDoc->m_mode == MODE_ASSIGN_COORDINATE)
	{
		pDoc->cordAssign.draw(m_displayMode);
	}
	else if (pDoc->m_mode == MODE_SWAP_VOXEL)
	{
		pDoc->detailSwap.draw(m_displayMode);
	}
	else if (pDoc->m_mode == MODE_SPLIT_BONE_GROUP)
	{
		pDoc->groupCutMngr.draw(m_displayMode);
	}

	pDoc->document.draw(m_displayMode);

//	octreet.draw(m_displayMode);

//	meshCut.draw(m_displayMode);

	glPopMatrix();
	glPopAttrib();
}

void CKEGIESView::drawAxis(bool atOrigin, CCamera* cam)
{
	glPushMatrix();

	float lenght = 0.5*cam->m_Distance;
	if(!atOrigin)
	{
		float textPosX = -0.5*(m_WindowWidth/m_WindowHeight)*cam->m_Distance/1.4;
		float textPosY = -0.5*cam->m_Distance/1.4;
		float textPosZ = 0.0*cam->m_Distance;
		vec3d textPos = vec3d(textPosX,textPosY,textPosZ);

		matrix rotateM = cam->m_RotMatrix;
		textPos = rotateM.mulVector(textPos);
		glTranslatef(cam->m_Center.x, cam->m_Center.y,cam->m_Center.z);
		glTranslatef(textPos.x,textPos.y,textPos.z);

		lenght = 0.05*cam->m_Distance;
	}
	
	//Axis
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(lenght, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, lenght, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, lenght);
	glEnd();

	glPopMatrix();
}

void CKEGIESView::DrawText()
{
	glPushMatrix();

	glPopMatrix();
}

void CKEGIESView::UpdateView()
{
	int _w = m_WindowWidth;
	int _h = m_WindowHeight;

	glViewport(0,0,_w,_h);
	float fovy=45;
	float aspect=float(_w)/float(_h);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	gluPerspective(fovy, aspect, 1, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_Cam1.m_Pos.x,m_Cam1.m_Pos.y,m_Cam1.m_Pos.z,
		m_Cam1.m_Center.x,m_Cam1.m_Center.y,m_Cam1.m_Center.z,
		m_Cam1.m_Up.x,m_Cam1.m_Up.y,m_Cam1.m_Up.z);
}

void CKEGIESView::SetupView()
{
	GLfloat diffuseLight[] = {0.2f,0.2f,0.2f,1.0f};
	GLfloat ambientLight[] = {0.4f,0.4f,0.4f,1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	GLfloat position[] = { m_Cam1.m_Pos.x, m_Cam1.m_Pos.y, m_Cam1.m_Pos.z, 0.0 };

	static float i = 0;
//	GLfloat position[] = { 20.0*sin(i), 20.0*cos(i), 0 };
	i = i + 1;


	glEnable(GL_MULTISAMPLE); 
  	glEnable(GL_DEPTH_TEST);                                        
//  	glEnable(GL_CULL_FACE);  
// 
// 		glDisable(GL_DEPTH_TEST); 
// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
// 		glEnable(GL_BLEND);
	
 	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
 	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	glEnable(GL_LIGHTING);   
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
	
	glFrontFace(GL_CW);

	//glShadeModel(GL_SMOOTH); 
	glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// CKEGIESView

#ifdef _DEBUG
void CKEGIESView::AssertValid() const
{
	CView::AssertValid();
}

void CKEGIESView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKEGIESDoc* CKEGIESView::GetDocument() const // 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKEGIESDoc)));
	return (CKEGIESDoc*)m_pDocument;
}
#endif //_DEBUG

#pragma INITIALIZE
// CKEGIESView
int CKEGIESView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	SetTimer(TIMER_UPDATE_VIEW,10,NULL);
	m_bDisplayAxis = AppSetting::bShowAxis;
	m_bDisplayText = AppSetting::bShowHelpText;
	textMode = AppSetting::readIntSetting("textMode");
	return 0;
}

void CKEGIESView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	LEFT_DOWN=false;
	RIGHT_DOWN=false;

	InitGL();

// 	dlg = new controlDialog;
// 	dlg->Create(IDD_CONTROL_DIALOG, this);
// 	//dlg->ShowWindow(SW_SHOW);
// 	dlg->changeToState(STATE_1_LOAD_FILE);
}

BOOL CKEGIESView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT 

	return CView::PreCreateWindow(cs);
}

void CKEGIESView::InitAfterShowWindow()
{

}

#pragma  WINDOW_EVENT

void CKEGIESView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	char lsChar;
	lsChar = char(nChar);
	keyCode key = KEY_NONE;

	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->document.receiveKey(lsChar);



	if (nChar >= 48 && nChar <= 57)
	{
		m_displayMode[nChar - 48] = ! m_displayMode[nChar - 48];
	}

	switch (nChar)
	{
	case VK_UP:
		key = KEY_UP_ARROW;	break;
	case VK_DOWN:
		key = KEY_DOWN_ARROW; break;
	case VK_LEFT:
		key = KEY_LEFT_ARROW; break;
	case VK_RIGHT:
		key = KEY_RIGHT_ARROW; break;
	case 88:
		key = KEY_X; break;
	case 90:
		key = KEY_Z; break;
	}

	if(key != KEY_NONE)
	{

	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CKEGIESView::OnSize(UINT nType, int cx, int cy)
{
	beraseBackGrnd = TRUE;
	CView::OnSize(nType, cx, cy);

	CSize size(cx,cy);
	m_WindowHeight=size.cy;
	m_WindowWidth=size.cx;
}
void CKEGIESView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_UPDATE_VIEW)
	{
		CKEGIESDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->document.updateRealtime();
		InvalidateRect(NULL, FALSE);
	}
	CView::OnTimer(nIDEvent);
}

void CKEGIESView::OnLButtonDown(UINT nFlags, CPoint point)
{
	LEFT_DOWN=true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnLButtonDown(nFlags, point);
}

void CKEGIESView::OnLButtonUp(UINT nFlags, CPoint point)
{
	LEFT_DOWN=false;
	CView::OnLButtonUp(nFlags, point);
}

void CKEGIESView::OnRButtonDown(UINT nFlags, CPoint point)
{
	RIGHT_DOWN=true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnRButtonDown(nFlags, point);
}

void CKEGIESView::OnRButtonUp(UINT nFlags, CPoint point)
{
	RIGHT_DOWN=false;
	CView::OnRButtonUp(nFlags, point);
}

void CKEGIESView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_MousePos.x=point.x;
	m_MousePos.y=-point.y;
	m_DMousePos=m_MousePos-m_PreMousePos;
	

	if(LEFT_DOWN)
	{
		if (curView == 1)
			m_Cam1.RotCamPos(m_DMousePos);
		else
			m_Cam2.RotCamPos(m_DMousePos);
	}
	else if(RIGHT_DOWN)
	{
		if (curView == 1)
			m_Cam1.MoveCamPos(m_DMousePos);
		else
			m_Cam2.MoveCamPos(m_DMousePos);
	}
	else
	{
		if (point.x < m_WindowWidth/2)
		{
			curView = 1;
		}
		else
			curView = 2;

		curView = 1;
	}
	
	m_PreMousePos=m_MousePos;
	CView::OnMouseMove(nFlags, point);
}

BOOL CKEGIESView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	vec3d temp;
	if (curView == 1)
	{
		m_Cam1.m_Distance-=zDelta*m_Cam1.m_Distance*0.001;
		m_Cam1.RotCamPos(temp);
	}
	else
	{
		m_Cam2.m_Distance-=zDelta*m_Cam2.m_Distance*0.001;
		m_Cam2.RotCamPos(temp);
	}


	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

#pragma SIDE_FUNCTION


void CKEGIESView::OnRendermodeLine()
{
	renderMode = 1;

}


void CKEGIESView::OnRendermodeLineandsurface()
{
	renderMode = 2;

}


void CKEGIESView::OnRendermodeSurface()
{
	renderMode = 3;

}

void CKEGIESView::StartStopSim()
{
	if(m_bRunningSimulation)
	{
		m_bRunningSimulation = FALSE;
		TRACE("stop simulation");
		KillTimer(TIMER_UPDATE_SIMULATION_DATA);
	}
	else
	{
		m_bRunningSimulation = TRUE;
		TRACE("Start simulation");
		SetTimer(TIMER_UPDATE_SIMULATION_DATA,dtime,NULL);
	}
}


void CKEGIESView::OnViewDisplayaxis()
{
	m_bDisplayAxis = !m_bDisplayAxis;
	AppSetting::bShowAxis = m_bDisplayAxis;
}


void CKEGIESView::OnViewDisplaytext()
{
	m_bDisplayText = !m_bDisplayText;
	AppSetting::bShowHelpText = m_bDisplayText;

	textMode = (textMode+1)%4;
	AppSetting::saveIntSetting(textMode, "TextMode");
}

void CKEGIESView::resetTime()
{
	curTime = 0;
}


BOOL CKEGIESView::OnEraseBkgnd(CDC* pDC)
{
	if (beraseBackGrnd)
	{
		return false;
	}
	else
	{
		return CView::OnEraseBkgnd(pDC);
	}
}

// When press test button on toolbar
void CKEGIESView::test()
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

}


void CKEGIESView::OnColorBackground()
{
	backGroundColor += 1;
	backGroundColor = backGroundColor%3;
}


void CKEGIESView::OnSimulationStart()
{
	// TODO: Add your command handler code here
	CMainFrame* mainF = (CMainFrame*)AfxGetMainWnd();
	if(m_bRunningSimulation)
	{
		m_bRunningSimulation = FALSE;
		KillTimer(TIMER_TEMPLATE_SIMULATION);
		mainF->timerUpdate(false);
	}
	else
	{
		m_bRunningSimulation = TRUE;
		SetTimer(TIMER_TEMPLATE_SIMULATION,30,NULL);
		mainF->timerUpdate(true);
		
	}
}


void CKEGIESView::OnViewResetcameraview()
{
	// TODO: Add your command handler code here
	m_Cam1 = CCamera();
	m_Cam2 = CCamera();
}

LRESULT CKEGIESView::testTimer( WPARAM, LPARAM )
{
	TRACE("-----------------------------In\n");
	while (1)
	{
	}
	return 0;
}




void CKEGIESView::OnBnVoxelOk()
{
	// TODO: Add your control notification handler code here
	Vec3f e;
	Vec3f b;
	e=b;
}


LRESULT CKEGIESView::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	TRACE(_T("Fcccc----------\n"));
	return 0;
}

LRESULT CKEGIESView::OnData(WPARAM wp, LPARAM lp)
{
	dialogVoxel* dlg = (dialogVoxel*)lp;

	CString stext;
	CEdit* editbox1 = (CEdit*)dlg->GetDlgItem(IDC_EDIT1);
	editbox1->GetWindowText(stext);
	yIdx = _ttoi(stext);

	CEdit* editbox2 = (CEdit*)dlg->GetDlgItem(IDC_EDIT2);
	editbox2->GetWindowText(stext);
	zIdx = _ttoi(stext);

	return 0;
}

void CKEGIESView::update()
{
	CMainFrame* mainF = (CMainFrame*)AfxGetMainWnd();

	CString stext;
	CEdit* editbox1 = &mainF->m_edit1;
	editbox1->GetWindowText(stext);
	yIdx = _ttoi(stext);

	CEdit* editbox2 = &mainF->m_edit2;
	editbox2->GetWindowText(stext);
	zIdx = _ttoi(stext);

	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->document.updateIdx(yIdx, zIdx);

	if (pDoc->m_mode == MODE_FINDING_CUT_SURFACE)
	{
		pDoc->testCut.updateDisplay(yIdx, zIdx);
	}
	else if (pDoc->m_mode == MODE_SWAP_VOXEL)
	{
		pDoc->detailSwap.updateDisplay(yIdx, zIdx);
	}
	else if (pDoc->m_mode == MODE_SPLIT_BONE_GROUP)
	{
		pDoc->groupCutMngr.updateDisplay(yIdx, zIdx);
	}
}


void CKEGIESView::OnBnClickedButtontest()
{
	// TODO: Add your control notification handler code here
	TRACE("ff");
}

void CKEGIESView::updateNumInput()
{
	CMainFrame* mainF = (CMainFrame*)AfxGetMainWnd();
	CEdit* editbox1 = &mainF->m_edit1;
	CString text; text.Format(_T("%d"), yIdx);
	editbox1->SetWindowText(text);

	CEdit* editbox2 = &mainF->m_edit2;
	text.Format(_T("%d"), zIdx);
	editbox2->SetWindowText(text);

}

void CKEGIESView::UpdateView2()
{
	int _w = m_WindowWidth/2;
	int _h = m_WindowHeight;

	glViewport(_w,0,_w,_h);
	float fovy=45;
	float aspect=float(_w)/float(_h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fovy, aspect, 1, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_Cam2.m_Pos.x,m_Cam2.m_Pos.y,m_Cam2.m_Pos.z,
		m_Cam2.m_Center.x,m_Cam2.m_Center.y,m_Cam2.m_Center.z,
		m_Cam2.m_Up.x,m_Cam2.m_Up.y,m_Cam2.m_Up.z);
}

void CKEGIESView::DrawView2()
{
	glPushMatrix();
	UpdateView2();
	SetupView();

	if(m_bDisplayAxis)
		drawAxis(true, &m_Cam2);

	if (mode == 1)
	{
		testCut.m_skeleton.draw();
	}
	if (mode == 2)
	{
		detailSwap.m_skeleton.draw();
	}

	glPopMatrix();
}

void CKEGIESView::resetDisplayMode()
{
	for (int i = 0; i < 10; i++)
	{
		m_displayMode[i] = TRUE;
	}
}
