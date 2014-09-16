#include "stdafx.h"
#include "energyMnager.h"
#include "Utility_wrap.h"

using namespace std;

energyMnager::energyMnager()
{
}


energyMnager::~energyMnager()
{
	for (int i = 0; i < m_sphereArray.size(); i++)
	{
		m_sphereArray[i].reset();
	}
	m_sphereArray.clear();

	for (int i = 0; i < m_boneArray.size(); i++)
	{
		m_boneArray[i].reset();
	}
	m_boneArray.clear();
}

void energyMnager::initTest()
{

}

void energyMnager::drawSphere()
{
	for (int i = 0; i < m_sphereArray.size(); i++)
	{
		m_sphereArray[i]->draw();
	}
}

void energyMnager::drawFixConstraint()
{
	for (auto v : fixConstraints)
	{
		glLineWidth(3.0);
		glBegin(GL_LINES);

		glVertex3fv(m_sphereArray[v[0]]->centerPos().data());
		glVertex3fv(m_sphereArray[v[1]]->centerPos().data());

		glEnd();
		glLineWidth(1.0);
	}
}

void energyMnager::drawNeighbor()
{
	for (auto v : neighborCts)
	{
		glLineWidth(2.0);
		glBegin(GL_LINES);

		glVertex3fv(m_sphereArray[v[0]]->centerPos().data());
		glVertex3fv(m_sphereArray[v[1]]->centerPos().data());

		glEnd();
		glLineWidth(1.0);
	}
}

void energyMnager::initFromSkeleton(skeletonPtr ske)
{
	// We discretize the skeleton to energy ball
	vector<bone*> boneArray;
	vector<pair<int, int>> neighborInfo;

	ske->getBoneAndNeighborInfo(boneArray, neighborInfo);
	ske->buildTransformMatrix();

	// 1. Init sphere inside bone
	for (int i = 0; i < boneArray.size(); i++)
	{
		bone* curB = boneArray[i];
		Vec3f sizef = curB->m_sizef;

		// 1. Find shortest edge
		Vec3i lOrder = Util_w::SMLIndexSizeOrder(sizef);
		float radius = sizef[lOrder[0]] / 2;

		// 2. divide the edge
		Vec3i numXYZ;
		Vec3f offsetXYZ;
		for (int ii = 0; ii < 3; ii++)
		{
			numXYZ[ii] = round(sizef[ii] / (2 * radius));
			offsetXYZ[ii] = sizef[ii] / numXYZ[ii];
		}

		// We translate it to the backward corner of the box

		Vec3f trans;
		trans[0] = -sizef[0] / 2;
		trans[1] = -sizef[1] / 2;
		trans[2] = 0;
		trans = trans + offsetXYZ / 2;
		boneSpherePtr newB = boneSpherePtr(new boneSphere(curB));

		for (int ii = 0; ii < numXYZ[0]; ii++)
		{
			for (int jj = 0; jj < numXYZ[1]; jj++)
			{
				for (int kk = 0; kk < numXYZ[2]; kk++)
				{
					Vec3f curlocalPos = Vec3f(ii*offsetXYZ[0], jj*offsetXYZ[1], kk*offsetXYZ[2]) + trans;
					Vec4f p(curlocalPos[0], curlocalPos[1], curlocalPos[2], 1);
					Mat4x4f tMat = curB->transformMat;
					Vec4f wPos4 = tMat*p;
					Vec3f wPos(wPos4[0], wPos4[1], wPos4[2]);
					// Create new sphere
					skeSpherePtr newSphere = skeSpherePtr(new skeSphere(wPos, radius));

					m_sphereArray.push_back(newSphere);
					newB->sphereIdxs.push_back(m_sphereArray.size() - 1);
				}
			}
		}
		m_boneArray.push_back(newB);
	}

	initConstraints(neighborInfo);
}

void energyMnager::initConstraints(vector<pair<int, int>> neighborInfo)
{
	// 2. Hard constraint
	for (auto curB : m_boneArray)
	{
		arrayInt sphereIdx = curB->sphereIdxs;
		for (int j = 0; j < sphereIdx.size(); j++)
		{
			for (int k = j + 1; k < sphereIdx.size(); k++)
			{
				fixConstraints.push_back(Vec2i(sphereIdx[j], sphereIdx[k]));
			}
		}
	}

	// 3. Neighbor constraint
	for (auto nb : neighborInfo)
	{
		int idxParent = nb.first;
		int idxChild = nb.second;

		arrayInt sphereParent = m_boneArray[idxParent]->sphereIdxs;
		arrayInt sphereChild = m_boneArray[idxChild]->sphereIdxs;

		// Shortest distance between sphere
		float shortest = MAX;
		for (auto idx1 : sphereParent)
		{
			for (auto idx2 : sphereChild)
			{
				float distance = (m_sphereArray[idx1]->centerPos() - m_sphereArray[idx2]->centerPos()).norm();
				if (shortest > distance)
				{
					shortest = distance;
				}
			}
		}

		// Find the sphere that near
		shortest *= 1.05; // To avoid epsilon error
		for (auto idx1 : sphereParent)
		{
			for (auto idx2 : sphereChild)
			{
				float distance = (m_sphereArray[idx1]->centerPos() - m_sphereArray[idx2]->centerPos()).norm();
				if (distance < shortest)
				{
					neighborCts.push_back(Vec2i(idx1, idx2));
				}
			}
		}
	}
}

energyMngerPtr energyMnager::clone() const
{
	energyMngerPtr newObj = energyMngerPtr(new energyMnager);
	*newObj = *this;

	newObj->m_sphereArray.clear();
	newObj->m_boneArray.clear();

	// Clone sphere
	for (auto s : m_sphereArray)
	{
		skeSpherePtr newS = skeSpherePtr(new skeSphere(s.get()));
		newObj->m_sphereArray.push_back(newS);
	}

	// Clone bone array
	for (auto b : m_boneArray)
	{
		boneSpherePtr newb = boneSpherePtr(new boneSphere(b.get()));
		newObj->m_boneArray.push_back(newb);
	}

	return newObj;
}
