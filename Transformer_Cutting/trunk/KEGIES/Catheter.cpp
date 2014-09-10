#include "StdAfx.h"
#include "Catheter.h"
#include "PrismaticLattice.h"

Catheter::Catheter(void)
{
	intro = "Catheter modeling";
	offset = 0;

	endoscope.ReadObjData("../Graphics_resource/endoScope.obj");
	arrow.ReadObjData("../Graphics_resource/arrow.obj");
}


Catheter::~Catheter(void)
{
	XYZ.clear();
	IJK.clear();
	uXYZ.clear();
	vXYZ.clear();
	aXYZ.clear();
	latticeBound.clear();
	quadMesh.clear();
	normalMesh.clear();
}

#pragma region Virtual function

/************************************************************************/
/* Virtual function                                                     */
/************************************************************************/
BOOL Catheter::newDoc()
{

	return true;
}

BOOL Catheter::OpenDoc(LPCTSTR pastName)
{
	constructMassSpringData();
	updateTriangleMesh();
	return true;
}

BOOL Catheter::closeDoc()
{
	return true;
}

// Draw
void Catheter::draw() 
{
	if (modeDraw & DRAW_MODE_LINE)
	{
		drawLattice();
	}
	
	if (modeDraw & DRAW_MODE_SURFACE)
	{
		dawSurface(modeDraw);
	}
	
	drawEndoScope();
}

bool Catheter::receiveKeyDown(keyCode key)
{
	if(GetAsyncKeyState(VK_SHIFT)) // Translate original point
	{
		switch (key)
		{
		case KEY_LEFT_ARROW:
			offsetOrigin(vec3d(0,-0.05,0));
			break;
		case KEY_RIGHT_ARROW:
			offsetOrigin(vec3d(0,0.05,0));
			break;
		case KEY_UP_ARROW:
			offsetOrigin(vec3d(0,0,0.05));
			break;
		case KEY_DOWN_ARROW:
			offsetOrigin(vec3d(0,0,-0.05));
			break;
		case KEY_Z:
			offsetOrigin(vec3d(-0.05,0,0));
			break;
		case KEY_X:
			offsetOrigin(vec3d(0.05,0,0));
			break;
		}
	}
	else if (GetAsyncKeyState(VK_CONTROL)) // rotate 
	{
		switch (key)
		{
		case KEY_LEFT_ARROW:
			setOffsetDirection(vec3d(-0.03,-0.03,0));
			break;
		case KEY_RIGHT_ARROW:
			setOffsetDirection(vec3d(0.03,0.03,0));
			break;
		case KEY_UP_ARROW:
			setOffsetDirection(vec3d(0.0,0.0,0.05));
			break;
		case KEY_DOWN_ARROW:
			setOffsetDirection(vec3d(0,0,-0.05));
			break;
		}
	}
	else
	{
		switch (key)
		{
		case KEY_X:
			setOffset(0.05);
			break;
		case KEY_Z:
			setOffset(-0.05);
			break;
		}
	}

	return false;
}

bool Catheter::receiFuntion(functionCode fCode)
{
	switch (fCode)
	{
	case TEST_FUNCTION:
	case TIMER_FUNCTION:
		continue1Step();
		updateTriangleMesh();
		updateFixIndex();
		break;
	}
	return false;
}

#pragma endregion Virtual function

/************************************************************************/
/*                                                                      */
/************************************************************************/
#pragma region Draw

