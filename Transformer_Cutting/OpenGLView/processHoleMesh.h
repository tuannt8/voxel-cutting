#pragma once
#include "Graphics\Surfaceobj.h"
class processHoleMesh
{
public:
	processHoleMesh();
	~processHoleMesh();

	void processMeshSTL(char* path);
	SurfaceObj * getBiggestWaterTightPart();
	void drawSeparatePart() const;
private:
	SurfaceObjPtr originalSurface;

	std::vector<arrayInt> independentObj;
	std::vector<bool> isWaterTightArray;
};

typedef std::shared_ptr<processHoleMesh> processHoleMeshPtr;