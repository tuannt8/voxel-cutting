#include "stdafx.h"
#include "boneSphere.h"


boneSphere::boneSphere()
{
}

boneSphere::boneSphere(bone* b)
{
	originBone = b;
}

boneSphere::boneSphere(boneSphere *b)
{
	*this = *b;
}


boneSphere::~boneSphere()
{
}
