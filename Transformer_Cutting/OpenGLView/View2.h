#pragma once
#include "KEGIESDoc.h"

class View2 : public CView
{
protected: // serialization
	View2();
	DECLARE_DYNCREATE(View2)

public:
	bool m_displayMode[10];
	CKEGIESDoc* GetDocument() const;

	// function
public:
	//Initialize
	void InitGL();

	//Drawing
	void DrawView();
	void SetupView();
	void UpdateView();
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
	CCamera m_Cam1;

	// 
	int bColorIdx;
public:
	virtual void OnDraw(CDC* pDC);  // 
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// 
public:
	virtual ~View2();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

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

	afx_msg void OnColorBackground();
};

#ifndef _DEBUG  // KEGIESView
inline CKEGIESDoc* View2::GetDocument() const
{ return reinterpret_cast<CKEGIESDoc*>(m_pDocument); }
#endif