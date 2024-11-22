#pragma once
#include "polyhedron.h"

// q1
void greyscale(Polyhedron* poly);

//q1 b
void multi(Polyhedron* poly);

// q1 c
void heatmap(Polyhedron* poly);

//convert RGBtoHSV
void RGBtoHSV(icVector3& hsv, icVector3& rgb);

//convert HSVtoRGB
void HSVtoRGB(icVector3& hsv, icVector3& rgb);

//q2
void height(Polyhedron* poly);
void resetHeight(Polyhedron* poly);

void findMm(Polyhedron* poly, double& M, double& m);