void Catheter::drawEndoScope()
{
	glPushMatrix();
	glTranslatef(originalV.x, originalV.y, originalV.z);

	double thetaZ = 180/PI*atan2(directionV.y, directionV.x);
	double thetaY = 180/PI*asin(directionV.z/sqrt(directionV.x*directionV.x + directionV.y*directionV.x));
	glRotatef(thetaZ,0,0,1);
	glRotatef(-thetaY,0,1,0);

	//

	glPushMatrix(); // Endoscope
	glRotatef(90,0,1,0);
	glRotatef(90,0,0,1);
	glTranslatef(0,-0.3,0);
	endoscope.DrawObject(1,vec3d(0.4,0.4,0.4));
	glPopMatrix();


	glRotatef(-90,0,0,1);
	glScaled(2.5,3.5,2.5);
	arrow.DrawObject(1,vec3d(0.2,0.7,0.3));
	glScaled(1,1,1);

	glPopMatrix();

}
void Catheter::dawSurface(drawMode mode)
{
	if (quadMesh.size() <= 0)
	{
		return;
	}
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
	glColor3f(0.8,0.2,0.2);

	
	if (mode & DRAW_MODE_SURFACE)
	{
		int colorIndex = quadMesh.size() - 100; //hard code

		glBegin(GL_QUADS);
		for (int i = 0; i<quadMesh.size(); i++)
		{
			if (i > colorIndex)
				glColor3f(0.2,0.8,0.2);
			else
				glColor3f(1,1,1);

			Utility::GLNorm(normalMesh[i]);
			Utility::GLPoint(quadMesh[i]);
		}
		glEnd();
	}
	glPopAttrib();
}

void Catheter::drawLattice()
{
	for (int i = 0; i< latticeBound.size(); i++)
	{
		latticeBound[i].drawCenterLine();

		glColor3f(1.0,0.0,0.0);
		glBegin(GL_LINES);
		if (i < latticeBound.size() - 1)
		{
			Utility::GLPoint(latticeBound[i].centerPoint);
			Utility::GLPoint(latticeBound[i+1].centerPoint);

			for(int j = 0; j< 6; j++)
			{
				Utility::GLPoint(latticeBound[i].latticePoints[j]);
				Utility::GLPoint(latticeBound[i+1].latticePoints[j]);
			}
		}
		glEnd();
	}
}
#pragma endregion Draw

#pragma region Catheter data

void Catheter::setOffset(double add)
{
	offset += add;
}
double Catheter::getOffset()
{
	double temp = offset;
	offset = 0;
	return temp;
}

void Catheter:: offsetOrigin(vec3d offset)
{
	offsetOriginal = offsetOriginal + offset;
}
vec3d Catheter:: getOffsetOrginal()
{
	vec3d temp = offsetOriginal;
	offsetOriginal = vec3d();
	return temp;
}

void Catheter::setOffsetDirection(vec3d offset)
{
	offsetDirection = offsetDirection + offset;
}
vec3d Catheter::getOffsetDirection()
{
	vec3d temp = offsetDirection;
	offsetDirection = vec3d();
	return temp;
}

void Catheter::checkError()
{
	
}
// 
void Catheter::updateFixIndex()
{
	// Update length
	double offL = getOffset();
	if ((offL > 0 && fixIndex > 2) || (offL < 0 && fixIndex <= 9))
	{
		vec3d offV = directionV*offL;
		for (int i = 0; i< uXYZ.size(); i++)
		{
			if (i<fixIndex)
				uXYZ[i] = uXYZ[i] + offV; // TODO: Consider a more prices method
			
			XYZ[i] = XYZ[i] + offV;
		}
		// Update index
		if(offL > 0)//Increase length
		{
			vec3d prePoint = uXYZ[fixIndex - 1] - originalV;
			prePoint.normalize();
			prePoint = prePoint + directionV;
			if (prePoint.mag() > 1.8) // It equals 0 or 2
			{
				fixIndex = fixIndex - 1;
			}
		}
		else{ // Decrease length
			vec3d prePoint = uXYZ[fixIndex] - originalV;
			prePoint.normalize();
			prePoint = prePoint + directionV;
			if (prePoint.mag() < 1.8) // It equals 0 or 2
			{
				fixIndex = fixIndex + 1;
			}
		}
	}

	// Update original position
	vec3d offV = getOffsetOrginal();
	originalV = originalV + offV;
	for (int i = 0; i< uXYZ.size(); i++)
	{
		if (i<fixIndex)
			uXYZ[i] = uXYZ[i] + offV; // TODO: Consider a more prices method

		XYZ[i] = XYZ[i] + offV;
	}

	// Update direction
	vec3d offD = getOffsetDirection();
	directionV = directionV + offD;
	directionV.normalize();
	for (int i = 0; i< uXYZ.size(); i++)
	{
		float sign = i < fixIndex? -1:1;

		if (i<fixIndex)
		{
			double length = (uXYZ[i] - originalV).mag();
			uXYZ[i] = originalV + directionV*(sign*length); // TODO: Consider a more prices method
		}

		double length = (XYZ[i] - originalV).mag();
		XYZ[i] = originalV + directionV*(sign*length);
	}

	// Hard code -> To Fix coordinate
	for (int i = 0; i< fixIndex; i++)
	{
		uXYZ[i] = XYZ[i];
		vXYZ[i] = vec3d();
		aXYZ[i] = vec3d();
	}
}

