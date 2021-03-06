#include "stdafx.h"
#include "myDocment.h"
#include <shobjidl.h>
#include "write_ply.hpp"
#include <iostream>
#include <afxwin.h>
#include "log.h"
#include "MainFrm.h"
#include "KEGIESView.h"

using namespace std;

myDocment::myDocment()
{
	m_swapMngr = nullptr;
	m_groupCutMngr = nullptr;
	m_coordAssign = nullptr;
	m_finalSwap = nullptr;
	m_meshCutting = nullptr;
	m_lowResVoxel = nullptr; 
	m_highResVoxel = nullptr; 
	m_skeleton = nullptr;
	cutFilterDialog = nullptr;
	m_surfaceObj = nullptr;
	view1 = nullptr;
	view2 = nullptr;

	shift = 0;
	m_debug = debugInfoPtr(new debugInfo);

	m_curMode = MODE_NONE;

	std::cout << endl << "Press 'S' to construct the cut tree" << endl << endl;


}


myDocment::~myDocment()
{
	if (cutFilterDialog)
	{
		delete cutFilterDialog;
	}
	if (m_meshCutting)
	{
		delete m_meshCutting;
	}

	if (m_surfaceObj)
	{
		delete m_surfaceObj;
	}

	if (m_highResVoxel)
	{
		delete m_highResVoxel;
	}

	if (m_lowResVoxel)
	{
		delete m_lowResVoxel;
	}

	if (m_skeleton)
	{
		delete m_skeleton;
	}

	if (m_swapMngr)
	{
		delete m_swapMngr;
	}

	if (m_groupCutMngr)
	{
		delete m_groupCutMngr;
	}

	if (m_coordAssign)
	{
		delete m_coordAssign;
	}

	if (m_finalSwap)
	{
		delete m_finalSwap;
	}

	if (m_highResFullVoxel)
	{
		delete m_highResFullVoxel;
		m_highResFullVoxel = nullptr;
	}
}

void myDocment::draw(BOOL mode[10])
{
	static arrayVec3f color = Util_w::randColor(10);

	if (m_curMode == MODE_TEST)
	{
		drawTest(mode);
		return;
	}

	if (mode[1] && m_surfaceObj)
	{
		glColor3f(0.8, 0.8, 0.8);
		m_surfaceObj->drawObject();
	}

	if (mode[2] && m_lowResVoxel)
	{
		glColor3f(0, 0, 0);
		m_lowResVoxel->drawVoxelLeaf();
		glColor3f(0.9, 0.9, 0.9);
		m_lowResVoxel->drawVoxelLeaf(1);
	}

	if (mode[3] && m_lowResVoxel)
	{
		glColor3f(1, 0, 0);
		m_lowResVoxel->drawVoxel();
	}

	if (m_curMode == MODE_NONE)
	{
		if (mode[4] && m_surfaceObj)
		{
			glColor3f(1, 0, 0);
			m_surfaceObj->drawBVH();
		}
		if (mode[5] && m_surfaceObj)
		{
			glColor3f(0, 0, 1);
			m_surfaceObj->drawWireFrame();
		}
		if (mode[6] && m_highResFullVoxel)
		{
			glColor3f(0, 0, 0);
			m_highResFullVoxel->drawVoxelLeaf();
			glColor3f(0.9, 0.9, 0.9);
			m_highResFullVoxel->drawVoxelLeaf(1);
		}
		if (mode[7] && holeMesh)
		{
			holeMesh->drawSeparatePart();
		}
	}

	if (m_curMode == MODE_FINDING_CUT_SURFACE)
	{
		if (mode[4])
		{
			m_cutSurface.drawLeaf(idx1);
		}
		if (mode[5])
		{
			if (m_swapMngr)
			{
				m_swapMngr->draw();
			}
		}
	}
	else if (m_curMode == MODE_SPLIT_BONE_GROUP)
	{
		if (mode[4])
		{
			m_swapMngr->draw();
		}
		
		if (mode[5])
		{
			m_groupCutMngr->draw(mode);
		}
	}
	else if (m_curMode == MODE_ASSIGN_COORDINATE)
	{
		if (m_coordAssign)
		{
			m_coordAssign->draw2(mode);
		}
	}
	else if (m_curMode == MODE_FIT_BONE)
	{
		if (mode[4] && m_finalSwap)
		{
			m_finalSwap->draw();
		}
		if (m_voxelProcess)
		{
			if (mode[5])
			{
				m_voxelProcess->drawCoord(m_boxPlaceMngr->getCurBoneIdx());
				m_voxelProcess->drawBoxInterfere();
			}

			if (mode[6])
			{
				m_voxelProcess->drawVoxelBox();
			}


		}
	}
	else if (m_curMode == MODE_CUTTING_MESH)
	{
		m_meshCutting->draw(mode);
	}
}

void myDocment::draw2(bool mode[10])
{
	if (m_curMode == MODE_TEST)
	{
		return;
	}

	if (mode[1] && m_skeleton) // draw normal bone
	{
		glLineWidth(2.0);
		glColor3f(0, 0, 1);
		m_skeleton->draw(SKE_DRAW_BOX_WIRE);
		glLineWidth(1.0);
	}


	if (m_curMode == MODE_ASSIGN_COORDINATE)
	{
		if (mode[2] && m_coordAssign)
		{
			m_coordAssign->drawBoneMap();//mapping
			m_skeleton->drawBoneWithMeshSize();
		}
	}
	else
	{
		if (mode[2] && m_skeleton)
		{
			glColor3f(1, 1, 1);
			m_skeleton->draw(SKE_DRAW_BOX_SOLID);
		}

		if (mode[3] && m_skeleton)
		{
			glColor3f(0, 0, 1);
			m_skeleton->drawGroup(SKE_DRAW_BOX_WIRE);
		}
		if (mode[4] && m_skeleton)
		{
			glColor3f(1, 1, 1);
			m_skeleton->drawGroup(SKE_DRAW_BOX_SOLID);
		}
	}

}

void myDocment::initState()
{
	if (m_curMode == MODE_FINDING_CUT_SURFACE)
	{

	}
}

