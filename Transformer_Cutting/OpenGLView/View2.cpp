#include "StdAfx.h"
#include "View2.h"

#include "KEGIESDoc.h"
#include "KEGIESView.h"
#include "MainFrm.h"
#include "GL\glut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(View2, CView)

	BEGIN_MESSAGE_MAP(View2, CView)
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
		ON_COMMAND(ID_COLOR_BACKGROUND, &View2::OnColorBackground)
	END_MESSAGE_MAP()

View2::View2(void)
{
	bColorIdx = 2;
}

View2::~View2()
{

}

void View2::InitGL()
{
	COpenGL Initgl;

	//Init
	Initgl.SetHWND(m_hWnd);
	Initgl.SetupPixelFormat();
	base=Initgl.base;

	m_hDC=Initgl.m_hDC;
	m_hRC=Initgl.m_hRC;

	m_Cam1 = AppSetting::loadcamera();
}

void View2::OnDraw(CDC* pDC)
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 
	wglMakeCurrent(m_hDC,m_hRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	arrayVec3f bColor = {Vec3f(0,0,0), Vec3f(0.7,0.7,0.7), Vec3f(1,1,1)};
	glClearColor(bColor[bColorIdx][0], bColor[bColorIdx][1], bColor[bColorIdx][1], 1);

	DrawView();

	SwapBuffers(m_hDC);
}

void View2::DrawView()
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	glPushMatrix();
	UpdateView();
	SetupView();

	//drawAxis(true, &m_Cam1);

	if (pDoc->m_mode == MODE_FINDING_CUT_SURFACE)
	{
		pDoc->testCut.m_skeleton.draw();
	}
	else if (pDoc->m_mode == MODE_ASSIGN_COORDINATE)
	{
		pDoc->detailSwap.m_skeleton.draw();
	}
	else if (pDoc->m_mode == MODE_SWAP_VOXEL )
	{
		pDoc->detailSwap.m_skeleton.draw();
	}
	else if (pDoc->m_mode == MODE_SPLIT_BONE_GROUP)
	{
		pDoc->groupCutMngr.m_skeleton.draw();
	}

	pDoc->document.draw2(m_displayMode);

	glPopMatrix();
	glPopAttrib();
}

BOOL View2::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}

#ifdef _DEBUG
void View2::AssertValid() const
{
	CView::AssertValid();
}

void View2::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKEGIESDoc* View2::GetDocument() const // 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKEGIESDoc)));
	return (CKEGIESDoc*)m_pDocument;
}

#endif //_DEBUG

int View2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(TIMER_UPDATE_VIEW,10,NULL);

	return 0;
}

void View2::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	LEFT_DOWN=false;
	RIGHT_DOWN=false;

	InitGL();
}

void View2::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CSize size(cx,cy);
	m_WindowHeight=size.cy;
	m_WindowWidth=size.cx;
}

void View2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	char lsChar;
	lsChar = char(nChar);

	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->document.receiveKey(lsChar);

	if (nChar >= 48 && nChar <= 57   )
	{
		m_displayMode[nChar - 48] = ! m_displayMode[nChar - 48];
	}


	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void View2::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_UPDATE_VIEW)
	{
		InvalidateRect(NULL, FALSE);
	}
	CView::OnTimer(nIDEvent);
}

void View2::OnLButtonDown(UINT nFlags, CPoint point)
{
	LEFT_DOWN=true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnLButtonDown(nFlags, point);
}

void View2::OnLButtonUp(UINT nFlags, CPoint point)
{
	LEFT_DOWN=false;
	CView::OnLButtonUp(nFlags, point);
}

void View2::OnRButtonDown(UINT nFlags, CPoint point)
{
	RIGHT_DOWN=true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnRButtonDown(nFlags, point);
}

void View2::OnRButtonUp(UINT nFlags, CPoint point)
{
	RIGHT_DOWN=false;
	CView::OnRButtonUp(nFlags, point);
}

void View2::OnMouseMove(UINT nFlags, CPoint point)
{
	m_MousePos.x=point.x;
	m_MousePos.y=-point.y;
	m_DMousePos=m_MousePos-m_PreMousePos;

	if(LEFT_DOWN)
	{
			m_Cam1.RotCamPos(m_DMousePos);
	}
	else if(RIGHT_DOWN)
	{
			m_Cam1.MoveCamPos(m_DMousePos);
	}

	m_PreMousePos=m_MousePos;
	CView::OnMouseMove(nFlags, point);
}

BOOL View2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	vec3d temp;

	m_Cam1.m_Distance-=zDelta*m_Cam1.m_Distance*0.001;
	m_Cam1.RotCamPos(temp);


	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void View2::drawAxis(bool atOrigin, CCamera* cam)
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

void View2::SetupView()
{
	GLfloat diffuseLight[] = {0.4f,0.4f,0.4f,1.0f};
	GLfloat ambientLight[] = {0.2f,0.2f,0.2f,1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glEnable(GL_DEPTH_TEST);                                        
	//  	glEnable(GL_CULL_FACE);  
	// 
	// 		glDisable(GL_DEPTH_TEST); 
	// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// 		glEnable(GL_BLEND);

	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHTING);   
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glEnable(GL_LIGHT0);

	glFrontFace(GL_CW);

	glShadeModel(GL_SMOOTH); 
	//glShadeModel(GL_FLAT);
	glPolygonMode(GL_FRONT, GL_FILL);
}

void View2::UpdateView()
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

void View2::OnColorBackground()
{
	bColorIdx++;
	bColorIdx = bColorIdx % 3;

}
