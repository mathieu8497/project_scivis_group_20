#include "project1.h"
#include <algorithm>

void greyscale(Polyhedron* poly) {

	double M, m;
	// find minimum and maximum scalar values
	findMm(poly, M, m);

	// traverse the vertices
	for (auto i = 0; i < poly->nverts; i++)
	{
		auto& vertex = poly->vlist[i];
		double s_v = vertex->scalar;

		// set the RGB values based on the scalar value of the vertex
		// (s_v - m)/(M-m)
		double grey = (s_v - m) / (M - m);

		// Set all RGB values to grey
		vertex->R = vertex->G = vertex->B = grey;
	}
	
}

void multi(Polyhedron* poly) {
	double M, m;
	// find minimum and maximum scalar values
	findMm(poly, M, m);

	// traverse the vertices
	for (auto i = 0; i < poly->nverts; i++)
	{
		auto& vertex = poly->vlist[i];
		double s_v = vertex->scalar;

		// pick two colors
		icVector3 c1(0.0, 1.0, 0.0);
		icVector3 c2(1.0, 0.0, 0.0);

		// set the RGB values based on the scalar value of the vertex
		double left = (s_v - m) / (M - m);
		double right = (M - s_v) / (M - m);

		icVector3 c = c1 * left + c2 * right;

		vertex->R = c.x;
		vertex->G = c.y;
		vertex->B = c.z;
	}
}

void heatmap(Polyhedron* poly) {
	double M, m;
	m = INFINITY;
	M = -m;
	findMm(poly, M, m);

	for (auto i = 0; i < poly->nverts; i++)
	{
		auto& vertex = poly->vlist[i];

		double s_v = vertex->scalar;

		// two colors
		// BLUE
		icVector3 c1(0.0, 0.0, 1.0);
		// RED
		icVector3 c2(1.0, 0.0, 0.0);
		//
		icVector3 HSVc1, HSVc2;
		RGBtoHSV(HSVc1, c1);
		RGBtoHSV(HSVc2, c2);

		double l = (s_v - m) / (M - m);
		double r = (M - s_v) / (M - m);

		icVector3 HSVc = HSVc1 * l + HSVc2 * r;
		icVector3 c;
		HSVtoRGB(HSVc, c);

		vertex->R = c.x;
		vertex->G = c.y;
		vertex->B = c.z;
	}

}

void height(Polyhedron* poly) {
	double M, m;
	// find minimum and maximum scalar values
	findMm(poly, M, m);

	// traverse the vertices
	for (auto i = 0; i < poly->nverts; i++)
	{
		auto& vertex = poly->vlist[i];
		double s_v = vertex->scalar;

		// change the height of polyhedron based on the scalar value
		double l = (s_v - m) / (M - m);

		vertex->z = l * 10;
	}
}

void resetHeight(Polyhedron* poly)
{
	for (auto i = 0; i < poly->nverts; i++)
		poly->vlist[i]->z = 0;
}


void RGBtoHSV(icVector3& hsv, icVector3& rgb) {

	double r = rgb.x;
	double g = rgb.y;
	double b = rgb.z;

	double& h = hsv.x;
	double& s = hsv.y;
	double& v = hsv.z;

	// find min and max of RGB respectively
	double cmax = std::max(r, std::max(g, b));
	double cmin = std::min(r, std::min(g, b));

	double diff = cmax - cmin;

	if (cmax == cmin) {
		h = 0;
	}
	else if (cmax == r)
	{
		h = fmod(60 * ((g - b) / diff) + 360, 360);
	}
	else if (cmax == g) {
		h = fmod(60 * ((b - r) / diff) + 120, 360);
	}
	else if (cmax == b) {
		h = fmod(60 * ((r - g) / diff) + 240, 360);
	}

	if (cmax == 0)
	{
		s = 0;
	}
	else {
		s = diff / cmax;
	}

	v = cmax;
}

void HSVtoRGB(icVector3& hsv, icVector3& rgb) {

	double h = hsv.x;
	double s = hsv.y;
	double v = hsv.z;

	double C = s + v;
	double X = C * (1 - abs(fmod(h / 60, 2) - 1));
	double m = v - C;

	double r, g, b;

	if (h >= 0 && h < 60)
	{
		r = C;
		g = X;
		b = 0;
	}
	else if (h >= 60 && h < 120)
	{
		r = X;
		g = C;
		b = 0;
	}
	else if (h >= 120 && h < 180)
	{
		r = 0;
		g = C;
		b = X;
	}
	else if (h >= 180 && h < 240)
	{
		r = 0;
		g = X;
		b = C;
	}
	else if (h >= 240 && h < 300)
	{
		r = X;
		g = 0;
		b = C;
	}
	else {
		r = C;
		g = 0;
		b = X;
	}

	rgb.x = (r + m);
	rgb.y = (g + m);
	rgb.z = (b + m);
}

void findMm(Polyhedron* poly, double& M, double& m) {

	// initialize for min and max
	m = INFINITY;
	M = -m;

	// traverse the vertices
	for (auto i = 0; i < poly->nverts; i++)
	{
		auto& vertex = poly->vlist[i];

		// get the min
		if (vertex->scalar < m) {
			m = vertex->scalar;
		}

		// get the max
		if (vertex->scalar > M)
		{
			M = vertex->scalar;
		}
	}
}