void myDocment::receiveKey(UINT nchar)
{
	char c = char(nchar);
	CWaitCursor w;

	if (m_curMode == MODE_TEST)
	{
		keyPressModeTest(c);
		return;
	}

	if (c == 'S')
	{
		changeState();
	}
	
	if (m_curMode == MODE_NONE)
	{
		if (c == 'D')
		{
			loadSwapGroupFromFile();
			return;
		}
		if (c == 'F')
		{
			//loadStateForFinalSwap();
			m_curMode = MODE_FIT_BONE;
			loadStateForPostProcess();
			return;
		}

		if (c == 'R') // reload skeleton
		{
			if (m_skeleton)
			{
				delete m_skeleton;
			}
			m_skeleton = new skeleton;
			char* skeletonPath = "../../Data/skeleton.xml";
			m_skeleton->loadFromFile(skeletonPath);
			m_skeleton->computeTempVar();
			m_skeleton->groupBone();
			cprintf("Load skeleton: %s\n", skeletonPath);
		}
	}

	if (m_curMode == MODE_FINDING_CUT_SURFACE)
	{
		if (nchar == VK_LEFT || nchar == VK_RIGHT || c == 'B') // Best configuration
		{
			int cofIdx = m_cutSurface.updateBestIdxFilter(idx1);
			// update
			CMainFrame* mainF = (CMainFrame*)AfxGetMainWnd();

			CEdit* editbox2 = &mainF->m_edit2;
			CString text; 
			text.Format(_T("%d"), cofIdx);
			editbox2->SetWindowTextW(text);
			return;
		}
		if (c == 'E')
		{
			loadSwapGroupFromFile();
		}
		if (c == 'D') // Swap voxel
		{
			changeToWrapMode();
		}
		if (c == 'R') // Save box state
		{
			saveCurrentBoxCut();
		}
		if (c == 'F')
		{
			if (m_swapMngr)
			{
				m_swapMngr->swapPossibleLayerWithVolume();
			}
		}
		if (c == 'G')
		{
			if (m_swapMngr)
			{
				m_swapMngr->swapVoxel2();
			}
		}
		if (c == 'P') // Debug
		{
			m_cutSurface.updateDebugDrawOfNode(1);
		}
		if (c == 'O')
		{
			m_cutSurface.updateDebugDrawOfNode(-1);
		}
	}

	if (m_curMode == MODE_FIT_BONE)
	{
		if (c == 'F')
		{
			m_voxelProcess->resolveVoxelBox();
		}
		if (c == 'R')
		{
			writeMeshBoxStateFinalSwap();
		}
		if (c == 'W')
		{
			//loadStateForFinalSwap();
			loadStateForPostProcess();
		}
	}

	if (m_curMode == MODE_CUTTING_MESH)
	{
		if (c == 'F')
		{
			m_meshCutting->cutTheMesh();
			m_meshCutting->CopyMeshToBone();
		}
		if (c == 'D')
		{
			saveFile();
		}
		if (c == 'E')
		{
			saveCutMeshToObj();
		}
	}

	w.Restore();
}

UINT myDocment::swapVoxelThread(LPVOID p)
{
	detailSwapManager * finalSwap = (detailSwapManager *)p;
	finalSwap->m_bStopThread = false;
	finalSwap->swapVoxelTotal();
	return 0;
}

void myDocment::changeState()
{

	switch (m_curMode)
	{
	case MODE_NONE:
		m_curMode = MODE_FINDING_CUT_SURFACE;
		constructCutTree();
		break;
	case MODE_FINDING_CUT_SURFACE:
		changeToCutGroupBone();
		break;
	case MODE_SPLIT_BONE_GROUP:
		changeToCoordAssignMode();
		break;
	case MODE_ASSIGN_COORDINATE:
		changeToSwapFinal();
		break;
	case MODE_FIT_BONE:
		changeToCuttingMeshMode();
		break;
	default:
		break;
	}

}

void myDocment::constructCutTree()
{
	// 1. Set skeleton and voxel
	cout << endl << "--------------------" << endl
		<< "Construct cut tree" << endl;

	CTimeTick time;
	time.SetStart();

	m_cutSurface.s_groupSkeleton = m_skeleton;
	m_cutSurface.s_voxelObj = m_lowResVoxel;
	m_cutSurface.bUniformCut = false;
	m_cutSurface.init();
	
	time.SetEnd();
	cout << "Construction time: " << time.GetTick() << " ms" << endl
		 << "Number of configurations: " << m_cutSurface.m_tree2.leaves.size() 
		 << " (" << m_cutSurface.m_tree2.poseMngr->poseMap.size() << " groups)" << endl
		<< "Now choose the configuration\n"
		<< " - 'LEFT' and 'RIGHT' to change pose index\n"
		<< " - 'UP' and 'DOWN' to change configuration index\n"
		<< " - 'B' for best option in pose group\n"
		<< " - 'D' to voxelize and 'F' to single swap "
		<< " and 'G' to swap\n"
		<< " - 'S' to change to state splitting group bones\n"
		<< endl;
	cout << "Display options" << endl
		<< " - 1: Surface object" << endl
		<< " - 2: Voxel object" << endl
		<< " - 3: Octree" << endl
		<< " - 4: Draw cut box" << endl
		<< " - 5: draw swapping voxel" << endl;

	view1->setDisplayOptions({0, 1, 0, 0, 1, 1});

	cutFilterDialog = new FilterCutDialog;
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CView * pView = pFrame->GetActiveView();
	cutFilterDialog->Create(FilterCutDialog::IDD, pView);
	cutFilterDialog->initFromCutTree(&m_cutSurface);
	cutFilterDialog->doc = this;
	cutFilterDialog->ShowWindow(SW_SHOW);

	updateFilterCutGroup();
}

void myDocment::updateIdx(int yIdx, int zIdx)
{
	idx1 = yIdx;
	idx2 = zIdx;

	if (m_curMode == MODE_FINDING_CUT_SURFACE)
	{
		m_cutSurface.updateDisplayFilter(idx1, idx2);
	}
	else if (m_curMode == MODE_SPLIT_BONE_GROUP)
	{
		m_groupCutMngr->updateDisplay(idx1, idx2);
	}
	if (m_curMode == MODE_CUTTING_MESH)
	{
		m_meshCutting->updateScale(idx1/100.0);
		m_skeleton->meshScale = float(idx1) / 100.0;
	}
	if (m_curMode == MODE_FIT_BONE)
	{
		//m_finalSwap->updateDisplay(idx1, idx2);
		if (m_voxelProcess)
			m_voxelProcess->updateParam(idx1, idx2);
	}
}

