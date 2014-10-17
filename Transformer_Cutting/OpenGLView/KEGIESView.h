// KEGIESView.h : CKEGIESView 
//


#pragma once
#include "cutSurfTreeMngr2.h"
#include "MeshCutting.h"
#include "octreeSolid.h"
#include "detailSwapManager.h"
#include "controlDialog.h"

extern class CKEGIESDoc;

class CKEGIESView : public CView
{
protected: // serialization
	CKEGIESView();
	DECLARE_DYNCREATE(CKEGIESView)

//////////////////////////////////////////////////////////////////////////
//		Test
public:
	int mode;

	int yIdx, zIdx;
	cutSurfTreeMngr2 testCut;
//	MeshCutting meshCut;

	octreeSolid octreet;

	detailSwapManager detailSwap;
//////////////////////////////////////////////////////////////////////////

	controlDialog *dlg;

public:
	CKEGIESDoc* GetDocument() const;

// function
public:
	//Initialize
	void InitGL();

	//Drawing
	void DrawView();
	void SetupView();
	void UpdateView();

	void UpdateView2();
	void DrawView2();

	void DrawText();
	void drawAxis(bool atOrigin, CCamera* cam);

// variable
public:
	HDC     m_hDC;
    HGLRC   m_hRC;
	GLuint	base;

	//flag
	bool LEFT_DOWN;
	bool RIGHT_DOWN;

	//window
	int m_WindowHeight;
	int m_WindowWidth;

	//mouse position
	vec3d m_MousePos;
	vec3d m_PreMousePos;
	vec3d m_DMousePos;

	//camera manipulation
	int curView;
	CCamera m_Cam1;
	CCamera m_Cam2;

	vec3d* direc;
	int nbSphere;
	int renderMode;

	//Text info
	double curTime, dtime;
	CString helpText;
	CString timeText;

	bool m_bDisplayAxis;
	bool m_bDisplayText;
	int textMode;
	BOOL m_displayMode[10];

	// Color
	int backGroundColor;
	//
	BOOL m_bRunningSimulation;

	//Flickering
	BOOL beraseBackGrnd;

// 
public:
	virtual void OnDraw(CDC* pDC);  // 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	void InitAfterShowWindow();
	void setTextDisplay(CString text);
	void setDisplayOptions(arrayInt opts);
// 
public:
	virtual ~CKEGIESView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void resetTime();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual void OnInitialUpdate();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	//Key board
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//Timer
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//Mouse function
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	//Menu
	afx_msg void OnRendermodeLine();
	afx_msg void OnRendermodeLineandsurface();
	afx_msg void OnRendermodeSurface();
	afx_msg void StartStopSim();
	afx_msg void OnViewDisplayaxis();
	afx_msg void OnViewDisplaytext();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnColorBackground();
	afx_msg void OnSimulationStart();
	afx_msg void OnViewResetcameraview();

	afx_msg LRESULT testTimer(WPARAM, LPARAM);
	afx_msg void test();
	afx_msg void update();
	afx_msg void OnBnVoxelOk();
	afx_msg LRESULT OnBnClickedButton1();

	afx_msg LRESULT OnData(WPARAM wp, LPARAM );
	afx_msg void OnBnClickedButtontest();
	void updateNumInput();
	void resetDisplayMode();

};

#ifndef _DEBUG  // KEGIESView
inline CKEGIESDoc* CKEGIESView::GetDocument() const
   { return reinterpret_cast<CKEGIESDoc*>(m_pDocument); }
#endif

