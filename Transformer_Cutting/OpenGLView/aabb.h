#pragma once

class AABBNode
{
public:
	AABBNode* parent;
	AABBNode* left;
	AABBNode* right;

	vec3f min;
	vec3f max;
public:
	AABBNode()
	{
		parent = NULL;
		left = NULL;
		right = NULL;
	}
	AABBNode(AABBNode* parent, vec3f min, vec3f max)
	{
		this->parent = parent;
		this->min = min;
		this->max = max;
		left = NULL;
		right = NULL;
	}
	~AABBNode();

	void drawBound();
};

class AABBTree
{
public:
	AABBNode* root;

	AABBTree();
	~AABBTree();

	void render();
	void construct(AABBNode* parent, vec3f min, vec3f max);
	void drawBound(AABBNode* parent);
};