#pragma endregion Catheter data

#pragma region Mass-spring

void Catheter::constructMassSpringData()
{
	originalV = vec3d(0.0, 0.0, 0.0);
	directionV = vec3d(1.0, 0.0, 0.0); // Must be unit vector
	// Construct catheter data
	int NNODE = 10;
	for(int i = 0; i<NNODE; i++)
	{
		int a = NNODE-1-i;
		vec3d coord = originalV - directionV*(a)*constant.elementLength;
		XYZ.push_back(coord);
		if (i<NNODE-1)
		{
			vec3i element(i, i+1, 0);
			IJK.push_back(element);
		}
	}

	fixIndex = NNODE-1;

	uXYZ = XYZ;
	vXYZ = std::vector<vec3d>(NNODE, vec3d());
	aXYZ = std::vector<vec3d>(NNODE, vec3d());

	//updateFixIndex();
}
matrix EyeMatrix(int size)
{
	matrix eye(size,size);
	for (int i = 0; i<size; i++)
	{
		eye(i,i) = 1;
	}
	return eye;
}
matrix Catheter:: arrayVtoarray1(arrayvec3d input)
{
	int NNODE = input.size();
	matrix outM(3*NNODE,1);
	for (int i = 0; i<NNODE; i++)
	{
		for (int j = 0; j< 3; j++)
		{
			outM(i*3+j,0) = input[i][j];
		}
	}
	return outM;
}
arrayvec3d Catheter::array1toArayV(matrix input)
{
	arrayvec3d output;
	int size = input.Row();
	for (int i = 0; i<size/3; i++)
	{
		float a[3];
		for(int j = 0; j<3; j++)
		{
			a[j] = input(i*3+j,0);
		}
		output.push_back(vec3d(a));
	}
	return output;
}

void Catheter::continue1Step()
{
	int NNODE = XYZ.size();
	matrix MM = EyeMatrix(3*NNODE)*constant.mass;
	matrix MC = EyeMatrix(3*NNODE)*constant.cDamping;

	matrix MK(3*NNODE, 3*NNODE);
	matrix MF(3*NNODE,1);

	// Construct matrix
	structureSpring(MF);
	bendingSpring(MF);
	addGravity(MF);
	restoreSping(MF);


	
	// Time integration
	matrix U = arrayVtoarray1(uXYZ);
	matrix dU = arrayVtoarray1(vXYZ);
	matrix ddU = arrayVtoarray1(aXYZ);

	matrix MKU = addPenalty(MM);
	matrix MKF = MF - MC*dU;

 	matrix ddUN(3*NNODE,1);
 	MKU.GaussElimination(&MKF, &ddUN);

	matrix dUN = dU + ddUN*constant.dt;
	matrix UN = U + dUN*constant.dt;

	//TODO: Try implicit integration

	// Result
	uXYZ = array1toArayV(UN);
	vXYZ = array1toArayV(dUN);
	aXYZ = array1toArayV(ddUN);
}