void myDocment::changeToWrapMode()
{
	if (!m_cutSurface.curNode)
	{
		AfxMessageBox(_T("Choose configuration first"));
		return;
	}
	if (m_swapMngr)
	{
		delete m_swapMngr;
	}

	cout << "Voxelize the object\n";

	m_swapMngr = new detailSwapManager;

	m_cutSurface.leatE2Node2 = m_cutSurface.curNode;
	m_swapMngr->s_obj = m_surfaceObj;
	m_swapMngr->s_skeleton = m_skeleton;
	m_swapMngr->s_octree = &m_lowResVoxel->m_octree;
	m_swapMngr->s_hashTable = &m_lowResVoxel->m_hashTable;
	m_swapMngr->s_boxes = &m_lowResVoxel->m_boxes;
	m_swapMngr->s_boxShareFaceWithBox = &m_lowResVoxel->m_boxShareFaceWithBox;
	m_swapMngr->voxelSize = m_lowResVoxel->m_voxelSizef;
	m_swapMngr->initFromCutTree2(&m_cutSurface);

	cout << " - 'F' to swap voxel for better box" << endl
		<< " - 'D' to reset to swapping" << endl
		<< " - 'S' to change to spliting group" << endl;

//	m_swapMngr->swapVoxel2();
}

void myDocment::changeToCutGroupBone()
{
	if (!m_swapMngr)
	{
		AfxMessageBox(_T("Box cut is not ready"));
		return;
	}

	cutFilterDialog->EndDialog(0);

	m_curMode = MODE_SPLIT_BONE_GROUP;
	
	if (m_groupCutMngr)
	{
		delete m_groupCutMngr;
	}

	cout << endl << "------------------------" << endl
		<< "Split group bones" << endl;

	m_groupCutMngr = new groupCutManager;

	m_groupCutMngr->s_skeleton = m_skeleton;
	m_groupCutMngr->s_surObj = m_surfaceObj;
	m_groupCutMngr->s_hashTable = &m_highResVoxel->m_hashTable;
	m_groupCutMngr->s_boxes = &m_highResVoxel->m_boxes;
	m_groupCutMngr->s_boxShareFaceWithBox = &m_highResVoxel->m_boxShareFaceWithBox;
	m_groupCutMngr->s_octree = &m_highResVoxel->m_octree;

	m_groupCutMngr->initFromSwapBox(m_swapMngr);
	m_groupCutMngr->showDialog();

	cout << "Use the dialog to choose the configurations of group bones" << endl
		<< " - Press 'S' to ready to assign coordinate to bones" << endl;

	cout << "Display options: " << endl
		<< " - 1: Surface object" << endl
		<< " - 2: Voxel object" << endl
		<< " - 3: Octree" << endl
		<< " - 4: Draw voxel in cut box" << endl
		<< " - 5: Draw box group" << endl;

	view1->setDisplayOptions({ 0, 0, 0, 0, 0, 1 });
}

void myDocment::changeStateBack()
{
	if (m_curMode == MODE_SPLIT_BONE_GROUP)
	{
		m_curMode == MODE_FINDING_CUT_SURFACE;
		if (m_groupCutMngr)
		{
			delete m_groupCutMngr;
			m_groupCutMngr = nullptr;
		}
	}
}

void myDocment::changeToCoordAssignMode()
{
	if (m_groupCutMngr->m_idxChoosen.size() == 0)
	{
		AfxMessageBox(_T("Press 'Apply to mesh'"));
		return;
	}

	// Check if user cut all the mesh
	arrayVec2i idxC = m_groupCutMngr->m_idxChoosen;
	for (int i = 0; i < idxC.size(); i++)
	{
		if (idxC[i][0] == -1 || idxC[i][1] == -1)
		{
			AfxMessageBox(_T("Not set all box"));
			return;
		}
	}

	m_curMode = MODE_ASSIGN_COORDINATE;

	if (m_coordAssign)
	{
		delete m_coordAssign;
	}

	m_coordAssign = new coordAssignManager;
	// We need array of all mesh box and bone, by order
	std::vector<bone*> boneFullArray;
	std::vector<bvhVoxel> meshBoxFull;

	getBoneArrayAndMeshBox(boneFullArray, meshBoxFull);

	// Now init the coord assign
	m_coordAssign->s_detailSwap = m_swapMngr;
	m_coordAssign->init(boneFullArray, meshBoxFull);

	cout << "-----------------------------" << endl
		<< "Use dialog to assign coordinate to bones" << endl
		<< "Display options: " << endl
		<< " - 1: Surface object" << endl
		<< " - 2: Voxel object" << endl
		<< " - 3: Octree" << endl;

	view1->setDisplayOptions({ 0 });
}

void myDocment::getBoneArrayAndMeshBox(std::vector<bone*> &boneFullArray, std::vector<bvhVoxel> &meshBoxFull)
{
	if (!m_swapMngr || !m_groupCutMngr)
		return;

	std::vector<bone*> * boneArray = & m_swapMngr->boneArray;
	std::vector<bvhVoxel*> * meshBox = & m_swapMngr->meshBox;

	// Init the bone and mesh array
	boneFullArray = *boneArray;
	// Copy mesh
	meshBoxFull.resize(meshBox->size());
	for (int i = 0; i < meshBox->size(); i++)
	{
		meshBoxFull[i] = *meshBox->at(i);
		meshBoxFull[i].Root = nullptr;
	}

	// The group
	std::vector<groupCut> * boneGroupArray = & m_groupCutMngr->boneGroupArray;
	for (int i = 0; i < boneGroupArray->size(); i++)
	{
		std::vector<bone*> boneInGroup; // First bone is the group bone
		std::vector<meshPiece> cutBoxByBoneOrder;
		m_groupCutMngr->getConfiguration(i, boneInGroup, cutBoxByBoneOrder);

		// Modify the array
		// First, find the bone
		arrayBone_p::iterator it = find(boneFullArray.begin(), boneFullArray.end(), boneInGroup[0]);
		int idx = it - boneFullArray.begin();

		boneFullArray.erase(boneFullArray.begin() + idx);
		meshBoxFull.erase(meshBoxFull.begin() + idx);
		for (int j = 0; j < boneInGroup.size(); j++)
		{
			bone* curBone = boneInGroup[j];
			boneFullArray.push_back(curBone);

			bvhVoxel newMesh;
			meshPiece curPiece = cutBoxByBoneOrder[j];
			newMesh.boneName = curBone->m_name;
			newMesh.boneType = curBone->m_type;
			newMesh.leftDown = curPiece.leftDown;
			newMesh.rightUp = curPiece.rightUp;
			newMesh.expectedVolRatio = curBone->m_volumeRatio;
			newMesh.voxelSize = m_highResVoxel->m_voxelSizef;
			newMesh.hashTable = & m_highResVoxel->m_hashTable;
			newMesh.s_octree = & m_highResVoxel->m_octree;
			newMesh.boxes = &m_highResVoxel->m_boxes;
			newMesh.voxelIdxs = curPiece.voxels;
			newMesh.curLeftDown = curPiece.leftDown;
			newMesh.curRightUp = curPiece.rightUp;

			meshBoxFull.push_back(newMesh);
		}
	}
}

