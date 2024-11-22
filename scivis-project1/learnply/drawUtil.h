#pragma once
#include "icVector.H"
#include <vector>

// simple struct for a line segment (usefull for Projects 2 and 3)
struct LineSegment
{
	icVector3 start, end;
	double len;
	
	LineSegment(icVector3 start_in, icVector3 end_in)
	{
		start = start_in;
		end = end_in;
		len = length(end - start);
	}
	LineSegment(double sx, double sy, double sz, double ex, double ey, double ez)
	{
		start = icVector3(sx, sy, sz);
		end = icVector3(ex, ey, ez);
		len = length(end - start);
	}
	icVector3 midpoint()
	{
		icVector3 diff = end - start;
		return start + (0.5 * diff);
	}
};

// A polyline is a list of (connected) line segments
typedef std::vector<LineSegment> PolyLine;


// Draws a single line segment
// width: width of the line segment
// R: red channel for the line color [0,1]
// B: blue channel for the line color [0,1]
// G: green channel of the line color [0,1]
void drawLineSegment(LineSegment ls, double width = 1.0, float R = 0.0, float G = 0.0, float B = 0.0);

// Draws a polyline assuming the end point and start point of consecutive segments is the same
// width: width of the line
// R: red channel for the line color [0,1]
// B: blue channel for the line color [0,1]
// G: green channel of the line color [0,1]
void drawPolyLine(PolyLine pl, double width = 1.0, float R = 0.0, float G = 0.0, float B = 0.0);


// Draws a sphere centered on the given coordinates
// x, y, z are the center coordinates
// radius: radius of the sphere
// R: red channel for the dot color [0,1]
// B: blue channel for the dot color [0,1]
// G: green channel of the dot color [0,1]
void drawSphere(double x, double y, double z, double radius = 0.15, float R = 0.0, float G = 0.0, float B = 0.0);


void dots_and_lines_example();