matrix Catheter::solve(matrix K, matrix f)
{
	//This is simple case -> diagonal matrix
	int row = f.Row();
	matrix x(row,1);
	for (int i = 0; i< row; i++)
	{
		x(i,0) = f(i,0)/K(i,i);
	}
	return x;
}
void Catheter::structureSpring(matrix& MF)
{
	int NEL = IJK.size();
	for (int ne = 0; ne < NEL; ne ++)
	{
		int node1 = IJK[ne][0];
		int node2 = IJK[ne][1];
		
		//vec3d vector0 = XYZ[node2] - XYZ[node1]; double L0 = vector0.mag();
		double L0 = constant.elementLength;
		vec3d vectorC = uXYZ[node2] - uXYZ[node1]; double L = vectorC.mag();

		vec3d Fe = vectorC*(constant.kElastic*(L-L0)/L); // Elastic force
		vec3d Fd = (vXYZ[node1] - vXYZ[node2])*(-constant.kDamping); // Damping force
		
		// Assemble to MF
		for (int j = 0; j<3; j++)
		{
			MF((node1 )*3 + j,0) += Fe[j] + Fd[j];
			MF((node2 )*3 + j,0) -= Fe[j] + Fd[j];
		}
	}
}
void Catheter::bendingSpring(matrix& MF)
{
	int NNODE = XYZ.size();
	for (int i = 1; i<NNODE-1; i++)
	{
		vec3d r12 = uXYZ[i] - uXYZ[i-1]; // Vector connect to neighbor node
		vec3d r23 = uXYZ[i+1] - uXYZ[i];

		vec3d normalV = r23.cross(r12); // Normal vector
		if (normalV.mag() == 0)
			continue;
		normalV.normalize();

		vec3d crossX = r12.cross(r23); // Moment
		double theta = asin(crossX.mag()/(r12.mag()*r23.mag()));
		double tau = constant.kBending*theta;

		vec3d vF23 = normalV.cross(r23); vF23.normalize(); // Force
		vec3d F23 = vF23*(tau/r23.mag());
		vec3d vF21 = normalV.cross(r12); vF21.normalize();
		vec3d F21 = vF21*(tau/r12.mag());

		// Add to MF
		for (int j = 0; j< 3; j++)
		{
			MF((i-1)*3 + j,0) += F21[j];
			MF((i+1)*3 + j,0) += F23[j];
			MF((i)*3 + j,0) -= (F21[j] + F23[j]);
		}
	}
}
void Catheter::addGravity(matrix& MF)
{
	int NNODE = XYZ.size();
	for(int i = 0; i < NNODE; i++)
	{
		for (int j = 0; j< 3; j++)
		{
			MF(i*3 + j,0) += constant.mass*constant.gravity[j];
		}
	}
}
void Catheter::restoreSping(matrix& MF)
{
	//Attach restore spring to last fixed point
	vec3d deform = uXYZ[fixIndex] - XYZ[fixIndex];
	vec3d Fe = deform*constant.kRestore;
	for (int j = 0; j < 3; j++)
	{
		
		MF(fixIndex*3 + j,0) -= Fe[j];
	}
}
matrix Catheter::addPenalty(matrix MM)
{
	matrix MKU = MM;
	for (int i = 0; i < fixIndex; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			MKU(i*3+j, i*3+j) += constant.penalty;
		}
	}
	
	return MKU;
}
#pragma endregion Mass-spring

#pragma region Visualize

// Construct catheter surface from centerline data
matrix rotationMat(vec3d normalV)
{
	double alpha = atan2(normalV.y, normalV.x);
	double beta = PI/2 - atan2(normalV.z,sqrt(normalV.x*normalV.x + normalV.y*normalV.y));

	matrix rotZ(3,3);
	rotZ(0,0) = cos(alpha); rotZ(0,1) = -sin(alpha); rotZ(0,2) = 0;
	rotZ(1,0) = sin(alpha); rotZ(1,1) = cos(alpha); rotZ(1,2) = 0;
	rotZ(2,0) = 0; rotZ(2,1) = 0; rotZ(2,2) = 1;

	matrix rotY(3,3);
	rotY(0,0) = cos(beta); rotY(0,1) = 0; rotY(0,2) = sin(beta);
	rotY(1,0) = 0; rotY(1,1) = 1; rotY(1,2) = 0;
	rotY(2,0) = -sin(beta); rotY(2,1) = 0; rotY(2,2) = cos(beta);

	matrix resultM = rotZ*rotY;
	return resultM;
}