void myDocment::changeToSwapFinal()
{
	arrayVec3i boneMeshCoordMap = m_coordAssign->dlg->coords;
	for (auto c : boneMeshCoordMap)
	{
		if (c[0] == -1 || c[1] == -1 || c[2] == -1)
		{
			AfxMessageBox(_T("Not all coords are set"));
			return;
		}
	}

	m_curMode = MODE_FIT_BONE;

	writeMeshBoxStateFinalSwap();

	loadStateForPostProcess();

	m_coordAssign->dlg->EndDialog(0);

	// Guide
	cout << "---------------------------------" << endl
		<< "Swap by box placing" << endl
		<< "Use the dialog to place the box" << endl
		<< " - Press F to resolve the conflict\n"
		<< " - Press R to save to mesh\n"
		<< " - Press W to reload the mesh\n"
		<< " - Press 'S' to cut the triangular mesh\n"
		<< "Display options" << endl
		<< " - 1: Surface object" << endl
		<< " - 2: Voxel object" << endl
		<< " - 3: Octree" << endl
		<< " - 4: cut box" << endl
		<< " - 5: Box placed" << endl
		<< " - 6: Voxel occupied by box placed" << endl;
		 
	view1->setDisplayOptions({ 0, 0, 0, 0, 1, 1, 0 });
}

void myDocment::changeToCuttingMeshMode()
{
	m_curMode = MODE_CUTTING_MESH;
	if (m_meshCutting)
	{
		delete m_meshCutting;
	}

	CTimeTick time;
	time.SetStart();

	cout << "Generate cut surface from voxel" << endl;

	m_meshCutting = new MeshCutting;
	m_meshCutting->s_voxelObj = m_highResFullVoxel;
	m_meshCutting->s_surObj = m_surfaceObj;
	m_meshCutting->coords = m_voxelProcess->coords;
	vector<arrayInt> voxelIdxBones = getVoxelIdxFullFromVoxelProcess();
	m_meshCutting->init2(voxelIdxBones, m_voxelProcess->boneArray);

	time.SetEnd();
	cout << "\n-------------------------------------" << endl
		<< "Change to cutting mesh state" << endl
		<< "Change state time: " << time.GetTick() << " ms" << endl
		<< "Change to cutting triangular mesh \n"
		<< " - Press 'F' to cut the original mesh and 'D' to save to file \n" << endl
		<< " - Press 'E' to export cut mesh to obj file" << endl
		<< "Display options: " << endl
		<< " - 1: Surface object" << endl
		<< " - 2: Voxel object" << endl
		<< " - 3: Octree" << endl
		<< " - 4: draw cut surface generated by voxel" << endl
		<< " - 5: Draw cut pieces" << endl;

	view1->setDisplayOptions({ 0, 0, 0, 0, 1, 1 });
}

void myDocment::writeMeshBoxStateFinalSwap(){



	// Write meshbox
	{
		std::vector<bvhVoxel> * meshBox = &m_coordAssign->m_meshBoxFull;;

		char * path = "../stateFinalSwap_meshBoxIdxs.txt";
		FILE* f = fopen(path, "w");
		ASSERT(f);

		fprintf(f, "%d\n", meshBox->size()); // Number of mesh box
		for (int i = 0; i < meshBox->size(); i++){
			arrayInt idxs = meshBox->at(i).voxelIdxs;
			fprintf(f, "%d\n", idxs.size()); // Number of voxel idxs
			
			for (int j = 0; j < idxs.size(); j++){
				fprintf(f, "%d\n", idxs[j]);
			}
		}

		fclose(f);
		command::print("Voxel boxes is saved to file: %s", path);
	}

	// Write bone order
	{
	std::vector<bone*> boneArray = m_coordAssign->m_boneFullArray;
	arrayVec3i boneMeshCoordMap = m_coordAssign->dlg->coords;
	char * path = "../stateFinalSwap_boneArray.txt";
		FILE* f = fopen(path, "w");
		ASSERT(f);

		fprintf(f, "%d\n", boneArray.size()); // Number of bone
		for (int i = 0; i < boneArray.size(); i++){
			bone* curB = boneArray[i];
			Vec3i coord = boneMeshCoordMap[i];
			fprintf(f, "%s\n", CStringA(curB->m_name).GetBuffer()); // Bone name
			fprintf(f, "%d %d %d\n", coord[0], coord[1], coord[2]);
		}
		fclose(f);

		command::print("Bones order saved: %s", path);
	}
}

