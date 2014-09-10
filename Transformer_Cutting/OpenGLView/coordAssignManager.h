#pragma once
#include "coordAsignDlg.h"
#include "detailSwapManager.h"


class coordAssignManager
{
public:
	coordAssignManager(void);
	~coordAssignManager(void);
	
	void assignCoord(detailSwapManager* detailSwap, CWnd* parent);
	void init(std::vector<bone*> boneFullArray, std::vector<bvhVoxel> meshBoxFull);

	void draw2(BOOL mode[10]);

	void draw(BOOL mode[10]);

public:
	detailSwapManager *s_detailSwap;
	coordAsignDlg *dlg;

	std::vector<bone*> m_boneFullArray;
	std::vector<bvhVoxel> m_meshBoxFull;
};