void Catheter::updateTriangleMesh()
{
	std::vector<latticeSlice> newLattice;
	
	// Construct new lattice 
	int NNODE = XYZ.size();
	for (int i = 0; i <= NNODE; i++) // size() + 1 lattice
	{
		// Center point and normal vector

		
		vec3d normalV;
		if (i == 0)
		{	normalV = uXYZ[i+1] - uXYZ[i];}
		else if (i == NNODE - 1 || i == NNODE)
		{	normalV = uXYZ[NNODE - 1] - uXYZ[NNODE - 2];	}
		else
		{	normalV = uXYZ[i+1] - uXYZ[i-1];	}
		normalV.normalize();

		vec3d original;
		if (i < NNODE)
		{
			original = uXYZ[i];
		}
		else
		{
			original = uXYZ[i-1] + normalV*constant.elementLength;
		}

		// Rotation matrix
		matrix rotation = rotationMat(normalV);
		// Lattice points
		arrayvec3d latticePoints;
		float dphi = PI/3;
		for (int j = 0; j< 6; j++)
		{
			vec3d localV(constant.radius*cos(j*dphi), constant.radius*sin(j*dphi), 0.0);
			vec3d point = original + rotation.mulVector(localV);
			latticePoints.push_back(point);
		}
		latticeSlice newSlice;
		newSlice.centerNorm = normalV;
		newSlice.centerPoint = original;
		newSlice.latticePoints = latticePoints;

		// Push to lattice point set
		newLattice.push_back(newSlice);
	}
	//
	latticeBound = newLattice;

	if (!(modeDraw & DRAW_MODE_SURFACE))
	{
		return;
	}
	// Construct new surface point
	arrayvec3d quadA; // To draw directly fixIndex-1
	arrayvec3d normA;
	for (int i = fixIndex-1; i < latticeBound.size()-1; i++)
	{
		for (int j = 0; j< 6; j++)
		{
			quadInLattice(i,j, quadA, normA);
		}
	}
	
	quadMesh = quadA;
	normalMesh = normA;
}
void Catheter::addPoint(vec3d uvw, int iOff, int vOff, arrayvec3d& quadA, arrayvec3d& normalA)
{
	vec3d norm = (PrismaticLattice::transform(latticeBound, vec3d(1, uvw.y, uvw.z), iOff, vOff) 
		- PrismaticLattice::transform(latticeBound, vec3d(0, uvw.y, uvw.z), iOff, vOff));
	vec3d point = (PrismaticLattice::transform(latticeBound, vec3d(1, uvw.y, uvw.z), iOff,vOff));
	quadA.push_back(point); normalA.push_back(norm);
}
void Catheter::quadInLattice(int iOff, int vOff, arrayvec3d& quadA, arrayvec3d& normalA)
{
	int NV = constant.meshResolution; float dv = 1.0/NV;
	for (int j = 0; j < NV; j ++)
	{
		int NW = NV;float dw = 1.0/NW;
		for (int i = 0; i< NW; i++)
		{
			addPoint(vec3d(0,j*dv, i*dw), iOff, vOff, quadA, normalA);
			addPoint(vec3d(0,(j+1)*dv, i*dw), iOff, vOff, quadA, normalA);
			addPoint(vec3d(0,(j+1)*dv, (i+1)*dw), iOff, vOff, quadA, normalA);
			addPoint(vec3d(0,(j)*dv, (i+1)*dw), iOff, vOff, quadA, normalA);
		}
	}
}
#pragma endregion Visualize