void myDocment::startToStateCuttingMesh()
{
	m_curMode = MODE_CUTTING_MESH;
	std::vector<arrayInt> meshIdx;

	// Load voxel box indexes of cut mesh
	{
		FILE* f = fopen("../stateFinalSwap_meshBoxIdxs.txt", "r");
		ASSERT(f);
		int nbMesh;
		fscanf(f, "%d\n", &nbMesh);
		for (int i = 0; i < nbMesh; i++)
		{
			arrayInt idxs;
			int nbv;
			fscanf(f, "%d\n", &nbv);
			idxs.resize(nbv);
			for (int j = 0; j < nbv; j++)
			{
				fscanf(f, "%d\n", &idxs[j]);
			}
			meshIdx.push_back(idxs);
		}
		fclose(f);
	}

	// Load bone order
	std::vector<bone*> boneArray;
	std::vector<CString> name;
	arrayVec3i coords;

	{
		FILE *f = fopen("../stateFinalSwap_boneArray.txt", "r");
		int nbBone;
		fscanf(f, "%d\n", &nbBone);
		coords.resize(nbBone);
		for (int i = 0; i < nbBone; i++)
		{
			char tmp[50];
			fscanf(f, "%s\n", tmp);
			name.push_back(CString(tmp));
			fscanf(f, "%d %d %d\n", &coords[i][0], &coords[i][1], &coords[i][2]);
		}

		std::vector<bone*> bones;
		m_skeleton->getSortedBoneArray(bones);
		for (int i = 0; i < name.size(); i++)
		{
			CString n = name[i];
			for (int j = 0; j < bones.size(); j++)
			{
				if ((bones[j]->m_name.Compare(n)) == 0)
				{
					boneArray.push_back(bones[j]);
					break;
				}
			}
		}
	}

	// Create mesh cutting
	if (m_meshCutting)
	{
		delete m_meshCutting;
	}

	m_meshCutting = new MeshCutting;

	m_meshCutting->s_voxelObj = m_highResVoxel;
	m_meshCutting->s_surObj = m_surfaceObj;
	m_meshCutting->coords = coords;

	m_meshCutting->init2(meshIdx, boneArray);
}

void myDocment::saveFile()
{
// 	CString path;
// 	if (!getSavePath(path))
// 	{
// 		AfxMessageBox(_T("Error choosing output folder"));
// 		return;
// 	}

	CString path = _T("../../temporary/meshes");
	if (!PathFileExists(path))
	{
		AfxMessageBox(_T("Folder does not exist"));
		return;
	}

	arrayBone_p boneArray = m_meshCutting->boneArray;

	// Write the cutting information
	myXML infoFile;

	myXMLNode * mainNode = infoFile.addNode(XML_INFOR);

	// Write the original mesh
	CStringA originalMesh(path);
	CStringA originalMeshName("originalMesh.obj");
	originalMesh += ("\\");
	originalMesh += originalMeshName;
	infoFile.addElementToNode(mainNode, XML_ORIGINAL_MESH_KEY, originalMeshName.GetBuffer());
	m_surfaceObj->writeObjMayaData(originalMesh.GetBuffer());

	// Write the skeleton information
	CStringA skeletonPath(path);
	CStringA skeletonPathName("skeleton.xml");
	skeletonPath += "\\";
	skeletonPath += skeletonPathName;
	infoFile.addElementToNode(mainNode, XML_SKELETON_MESH_KEY, skeletonPathName.GetBuffer());
	m_skeleton->writeToFile(skeletonPath.GetBuffer());

	// Write the cut mesh in global coord
	std::vector<Polyhedron*> cutPieces = m_meshCutting->m_cutPieces;
	arrayVec3i xyzCoord = {Vec3i(1,0,0), Vec3i(0,1,0), Vec3i(0,0,1)};
	arrayVec3i coord = m_meshCutting->coords;
	arrayVec3f coordOrign = m_meshCutting->getMeshCoordOrigin();
	for (int i = 0; i < cutPieces.size(); i++)
	{
		CStringA meshPath(path);
		CStringA meshPathName; meshPathName.Format("meshPath_%d.obj", i);
		meshPath.AppendFormat("\\%s", meshPathName.GetBuffer());

		myXMLNode * meshPartNode = infoFile.addNodeToNode(mainNode, XML_MESH_PART);
		infoFile.addElementToNode(meshPartNode, XML_CUT_MESH_NAME, std::string(meshPathName));
		infoFile.addElementToNode(meshPartNode, XML_BONE_NAME, std::string(CStringA(boneArray[i]->m_name)));
		myXMLNode* coordNode = infoFile.addNodeToNode(meshPartNode, XML_BONE_COORD_RESPECT_TO_WORLD);
		infoFile.addVectorDatafToNode(coordNode, XML_ORIGIN, coordOrign[i]);
		infoFile.addVectoriToNode(coordNode, XML_LOCAL_X_RESPECT_TO_WORLD, xyzCoord[coord[i][0]]);
		infoFile.addVectoriToNode(coordNode, XML_LOCAL_Y_RESPECT_TO_WORLD, xyzCoord[coord[i][1]]);
		infoFile.addVectoriToNode(coordNode, XML_LOCAL_Z_RESPECT_TO_WORLD, xyzCoord[coord[i][2]]);

		convertPolyHedronToMayaObj(cutPieces[i], meshPath.GetBuffer());

		// Write symmetric bone
		if (m_meshCutting->boneArray[i]->m_type == SIDE_BONE)
		{
			CStringA meshPathSym(path);
			CStringA meshPathNameN; meshPathNameN.Format("meshPath_%d_sym.obj", i);
			meshPathSym.AppendFormat("\\%s", meshPathNameN.GetBuffer());
			Polyhedron* symPiece = getSymmetric_by_X(cutPieces[i]);
			convertPolyHedronToMayaObj(symPiece, meshPathSym.GetBuffer());
			delete symPiece;
		}
	}

	CStringA infoPath(path);
	infoPath.Append("\\info.xml");
	infoFile.save(infoPath.GetBuffer());

	AfxMessageBox(_T("File saved"));
}

bool myDocment::getSavePath(CString &path)
{
	BROWSEINFO   bi;
	ZeroMemory(&bi, sizeof(bi));
	TCHAR   szDisplayName[MAX_PATH];
	szDisplayName[0] = ' ';

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = _T("Please select a folder for storing received files :");
	bi.ulFlags = BIF_USENEWUI;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST   pidl = SHBrowseForFolder(&bi);
	TCHAR   szPathName[MAX_PATH];
	if (NULL != pidl)
	{
		BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
		if (FALSE == bRet)
			return false;

		path = CString(szPathName);
		return true;
	}

	return false;
}

