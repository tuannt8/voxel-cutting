#include "stdafx.h"
#include "myDocment.h"
#include <shobjidl.h>
#include "write_ply.hpp"
#include <iostream>
#include <afxwin.h>
#include "log.h"
#include "MainFrm.h"

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
	shift = 0;
	m_debug = debugInfoPtr(new debugInfo);

	loadFile();
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
}

void myDocment::draw(BOOL mode[10])
{
	static arrayVec3f color = Util_w::randColor(10);

	if (m_curMode == MODE_TEST)
	{
		drawTest(mode);
		return;
	}

	if (mode[1])
	{
		glColor3f(0.8, 0.8, 0.8);
		m_surfaceObj->drawObject();
	}

	if (!mode[2])
	{
		glColor3f(0, 0, 0);
		m_lowResVoxel->drawVoxelLeaf();
		glColor3f(0.9, 0.9, 0.9);
		m_lowResVoxel->drawVoxelLeaf(1);
	}

	if (!mode[3])
	{
		glColor3f(0, 0, 0);
		m_highResFullVoxel->drawVoxelLeaf();
		glColor3f(0.9, 0.9, 0.9);
		m_highResFullVoxel->drawVoxelLeaf(1);
	}

	if (m_curMode == MODE_NONE)
	{
		if (mode[4])
		{
			glColor3f(1, 0, 0);
			m_lowResVoxel->drawVoxel();
		}
		if (mode[5])
		{
			glColor3f(0, 0, 0);
			m_highResVoxel->drawVoxelLeaf();
			glColor3f(0.7, 0.7, 0.7);
			m_highResVoxel->drawVoxelLeaf(1);
		}
		if (mode[6] && holeMesh)
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
				m_swapMngr->draw(mode);
			}
		}

	}
	else if (m_curMode == MODE_SPLIT_BONE_GROUP)
	{
		if (mode[4])
		{
			m_swapMngr->draw(mode);
		}
		else
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
		if (m_finalSwap)
		{
			m_finalSwap->draw(mode);
		}
		if (m_voxelProcess)
		{
			m_voxelProcess->draw(mode);
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

	if (mode[1])
	{
		glLineWidth(2.0);
		glColor3f(0, 0, 1);
		m_skeleton->draw(SKE_DRAW_BOX_WIRE);
		glLineWidth(1.0);
	}

// 	glColor3f(0, 0, 1);
// 	m_highResVoxel->drawVoxel(1);
// 	m_debug->s_voxelObj = m_highResVoxel;
// 	glColor3f(0, 0, 1);
// 	m_debug->drawtestVoxelSym();

	if (m_finalSwap)
	{
// 		static arrayVec3f color = Util_w::randColor(10);
// 		std::vector<bvhVoxel*> mb = m_finalSwap->meshBox;
// 		for (int i = 0; i < mb.size(); i++)
// 		{
// 			if (mode[i])
// 			{
// 				glColor3fv(color[i+1].data());
// 				mb[i]->drawVoxels(nullptr, 0);
// 				glColor3fv(color[i].data());
// 				mb[i]->drawVoxels(nullptr, 1);
// 			}
// 		}
	}

}

void myDocment::initState()
{
	if (m_curMode == MODE_FINDING_CUT_SURFACE)
	{

	}
}

void myDocment::receiveKey(char c)
{
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
	}

	if (m_curMode == MODE_FINDING_CUT_SURFACE)
	{
		if ((int)c == VK_LEFT || (int)c == VK_LEFT || c == 'B') // Best configuration
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
	command::print("Construct the tree");
	CTimeTick time;
	time.SetStart();

	m_cutSurface.s_groupSkeleton = m_skeleton;
	m_cutSurface.s_voxelObj = m_lowResVoxel;
	m_cutSurface.bUniformCut = false;
	m_cutSurface.init();
	
	time.SetEnd();
	cout << "Construction time: " << time.GetTick() << " ms" << endl;
	cout << "Number of configurations: " << m_cutSurface.m_tree2.leaves.size() << " (" << m_cutSurface.m_tree2.poseMngr->poseMap.size() << " groups)" << endl;
	cout << "Now choose the configuration\n"
		<< " - 'LEFT' and 'RIGHT' to change pose index\n"
		<< " - 'UP' and 'DOWN' to change configuration index\n"
		<< " - 'B' for best option in pose group\n"
		<< " - 'D' to voxelize and 'G' to swap\n"
		<< " - 'S' to change to state splitting group bones\n"
		<< endl << endl;

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

	cout << " - 'R' to save current file" << endl;

//	m_swapMngr->swapVoxel2();
}

void myDocment::changeToCutGroupBone()
{
	if (!m_swapMngr)
	{
		AfxMessageBox(_T("Box cut is not ready"));
		return;
	}

	m_curMode = MODE_SPLIT_BONE_GROUP;
	
	if (m_groupCutMngr)
	{
		delete m_groupCutMngr;
	}

	cout << "Split group bones" << endl;

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
		<< "Press 'S' to ready to assign coordinate to bones"
		<< endl << endl;
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
	ASSERT(m_groupCutMngr->m_idxChoosen.size() > 0);

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

	cout << endl << "Use dialog to assign coordinate to bones" << endl
		<< endl;
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
	m_curMode = MODE_FIT_BONE;

	writeMeshBoxStateFinalSwap();

	loadStateForPostProcess();

	// Guide
	cout << "In put the percentage in first tool bar box and update\n"
		<< " - Press F to resolve the conflict\n"
		<< " - Press R to save to mesh\n"
		<< " - Press W to reload the mesh\n"
		<< " - Press 'S' to cut the triangular mesh\n"
		<< endl;
		
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
	cout << "\n Change to cutting mesh state" << endl
		<< "Change state time: " << time.GetTick() << " ms" << endl
		<< "Change to cutting triangular mesh \n"
		<< " - Press 'F' to cut the original mesh and 'D' to save to file \n" << endl
		<< " - Press 'E' to export cut mesh to obj file" << endl
		<< endl;
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
	}

	arrayBone_p boneArray = m_meshCutting->boneArray;

	// Write the cutting information
	myXML infoFile;

	// Write the original mesh
	CStringA originalMesh(path);
	CStringA originalMeshName("originalMesh.obj");
	originalMesh += ("\\");
	originalMesh += originalMeshName;
	infoFile.addNode(XML_ORIGINAL_MESH_KEY, originalMeshName.GetBuffer());
	m_surfaceObj->writeObjMayaData(originalMesh.GetBuffer());

	// Write the skeleton information
	CStringA skeletonPath(path);
	CStringA skeletonPathName("skeleton.xml");
	skeletonPath += "\\";
	skeletonPath += skeletonPathName;
	infoFile.addNode(XML_SKELETON_MESH_KEY, skeletonPathName.GetBuffer());
	m_skeleton->writeToFile(skeletonPath.GetBuffer());

	// Write the cut mesh in global coord
	std::vector<Polyhedron*> cutPieces = m_meshCutting->m_cutPieces;
	arrayVec3i coord = m_meshCutting->coords;
	for (int i = 0; i < cutPieces.size(); i++)
	{
		CStringA meshPath(path);
		CStringA meshPathName; meshPathName.Format("meshPath_%d.obj", i);
		meshPath.AppendFormat("\\%s", meshPathName.GetBuffer());

		myXMLNode * meshPartNode = infoFile.addNode(XML_MESH_PART);
		infoFile.addElementToNode(meshPartNode, XML_CUT_MESH_NAME, std::string(meshPathName));
		infoFile.addElementToNode(meshPartNode, XML_BONE_NAME, std::string(CStringA(boneArray[i]->m_name)));
		infoFile.addVectoriToNode(meshPartNode, XML_COORD_MAP, coord[i]);

		convertPolyHedronToMayaObj(cutPieces[i], meshPath.GetBuffer());
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
	char* surfacePath = "../../Data/jetSki/jetSki.stl";
	cprintf("Init document\n");

	// 1. Surface
	cprintf("Load surface object: %s\n", surfacePath);
	CTimeTick tm; tm.SetStart();
	// Pre process
	holeMesh = processHoleMeshPtr(new processHoleMesh);
	holeMesh->processMeshSTL(surfacePath);
	m_surfaceObj = holeMesh->getBiggestWaterTightPart();
	if (!m_surfaceObj)
	{
		AfxMessageBox(_T("The input mesh is not water tight"));
		return;
	}
// 	m_surfaceObj = new SurfaceObj;
// 	m_surfaceObj->readObjDataSTL(surfacePath);
	m_surfaceObj->centerlize();
	m_surfaceObj->constructAABBTree();

	tm.SetEnd();
	cprintf("Load surface time: %f ms\n", tm.GetTick());

	// 2. Voxel object, high res and low res
	tm.SetStart();

	int voxelRes = 6;
	int highRes = 6;
	int downSampleRate = 12;
	voxelObject * forSamplingVoxel = new voxelObject;
	forSamplingVoxel->init(m_surfaceObj, downSampleRate); // TODO: The box should be symmetric !!!!

	m_highResVoxel = new voxelObject;
	m_highResVoxel->init(forSamplingVoxel, highRes);

	m_highResFullVoxel = new voxelObject;
	m_highResFullVoxel->init(m_surfaceObj, highRes);

	m_lowResVoxel = new voxelObject;
	m_lowResVoxel->init(forSamplingVoxel, voxelRes);

	tm.SetEnd();
	cprintf("Constuct voxel; time: %f ms\n", tm.GetTick());
	cout << "Low res: " << voxelRes << "; high res: " << highRes << endl;

	delete forSamplingVoxel;
	// We may construct low res voxel from high res voxel

	// 3. Skeleton
	m_skeleton = new skeleton;
	char* skeletonPath = "../../Data/skeleton.xml";
	m_skeleton->loadFromFile(skeletonPath);
	m_skeleton->computeTempVar();
	m_skeleton->groupBone();
	cprintf("Load skeleton: %s\n", skeletonPath);
	cprintf("Finish loading --------");
}

void myDocment::loadTestVoxelBitSet()
{
	// Init
	char* surfacePath = "../../Data/UShape.stl";
	std::cout<<"Init test voxel\n";

	// 1. Surface
	std::cout << "Load surface object: " << surfacePath <<std::endl;
	CTimeTick tm; tm.SetStart();

	m_surfaceObj = new SurfaceObj;
	m_surfaceObj->readObjDataSTL(surfacePath);
	m_surfaceObj->centerlize();
	m_surfaceObj->constructAABBTree();

	tm.SetEnd();
	cprintf("Load surface time: %f ms\n", tm.GetTick());

	// 2. Voxel object, high res
	std::cout << "Start construct voxel object\n";
	tm.SetStart();

	int highRes = 5;
	m_highResVoxel = new voxelObject;
	m_highResVoxel->init(m_surfaceObj, highRes); // TODO: The box should be symmetric !!!!

	tm.SetEnd();
	cprintf("Constuct voxel high res: %d; time: %f ms\n", highRes, tm.GetTick());

	std::cout << breakLine;
}

void myDocment::drawTest(BOOL mode[])
{
	static arrayVec3f colors = Util_w::randColor(20);
	if (!mode[1])
	{
		m_highResVoxel->drawVoxelBitDecomposed();
		glColor3f(1, 0, 0);
		m_highResVoxel->m_voxelBitSet.drawBoundingBox();
	}
	if (!mode[2])
	{
		glColor3f(0.7, 0.7, 0.7);
		m_highResVoxel->m_octree.drawWireOctree();
	}
	if (!mode[3])
	{
		glColor3f(1, 0, 0);
		m_highResVoxel->drawBitSetBoundingBox();
	}
	if (!mode[4])
	{
		glColor3f(0, 0, 0);
		m_highResVoxel->drawVoxelLeaf();
		glColor3f(0.7, 0.7, 0.7);
		m_highResVoxel->drawVoxelLeaf(1);
	}

	if (mode[5])
	{
		voxelSplitObj * v = &m_highResVoxel->m_voxelBitSet;
		std::vector<voxelSplitObj> a = v->cutByOthogonalFace(0, shift);

		
		for (int i = 0; i < a.size(); i++)
		{
			if (!mode[i])
			{
				continue;
			}
			glColor3fv(colors[i+1].data());
			a[i].drawVoxelBoxWire();

			glColor3fv(colors[i].data());
			a[i].drawVoxelBox();

			glColor3f(1, 0, 0);
			a[i].drawBoundingBox();
		}
	}
	if (mode[6])
	{
// 		voxelSplitObj * v = &m_highResVoxel->m_voxelBitSet;
// 		Vec3i ruLower = v->rightUpi;
// 		ruLower[0] = shift;
// 		setLower.drawBoxSolid(&m_highResVoxel->m_hashTable);
	}
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
	if (c == 'S')
	{
		shift++;
	}
	if (c=='A')
	{
		shift--;
	}

	if (c == 'T') // Test
	{
		std::vector<voxelBox> * boxes = &m_highResVoxel->m_boxes;
		std::cout << "Voxel resolution: " << m_highResVoxel->m_boxes.size() << "\n";
		arrayInt idxs;
		for (int i = 0; i < boxes->size(); i++)
		{
			idxs.push_back(i);
		}

		for (int N = 1; N < 1000; N+=200)
		{
			CTimeTick time;
			time.SetStart();

			for (int i = 0; i < N; i++)
			{
				testCut(m_highResVoxel, idxs, 0);
			}
			time.SetEnd();
			std::cout << "Breadth first search (" << N << " times): " << time.GetTick() << "\n";
		}
	}

	if (c == 'Y')
	{
		voxelSplitObj * v = &m_highResVoxel->m_voxelBitSet;
		std::cout << "Voxel resolution: " << m_highResVoxel->m_boxes.size() << "\n";

		for (int N = 1; N < 1000; N += 200)
		{

			CTimeTick time;
			time.SetStart();

			for (int i = 0; i < N; i++)
			{
				std::vector<voxelSplitObj> a = v->cutByOthogonalFace(0, 5);
			}
			time.SetEnd();
			std::cout << "Bit set (" << N << " times): " << time.GetTick() << "\n";
		}
	}
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
				if (hashV[symIdx] == 0)
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
		carve::poly::Face<3> &f = cutPieces->faces[i];
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
