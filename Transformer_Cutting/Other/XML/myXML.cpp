#include "stdafx.h"
#include "myXML.h"
#include <afx.h>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
#include "Utility.h"


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
	std::stringstream buffer;
	buffer << x << " , " << y << " , " << z;
	addElementToNode(node, element, buffer.str());
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
	char* coord = child->value();

	// parse the value
	using namespace std;
	vector<string> out;
	Util::split(string(coord), ',', out);

	pt[0] = atof(out[0].c_str());
	pt[1] = atof(out[1].c_str());
	pt[2] = atof(out[2].c_str());

	return pt;
}

void myXML::addCommentToNode(myXMLNode* node, const char* comment)
{
	myXMLNode * cm = allocate_node(node_comment, 0, comment);
	node->append_node(cm);
}

void myXML::addVectoriToNode(myXMLNode * node, const char * element, Vec3i v)
{
	std::stringstream str;
	str << v[0] << ", " << v[1] << ", " << v[2];
	addElementToNode(node, element, str.str());
}

Vec3i myXML::getVec3i(myXMLNode * node, char * element)
{
	ASSERT(node);
	Vec3i pt;
	myXMLNode * child = node->first_node(element);
	char* coord = child->value();

	// parse the value
	using namespace std;
	vector<string> out;
	Util::split(string(coord), ',', out);

	pt[0] = atoi(out[0].c_str());
	pt[1] = atoi(out[1].c_str());
	pt[2] = atoi(out[2].c_str());

	return pt;
}