void myDocment::writePolygon(Polyhedron* cutPieces, const char* path)
{
	std::vector<cVertex> * vertices = &cutPieces->vertices;
	std::map<cVertex*, int> hashIndex;
	for (int i = 0; i < vertices->size(); i++)
	{
		hashIndex.insert(std::pair<cVertex*, int>(&vertices->at(i), i));
	}

	std::ofstream myfile;
	myfile.open(path);
	// Write vertex
	myfile << vertices->size() << std::endl; // Number of vertices
	for (int i = 0; i < vertices->size(); i++)
	{
		myfile << vertices->at(i).v[0] << " "<< vertices->at(i).v[1] << " " << vertices->at(i).v[2] << std::endl;
	}

	// Write face
	myfile << cutPieces->faces.size() << std::endl;
	for (int i = 0; i < cutPieces->faces.size(); i++)
	{
		carve::poly::Face<3> &f = cutPieces->faces[i];
		myfile << f.nVertices() <<" ";
		for (int j = 0; j < f.nVertices(); j++)
		{
			int idx = hashIndex.find((cVertex*)f.vertex(j))->second;
			myfile << idx << " ";
		}
		myfile << std::endl;
	}
	myfile.close();
}

void myDocment::loadStateForFinalSwap()
{


	// Init final swap
	if (m_finalSwap)
	{
		delete m_finalSwap;
	}

	m_finalSwap = new detailSwapManager;

	m_finalSwap->s_obj = m_surfaceObj;
	m_finalSwap->s_skeleton = m_skeleton;
	m_finalSwap->s_octree = &m_highResVoxel->m_octree;
	m_finalSwap->s_hashTable = &m_highResVoxel->m_hashTable;
	m_finalSwap->s_boxes = &m_highResVoxel->m_boxes;
	m_finalSwap->s_boxShareFaceWithBox = &m_highResVoxel->m_boxShareFaceWithBox;
	m_finalSwap->voxelSize = m_highResVoxel->m_voxelSizef;

	m_finalSwap->initFromSaveFile();
}

void myDocment::loadStateForPostProcess()
{
	m_voxelProcess = manipulateVoxelPtr(new manipulateVoxel);

	m_voxelProcess->s_skeleton = m_skeleton;
	m_voxelProcess->s_voxelObj = m_highResVoxel;

	// We need mesh index, bone array and coord
	m_voxelProcess->loadFromFile();
	m_voxelProcess->cutCenterBoxByHalf(); // For better performance
	
	// Dialog
	if (m_boxPlaceMngr)
	{
		m_boxPlaceMngr->EndDialog(0);
	}
	m_boxPlaceMngr = movePlacedBoxDlgPtr(new movePlacedBoxDlg());
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CView * pView = pFrame->GetActiveView();
	m_boxPlaceMngr->Create(movePlacedBoxDlg::IDD, pView);
	m_boxPlaceMngr->initFromVoxelProcess(m_voxelProcess);
	m_boxPlaceMngr->ShowWindow(SW_SHOW);

	cout << "Mesh loaded" << endl
		<< " - Press 'S' to change to state cut mesh" << endl
		<< endl;
}

void myDocment::updateRealtime()
{
	if (m_curMode == MODE_SPLIT_BONE_GROUP)
	{
		m_groupCutMngr->updateRealTime();
	}
}

void myDocment::loadFile()
{
	// Init
	char* surfacePath = "../../Data/subMarine/subMarine.stl";
	cprintf("Init document\n");

	// 1. Surface
	cprintf("Load surface object: %s\n", surfacePath);
	CTimeTick tm; tm.SetStart();
	// Preprocess
 	holeMesh = processHoleMeshPtr(new processHoleMesh);
 	holeMesh->processMeshSTL(surfacePath);
 	m_surfaceObj = holeMesh->getBiggestWaterTightPart();
	if (!m_surfaceObj)
	{
		AfxMessageBox(_T("The input mesh is not water tight"));
		return;
	}

	m_surfaceObj->centerlize();
	m_surfaceObj->constructAABBTree();

	tm.SetEnd();
	cprintf("Load surface time: %f ms\n", tm.GetTick());

	// 2. Voxel object, high res and low res
	tm.SetStart();
	//Decide size
	float voxelSize = getVoxelSize(400); // Should be less than 500

	voxelObject * forSamplingVoxel = new voxelObject;
	forSamplingVoxel->initWithSize(m_surfaceObj, voxelSize / 3.0);

	m_highResVoxel = new voxelObject;
	m_highResVoxel->init(forSamplingVoxel, voxelSize);

	m_highResFullVoxel = new voxelObject;
	m_highResFullVoxel->initWithSize(m_surfaceObj, voxelSize);

	m_lowResVoxel = new voxelObject;
	m_lowResVoxel->init(forSamplingVoxel, voxelSize);

	tm.SetEnd();
	cout << "Voxel object constructed" << endl
		<< " - Voxel size: " << voxelSize << endl
		<< " - # of voxels: " << m_lowResVoxel->m_boxes.size() << endl;

	delete forSamplingVoxel;

	if (m_lowResVoxel->m_boxes.size() > 500)
	{
		AfxMessageBox(_T("Voxel object has more than 500 voxels. The program may over the memory"));
	}
	// We may construct low res voxel from high res voxel

	// 3. Skeleton
	m_skeleton = new skeleton;
	char* skeletonPath = "../../Data/skeleton.xml";
	m_skeleton->loadFromFile(skeletonPath);
	m_skeleton->computeTempVar();
	m_skeleton->groupBone();
	cprintf("Load skeleton: %s\n", skeletonPath);
	cprintf("Finish loading --------");

	// Message
	cout << endl << "-----------------------" << endl
		<< "Display options in view 1:" << endl
		<< " - 1: Surface mesh" << endl
		<< " - 2: Voxel object" << endl
		<< " - 3: Octree" << endl
		<< " - 4: BVH tree" << endl
		<< " - 5: Wire surface mesh" << endl
		<< " - 6: draw voxel object full" << endl
		<< " - 7: Draw components in mesh" << endl;
	cout << "Press 'S' to construct cut tree" << endl;

	view1->setDisplayOptions({ 0, 1, 1, 1 });
}


void myDocment::drawTest(BOOL mode[])
{

}

void myDocment::drawTest()
{
// 	vector<voxelBox> *boxes = &m_lowResVoxel->m_boxes;
// 	arrayVec3f colors = {Vec3f(0,0,0), Vec3f(0.5, 0.5, 0.5), Vec3f(1,1,1)};
// 
// 	for (auto b : *boxes)
// 	{
// 		glColor3fv(colors[0].data());
// 		b.draw(0);
// 		glColor3fv(colors[1].data());
// 		if (b.center[0] <0)
// 		{
// 			glColor3fv(colors[2].data());
// 		}
// 		b.draw(1);
// 	}
}

