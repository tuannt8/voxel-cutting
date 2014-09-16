#pragma once
#include "rapidxml.hpp"
#include <string>
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include <iostream>
#include "DataTypes/vec.h"

using namespace rapidxml;
typedef xml_node<> myXMLNode;

#define X_KEY "x"
#define Y_KEY "y"
#define Z_KEY "z"


class myXML : public xml_document < >
{
public:
	myXML();
	~myXML();

	// Add node to root doc
	xml_node<> *createNode(const char* element, const char* data = nullptr);
	xml_node<> *addNode(const char* element, const char* data = nullptr);

	// load and save
	bool save(const char* path);
	bool load(const char* path);

	// Modifying node
	myXMLNode * addNodeToNode(myXMLNode * parent, const char* element, const char* data = nullptr);
	// Write properties to node
	void addElementToNode(xml_node* node, const char* element, const char* data = nullptr);
	void addElementToNode(xml_node* node, const char* element, std::string data);

	void addVectorDatafToNode(myXMLNode * node, const char * element, const float *data);
	void addVectorDatafToNode(myXMLNode * node, const char * element, float x, float y, float z);
	void addVectorDatafToNode(myXMLNode * node, const char * element, Vec3f v);
	void addVectoriToNode(myXMLNode * node, const char * element, Vec3i v);

	void addCommentToNode(myXMLNode* node, const char* comment);

	// Get value
	std::string getStringProperty(myXMLNode * node, const char* element);
	Vec3f getVec3f(myXMLNode * xmlNode, char* element);
	std::string getStringProperty(const char* element);
	Vec3i getVec3i(myXMLNode * node, char * element);
private:
	std::vector<char*> m_backUpData;
	// To keep the memory of string alive
	std::ifstream m_file;
	std::vector<char> m_buffer;
};

