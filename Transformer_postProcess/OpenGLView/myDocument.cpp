#include "stdafx.h"
#include "myDocument.h"
#include "Utility_wrap.h"

using namespace std;

myDocument::myDocument()
{
}


myDocument::~myDocument()
{

}

void myDocument::init()
{
	folderPath = "../../Data/Barrel";
	loadDataWithObjMesh(folderPath);
}

void myDocument::loadData(const std::string folderPath)
{
	// info file
	string infoFilePath = folderPath + "/info.xml";
	myXML infoFile;
	infoFile.load(infoFilePath.c_str());

	// Load original mesh
	string meshPath = folderPath + "/" + infoFile.getStringProperty(XML_ORIGINAL_MESH_KEY);
	m_surObj = SurfaceObjPtr(new SurfaceObj);
	m_surObj->readObjData(meshPath.c_str());

	// Load skeleton
	string skeletonPath = folderPath + "/" + infoFile.getStringProperty(XML_SKELETON_MESH_KEY);
	m_skeleton = skeletonPtr(new skeleton);
	m_skeleton->loadFromFile(skeletonPath.c_str());
	m_originalSkeleton = skeletonPtr(new skeleton);
	m_originalSkeleton->loadFromFile(skeletonPath.c_str());
	temp_skeletonPart = skeletonPath;

	// Load cut mesh
	m_meshMnger = meshMngerPtr(new meshMnger);
	m_meshMnger->surObj = m_surObj;
	m_meshMnger->skeleton = m_skeleton;
	arrayBone_p boneArray;
	m_skeleton->getSortedBoneArray(boneArray);
	for (myXMLNode * node = infoFile.first_node(XML_MESH_PART); node; node = node->next_sibling())
	{
		string cutPartPath = folderPath + "/" + infoFile.getStringProperty(node, XML_CUT_MESH_NAME);
		PolyhedronPtr newPiece = PolyhedronPtr(loadPolyhedron(cutPartPath));
		string boneName = infoFile.getStringProperty(node, XML_BONE_NAME);
		Vec3i coordMap = infoFile.getVec3i(node, XML_COORD_MAP);

		meshObjectPtr newMeshObj = meshObjectPtr(new meshObject);
		newMeshObj->m_polyMesh = newPiece;
		newMeshObj->coord = coordMap;
		// Find the bone
		for (int i = 0; i < boneArray.size(); i++)
		{
			if (boneArray[i]->m_name.Compare(CString(boneName.c_str())) == 0)
			{
				newMeshObj->s_bone = boneArray[i];
				break;
			}
		}

		newMeshObj->initOther();
		m_meshMnger->m_meshArray.push_back(newMeshObj);
	}

	// Transform the mesh
	m_meshMnger->transformMesh();
}

void myDocument::draw1(BOOL mode[10])
{
	static arrayVec3f color = Util_w::randColor(10);

	if (!mode[1] && m_surObj)
	{
		glColor3f(1, 0, 0);
		m_surObj->drawObject();
	}

	if (mode[2])
	{
		m_meshMnger->drawMeshFace();
	}
}

void myDocument::draw2(bool mode[10])
{
	if (!m_skeleton)
	{
		return;
	}
	if (mode[1])
	{
		m_skeleton->draw(SKE_DRAW_BOX_WIRE);

	}

	if (mode[2])
	{
		m_skeleton->draw(SKE_DRAW_MESH);
	}

	if (mode[3])
	{
		m_originalSkeleton->draw(SKE_DRAW_BOX_WIRE);
	}
}

Polyhedron * myDocument::loadPolyhedron(std::string cutPartPath)
{
	std::vector<carve::geom3d::Vector> vertices;
	arrayInt faces;

	FILE* f = fopen(cutPartPath.c_str(), "r");
	ASSERT(f);
	int nbPoint;
	fscanf(f, "%d\n", &nbPoint);
	vertices.resize(nbPoint);
	for (int i = 0; i < nbPoint; i++)
	{
		float x, y, z;
		fscanf(f, "%f %f %f\n", &x, &y, &z);
		vertices[i] = carve::geom::VECTOR(x, y, z);
	}

	int nbFace;
	fscanf(f, "%d\n", &nbFace);
	for (int i = 0; i < nbFace; i++)
	{
		int nbPt;
		fscanf(f, "%d\n", &nbPt);
		faces.push_back(nbPt);
		for (size_t j = 0; j < nbPt; j++)
		{
			int idx;
			fscanf(f, "%d\n", &idx);
			faces.push_back(idx);
		}
	}
	fclose(f);

	return new carve::poly::Polyhedron(vertices, nbFace, faces);
}

