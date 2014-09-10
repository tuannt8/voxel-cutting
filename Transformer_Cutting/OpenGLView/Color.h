#pragma  once
#include "stdafx.h"
#include "DataTypes/vec.h"
#include <map>
#include <string>
#include <GL\gl.h>

typedef std::map<std::string, Vec3f> intMap;
#define nbFixColor 10
Vec3f fixColor[nbFixColor] = {Vec3f(0,0,1), Vec3f(0,1,0), Vec3f(1,0,0), Vec3f(1,0,1), Vec3f(0,1,1), Vec3f(0,0.5,1), Vec3f(0,1,.5), Vec3f(1,0.5,0), Vec3f(0.5,0,1), Vec3f(0,1,0.5)};

#define rand0_1 ((double) rand() / (RAND_MAX))

class ColorGL
{
public:	 
	ColorGL(){};
	~ColorGL(){};

	static void setGLColorForKey(char *key)
	{
		if (final)
			return;

		intMap::const_iterator pos = colorMap.find(key);
		if (pos == colorMap.end()) {
			//Create new color
			if (++idx < nbFixColor){
				(colorMap)[key] = fixColor[idx];
			}
			else{
				(colorMap)[key] = Vec3f(rand0_1, rand0_1, rand0_1);
			}
			
			pos = colorMap.find(key);
		} 

		Vec3f c = pos->second;
		glColor3f(c[0], c[1], c[2]);
	}

	static void finalize()
	{
		final = true;
	}

private:
	static intMap colorMap;
	static int idx;
	static bool final;
};

bool ColorGL:: final = false;
int ColorGL::idx = 0;
intMap ColorGL::colorMap;