std::vector<arrayInt> testCut(voxelObject* obj, arrayInt idxs, int xcoord)
{
// 	std::vector<voxelBox> * boxes = &obj->m_boxes;
// 	std::vector<arrayInt> * boxShareFaceWithBox = &obj->m_boxShareFaceWithBox;
// 
// 	voxelBitConvex setLower = voxelBitConvex::makeCubeBitSet(v->leftDowni, ruLower);
// 		glColor3f(1, 1, 0);
// 	int* mark = new int[boxes->size()];
// 	std::fill(mark, mark + boxes->size(), 0);
// 	for (int i = 0; i < idxs.size(); i++)
// 	{
// 		mark[idxs[i]] = 1;
// 	}
// 
// 	int* hashvQ = new int[boxes->size()];
// 	std::fill(hashvQ, hashvQ + boxes->size(), 0);
// 
// 	std::vector<arrayInt> out;
// 	arrayInt remain = idxs;
// 	while (remain.size() > 0)
// 	{
// 		arrayInt newObj;
// 		std::queue<int> vQ;
// 		vQ.push(remain[0]);
// 
// 		bool isLowerCut = boxes->at(remain[0]).center[0] < xcoord;
// 
// 		while (!vQ.empty())
// 		{
// 			int idx = vQ.front();
// 			vQ.pop();
// 			newObj.push_back(idx);
// 			mark[idx] = 0; // No longer available
// 
// 			// add all its neighbor
// 			arrayInt neighborN = boxShareFaceWithBox->at(idx);
// 			for (int j = 0; j < neighborN.size(); j++)
// 			{
// 				bool curisLowerCut = boxes->at(neighborN[j]).center[0] < xcoord;
// 				if (mark[neighborN[j]] == 1
// 					&& hashvQ[neighborN[j]] == 0
// 					&& isLowerCut == curisLowerCut)
// 				{
// 					hashvQ[neighborN[j]] = 1;
// 					vQ.push(neighborN[j]);
// 				}
// 			}
// 		}
// 
// 		out.push_back(newObj);
// 		remain = Util_w::substractArrayInt(remain, newObj, boxes->size());
// 	}
// 
// 	return out;

	return std::vector<arrayInt>();
}

void myDocment::keyPressModeTest(char c)
{

}

void myDocment::saveCurrentBoxCut()
{
	// Save what swap box need for further debugging
	if (!m_swapMngr)
	{
		AfxMessageBox(_T("Swap box is not initialized"));
		return;
	}

	// what we need is the bounding box of cut mesh and the bone order
	vector<bvhVoxel*> meshBoxes = m_swapMngr->meshBox;
	char* filePath = "../../temporary/boxCutBoundingBox.txt";

	FILE* f = fopen(filePath, "w");
	ASSERT(f);
	fprintf(f, "%d\n", meshBoxes.size()); // Number of mesh box
	for (auto m : meshBoxes)
	{
		fprintf(f, "%s\n", CStringA(m->boneName).GetBuffer()); // Bone name
		fprintf(f, "%d\n", m->boneType); // Bone type
		fprintf(f, "%f %f %f\n", m->curLeftDown[0], m->curLeftDown[1], m->curLeftDown[2]); // Left down of bounding box
		fprintf(f, "%f %f %f\n", m->curRightUp[0], m->curRightUp[1], m->curRightUp[2]); // Right up[ of bounding box
	}
	fclose(f);

	cout << "Finish writing box cut: " << filePath << endl;;
}

void myDocment::loadSwapGroupFromFile()
{
	if (m_swapMngr)
	{
		delete m_swapMngr;
	}
	m_swapMngr = new detailSwapManager;

	m_skeleton->groupBone();

	m_swapMngr->s_obj = m_surfaceObj;
	m_swapMngr->s_skeleton = m_skeleton;
	m_swapMngr->s_octree = &m_lowResVoxel->m_octree;
	m_swapMngr->s_hashTable = &m_lowResVoxel->m_hashTable;
	m_swapMngr->s_boxes = &m_lowResVoxel->m_boxes;
	m_swapMngr->s_boxShareFaceWithBox = &m_lowResVoxel->m_boxShareFaceWithBox;
	m_swapMngr->voxelSize = m_lowResVoxel->m_voxelSizef;

	char* filePath = "../../temporary/boxCutBoundingBox.txt";
	m_swapMngr->initGroupVoxelFromSaveFile(filePath);

	cout << "Loaded mesh for swapping.\n"
		<< " - Press 'E' to reload" << endl
		<< " - Press 'F' to swap" << endl
		<< " - Press 'S' to cut grouped bones" << endl << endl;

	m_curMode = MODE_FINDING_CUT_SURFACE;
}