void myDocument::drawCoord(Vec3i coords)
{
	float length = 2;
	static arrayVec3f xyz = { Vec3f(1, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 1, 0)};
	glPushAttrib(GL_COLOR);

	glBegin(GL_LINES);
	for (int i = 0; i < 3; i++)
	{
		glColor3fv(xyz[i].data());
		glVertex3f(0, 0, 0);
		glVertex3fv((xyz[coords[i]] * length).data());
	}
	glEnd();

	glPopAttrib();
}

void myDocument::receiveKey(char k)
{
	if (k == 'S')
	{
		m_meshMnger->showModifyingDlg();
	}
	if (k == 'D')
	{
		saveNewBone();
	}
	if (k == 'M')
	{
		saveForShumin();
	}
}

void myDocument::updateParam()
{
	if (m_meshMnger)
		m_meshMnger->updateBone();
}

void myDocument::update(int yIdx, int zIdx)
{
	arrayBone_p bones;
	m_skeleton->getSortedBoneArray(bones);

	arrayBone_p bones_origin;
	m_originalSkeleton->getSortedBoneArray(bones_origin);

	for (int i = 0; i < bones.size(); i++)
	{
		bones[i]->scale(bones_origin[i], (float)yIdx / 100);
	}
}

void myDocument::saveNewBone()
{
	// back up old file
	std::string backUpPath;
	int idx = 0;
	while (true)
	{
		backUpPath = folderPath + "/Skeleton_origin_" + std::to_string(idx) + ".xml";
		FILE*fp = NULL;
		errno_t en = fopen_s(&fp, backUpPath.c_str(), "rb");
		if (!en)
		{
			// file exists
			fclose(fp);
			idx++;
			continue;
		}

		// Not exist, then copy
		CopyFile(CString(temp_skeletonPart.c_str()), CString(backUpPath.c_str()), FALSE);
		break;
	}
	m_skeleton->writeToFileWithModification(temp_skeletonPart.c_str());
}

void myDocument::convertPolyHedronToMayaObj(Polyhedron *cutPieces, const char* path)
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

void myDocument::writeAllFileToMayaFormat()
{
	std::vector<meshObjectPtr> meshes = m_meshMnger->m_meshArray;
	for (int i = 0; i < meshes.size(); i++)
	{
		std::string filePath = folderPath + "/mayaObj_" + to_string(i) + ".obj";
		convertPolyHedronToMayaObj(meshes[i]->m_polyMesh.get(), filePath.c_str());

		if (meshes[i]->s_bone->m_type == SIDE_BONE)
		{
			// Write mirror
		}
	}
}

