#include "stdafx.h"
#include "myXML.h"
#include <afx.h>


using namespace std;

myXML::myXML()
{
}


myXML::~myXML()
{
	for (int i = 0; i < m_backUpData.size(); i++)
	{
		free(m_backUpData[i]);
	}
}

void myXML::addElementToNode(xml_node* node, const char* element, const char* data)
{
	myXMLNode *newNode = createNode(element, data);
	node->append_node(newNode);
}

void myXML::addElementToNode(xml_node* node, const char* element, std::string data)
{
	char *cstr = new char[data.length() + 1];
	strcpy(cstr, data.c_str());
	m_backUpData.push_back(cstr);
	addElementToNode(node, element, cstr);
}

xml_node<> * myXML::createNode(const char* element, const char* data /*= nullptr*/)
{
	if (data)
		return allocate_node(node_element, element, data);
	else
		return allocate_node(node_element, element, " ");;
}

xml_node<> * myXML::addNode(const char* element, const char* data /*= nullptr*/)
{
	myXMLNode* node = createNode(element, data);
	append_node(node);
	return node;
}

myXMLNode * myXML::addNodeToNode(myXMLNode * parent, const char* element, const char* data /*= nullptr*/)
{
	myXMLNode* node = createNode(element, data);
	parent->append_node(node);
	return node;
}

void myXML::addVectorDatafToNode(myXMLNode * node, const char * element, const float *data)
{
	addVectorDatafToNode(node, element, data[0], data[1], data[2]);
}

void myXML::addVectorDatafToNode(myXMLNode * node, const char * element, float x, float y, float z)
{
	myXMLNode * dataNode = addNodeToNode(node, element);
	addElementToNode(dataNode, X_KEY, std::to_string(x));
	addElementToNode(dataNode, Y_KEY, std::to_string(y));
	addElementToNode(dataNode, Z_KEY, std::to_string(z));
}

void myXML::addVectorDatafToNode(myXMLNode * node, const char * element, Vec3f v)
{
	addVectorDatafToNode(node, element, v[0], v[1], v[2]);
}

bool myXML::save(const char* path)
{
	std::string s;
	print(std::back_inserter(s), *this, 0);

	FILE* f;
	fopen_s(&f, path, ("w"));
	if (!f)
	{
		return false;
	}

	fprintf(f, "%s", s.c_str());
	fclose(f);

	return true;
}

bool myXML::load(const char* path)
{
	try
	{
		// Read the xml file into a vector
		m_file = ifstream(path);
		m_buffer = vector<char>((istreambuf_iterator<char>(m_file)), istreambuf_iterator<char>());
		m_buffer.push_back('\0');

		// Parse the buffer using the xml file parsing library into doc 
		parse<0>(&m_buffer[0]);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

std::string myXML::getStringProperty(myXMLNode * node, const char* element)
{
	if (node)
	{
		myXMLNode * child = node->first_node(element);
		ASSERT(child);

		std::string s = child->value();
		return s;
	}
}

std::string myXML::getStringProperty(const char* element)
{
	myXMLNode * child = first_node(element);
	ASSERT(child);

	std::string s = child->value();
	return s;
}

Vec3f myXML::getVec3f(myXMLNode * xmlNode, char* element)
{
	ASSERT(xmlNode);
	Vec3f pt;
	myXMLNode * child = xmlNode->first_node(element);
	char* xc = child->first_node(X_KEY)->value();
	char* yc = child->first_node(Y_KEY)->value();
	char* zc = child->first_node(Z_KEY)->value();

	pt[0] = atof(xc);
	pt[1] = atof(yc);
	pt[2] = atof(zc);

	return pt;
}

void myXML::addCommentToNode(myXMLNode* node, const char* comment)
{
	myXMLNode * cm = allocate_node(node_comment, 0, comment);
	node->append_node(cm);
}

void myXML::addVectoriToNode(myXMLNode * node, const char * element, Vec3i v)
{
	myXMLNode * dataNode = addNodeToNode(node, element);
	addElementToNode(dataNode, X_KEY, std::to_string(v[0]));
	addElementToNode(dataNode, Y_KEY, std::to_string(v[1]));
	addElementToNode(dataNode, Z_KEY, std::to_string(v[2]));
}

Vec3i myXML::getVec3i(myXMLNode * node, char * element)
{
	myXMLNode * child = node->first_node(element);
	Vec3i pt;
	char* xc = child->first_node(X_KEY)->value();
	char* yc = child->first_node(Y_KEY)->value();
	char* zc = child->first_node(Z_KEY)->value();

	pt[0] = atoi(xc);
	pt[1] = atoi(yc);
	pt[2] = atoi(zc);

	return pt;
}
