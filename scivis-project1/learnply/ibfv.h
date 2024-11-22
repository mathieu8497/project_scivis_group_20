#pragma once
#include "polyhedron.h"

// Image Based Flow Visualization (Van Wijk 2002)
//https://www.win.tue.nl/~vanwijk/ibfv/
class IBFV
{
private:

	Polyhedron* poly;
	int width, height;
	static const int npn = 64;
	float scale = 4.0;
	float alpha = 8.0;
	float tmax, dmax;
	unsigned char* pixels;

public:

	IBFV(Polyhedron* p, int w, int h);
	~IBFV();
	void freepixels();
	void initIBFV(int w, int h);
	void drawIBFV();

};