void myDocument::loadDataWithObjMesh(const std::string folderPath)
{
	// info file
	string infoFilePath = folderPath + "/info.xml";
	myXML infoFile;
	infoFile.load(infoFilePath.c_str());

	// Load original mesh
	string meshPath = folderPath + "/" + infoFile.getStringProperty(XML_ORIGINAL_MESH_KEY);
	m_surObj = SurfaceObjPtr(new SurfaceObj);
	m_surObj->readObjDataSTL(meshPath.c_str());

	// Load skeleton
	string skeletonPath = folderPath + "/" + infoFile.getStringProperty(XML_SKELETON_MESH_KEY);
	m_skeleton = skeletonPtr(new skeleton);
	m_skeleton->loadFromFile(skeletonPath.c_str());
	m_originalSkeleton = skeletonPtr(new skeleton);
	m_originalSkeleton->loadFromFile(skeletonPath.c_str());
	temp_skeletonPart = skeletonPath;

	// Load the mesh
	m_meshMnger = meshMngerPtr(new meshMnger);
	m_meshMnger->surObj = m_surObj;
	m_meshMnger->skeleton = m_skeleton;

	arrayBone_p boneArray;
	m_skeleton->getSortedBoneArray(boneArray);

	for (myXMLNode * node = infoFile.first_node(XML_MESH_PART); node; node = node->next_sibling())
	{
		// Load the mesh
		string cutPartPath = folderPath + "/" + infoFile.getStringProperty(node, XML_CUT_MESH_NAME);
		
		PolyhedronPtr newPolyMesh = m_meshMnger->loadPolyFromObj(cutPartPath.c_str());;


		meshObjectPtr newMeshObj = meshObjectPtr(new meshObject);
		newMeshObj->m_polyMesh = newPolyMesh;

		// Find the bone
		string boneName = infoFile.getStringProperty(node, XML_BONE_NAME);
		for (int i = 0; i < boneArray.size(); i++)
		{
			if (boneArray[i]->m_name.Compare(CString(boneName.c_str())) == 0)
			{
				newMeshObj->s_bone = boneArray[i];
				break;
			}
		}

		// Load the coord
		myXMLNode *coordNode = node->first_node(XML_LOCAL_COORD);
		coordInfo newCoord;
		newCoord.origin = infoFile.getVec3f(coordNode, XML_COORD_ORIGIN);
		newCoord.xyzOrient[0] = infoFile.getVec3f(coordNode, XML_COORD_X);
		newCoord.xyzOrient[1] = infoFile.getVec3f(coordNode, XML_COORD_Y);
		newCoord.xyzOrient[2] = infoFile.getVec3f(coordNode, XML_COORD_Z);

		newMeshObj->coordMap = newCoord;

		newMeshObj->initOther();

		m_meshMnger->m_meshArray.push_back(newMeshObj);
	}

	// Transform mesh to bone
	// Transform the mesh
	m_meshMnger->transformMesh2();
}

void myDocument::saveForShumin()
{
	// We update the info file and save mesh in local coordinate
	// 1. Save the new info file
	using namespace std;
	string newFilePath = folderPath + "/newInfo.xml";
	string oldFilePath = folderPath + "/info.xml";

	myXML * doc = new myXML;
	myXML *docOld = new myXML;
	docOld->load(oldFilePath.c_str());

	// Mesh name
	string meshName = docOld->getStringProperty(XML_ORIGINAL_MESH_KEY);
	doc->addElementToNode(doc, XML_ORIGINAL_MESH_KEY, meshName);

	// Skeleton name
	string skeletonFille = docOld->getStringProperty(XML_SKELETON_MESH_KEY);
	doc->addElementToNode(doc, XML_SKELETON_MESH_KEY, skeletonFille);

	// Bone map
	std::vector<meshObjectPtr> meshArray = m_meshMnger->m_meshArray;
	for (int i = 0; i < meshArray.size(); i++)
	{
		myXMLNode * curNode = doc->addNode(XML_MESH_PART);
		meshObjectPtr curMesh = meshArray[i];
		
		// Write mesh
		string meshFile = "mesh_local_" + to_string(i) + ".obj";
		string filePath = folderPath + "/" + meshFile;

		doc->addElementToNode(curNode, XML_CUT_MESH_NAME, meshFile);
		convertPolyHedronToMayaObj(curMesh->s_bone->mesh.get(), filePath.c_str());

		// Bone name
		doc->addElementToNode(curNode, XML_BONE_NAME, string(CStringA(curMesh->s_bone->m_name).GetBuffer()));

		// Bone coord
		coordInfo curCoord = curMesh->coordMap;
		myXMLNode * coord = doc->addNodeToNode(curNode, XML_LOCAL_COORD);
		doc->addVectorDatafToNode(coord, XML_COORD_ORIGIN, curCoord.origin);
		doc->addVectorDatafToNode(coord, XML_COORD_X, curCoord.xyzOrient[0]);
		doc->addVectorDatafToNode(coord, XML_COORD_Y, curCoord.xyzOrient[1]);
		doc->addVectorDatafToNode(coord, XML_COORD_Z, curCoord.xyzOrient[2]);
	}

	string docPath = folderPath + "/info_new.xml";
	doc->save(docPath.c_str());
}