std::vector<arrayInt> myDocment::getVoxelIdxFullFromVoxelProcess()
{
	vector<arrayInt> originIdxs = m_voxelProcess->getListOfVoxelIdxs();
	vector<bone*> boneArray = m_voxelProcess->boneArray;

	// The full list contain more voxel
	vector<voxelBox> *originBox = &m_highResVoxel->m_boxes;
	vector<voxelBox> *fullBox = &m_highResFullVoxel->m_boxes;
	hashVoxel *hashTableFull = &m_highResFullVoxel->m_hashTable;

	vector<voxelList*> meshCut = m_voxelProcess->meshBox;

	vector<arrayInt> fullIdxs;
	arrayInt hashOccupy(fullBox->size(), 0);

	for (int i = 0; i < originIdxs.size(); i++)
	{
		arrayInt idxs = originIdxs[i];
		arrayInt newIdxs;
		for (auto vidx : idxs)
		{
			// Find it on the full voxel object
			Vec3f center = originBox->at(vidx).center;
			int idxInFull = hashTableFull->getBoxIndexFromCoord(center);
			ASSERT(idxInFull != -1);
			newIdxs.push_back(idxInFull);
			fullBox->at(idxInFull).boneIndex = i;
			hashOccupy[idxInFull] = 1;
		}

		fullIdxs.push_back(newIdxs);
	}

	// Other voxel
	// Just merge it to biggest bone
	// The voxel will be manipulated later
	arrayInt remainIdxs;
	for (int i = 0; i < fullBox->size(); i++)
	{
		if (hashOccupy[i] == 0) // Free voxel
		{
			Vec3f centerP = m_surfaceObj->midPoint();
			if (fullBox->at(i).center[0] < centerP[0])
			{
				remainIdxs.push_back(i);
			}
		}
	}

	while (remainIdxs.size() > 0)
	{
		arrayInt remainTemp;
		for (int ii = 0; ii < remainIdxs.size(); ii++)
		{
			int vIdx = remainIdxs[ii];
			// Find bone around
			int boneIdx = -1;
			int biggestBoneV = 0;
			arrayInt nbVoxel = m_highResFullVoxel->m_boxShareFaceWithBox[vIdx];
			for (auto nbIdx : nbVoxel)
			{
				int bIdx = fullBox->at(nbIdx).boneIndex;
				if (bIdx != -1)
				{
					arrayFloat err = meshCut[bIdx]->getErrorAssumeVoxelList(originIdxs[bIdx]);
					if (biggestBoneV < err[VOLUME_ERROR])
					{
						biggestBoneV = err[VOLUME_ERROR];
						boneIdx = bIdx;
					}
				}
			}

			if (boneIdx != -1)
			{
				fullIdxs[boneIdx].push_back(vIdx);
				fullBox->at(vIdx).state = boneIdx;
// 				remainIdxs.erase(remainIdxs.begin() + ii);
// 				break;
			}
			else
				remainTemp.push_back(vIdx);
		}

		remainIdxs = remainTemp;

		for (int i = 0; i < fullBox->size(); i++)
		{
			if (fullBox->at(i).state != -1)
			{
				fullBox->at(i).boneIndex = fullBox->at(i).state;
				fullBox->at(i).state = -1;
			}
		}
	}

	// Re edit center bone. THey should be symmetric
	for (int i = 0; i < boneArray.size(); i++)
	{
		if (boneArray[i]->m_type == CENTER_BONE)
		{
			arrayInt vidxs = fullIdxs[i];
			arrayInt hashV(fullBox->size(), 0);
			for (auto id : vidxs)
			{
				hashV[id] = 1;
			}

			for (int ii = 0; ii < vidxs.size(); ii++)
			{
				int id = vidxs[ii];
				int symIdx = hashTableFull->getSymmetricBox(id);
				if (symIdx != -1 && hashV[symIdx] == 0)
				{
					vidxs.push_back(symIdx);
					hashV[symIdx] = 1;
				}
			}

			fullIdxs[i] = vidxs;
		}
	}

	return fullIdxs;
}

void myDocment::saveCutMeshToObj()
{
	string path = "../../temporary/meshCut"; // if not exist, create
	if (!PathFileExists(CString(path.c_str())))
	{
		AfxMessageBox(_T("Folder does not exist"));
	}
	vector<Polyhedron*> cutSurfaces = m_meshCutting->m_cutSurface;
	for (int i = 0; i < cutSurfaces.size(); i++)
	{
		auto p = cutSurfaces[i];
		string cutmeshPath = path + "/meshCut_" + to_string(i) + ".obj";
		convertPolyHedronToMayaObj(p, cutmeshPath.c_str());
	}
}

void myDocment::convertPolyHedronToMayaObj(Polyhedron *cutPieces, const char* path) const
{
	std::vector<cVertex> * vertices = &cutPieces->vertices;
	std::map<cVertex*, int> hashIndex;
	for (int i = 0; i < vertices->size(); i++)
	{
		hashIndex.insert(std::pair<cVertex*, int>(&vertices->at(i), i));
	}

	std::ofstream myfile;
	myfile.open(path);
	// Write vertex
	for (int i = 0; i < vertices->size(); i++)
	{
		myfile << "v " << vertices->at(i).v[0] << " " << vertices->at(i).v[1] << " " << vertices->at(i).v[2] << "\n";
	}

	// Write face
	for (int i = 0; i < cutPieces->faces.size(); i++)
	{
		carve::poly::Face<3> f = cutPieces->faces[i];
		myfile << "f ";
		for (int j = 0; j < f.nVertices(); j++)
		{
			int idx = hashIndex.find((cVertex*)f.vertex(j))->second;
			myfile << idx + 1 << " ";
		}
		myfile << "\n";
	}
	myfile.close();
}

void myDocment::updateFilterCutGroup()
{
	std::vector<neighborPos> pp = cutFilterDialog->chosenPose;
	m_cutSurface.filterPose(pp);
}

float myDocment::getVoxelSize(int nbVoxel)
{
	int voxelTestRes = 6;
	voxelObject * testObj = new voxelObject;
	testObj->init(m_surfaceObj, voxelTestRes);

	AABBNode* root = m_surfaceObj->getBVH()->root();
	Vec3f bondingBoxSize = root->RightUp - root->LeftDown;

	float volumeObj = testObj->m_boxes.size() * std::pow(testObj->m_voxelSizef, 3);
	float voxelVol = volumeObj / nbVoxel;

	delete testObj;

	return std::pow(voxelVol, 1.0 / 3);
}

void myDocment::setDisplayOptions(std::initializer_list<int> opts)
{
// 	CKEGIESView* p = (CKEGIESView*)view1;
// 
// 	p->setDisplayOptions(opts);
}

Polyhedron* myDocment::getSymmetric_by_X(Polyhedron* cutPieces)
{
	std::vector<cVertex> * vertices = &cutPieces->vertices;
	std::vector<carve::poly::Face<3>> *faces = &cutPieces->faces;

	std::map<cVertex*, int> hashIndex;
	for (int i = 0; i < vertices->size(); i++)
	{
		hashIndex.insert(std::pair<cVertex*, int>(&vertices->at(i), i));
	}

	std::vector<int> fpoly;
	int nbFace = faces->size();
	for (auto f : *faces)
	{
		fpoly.push_back(f.nVertices());
		// Reverse normal
		for (int j = f.nVertices()-1; j >= 0; j--)
		{
			int idx = hashIndex.find((cVertex*)f.vertex(j))->second;
			fpoly.push_back(idx);
		}
	}

	std::vector<carve::geom3d::Vector> newVertices;
	for (auto v : *vertices)
	{
		newVertices.push_back(carve::geom::VECTOR(-v.v[0], v.v[1], v.v[2]));
	}

	return new Polyhedron(newVertices, nbFace, fpoly);
}
