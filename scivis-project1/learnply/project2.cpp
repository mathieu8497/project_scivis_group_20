#include "project2.h"
#include "project1.h"
#include <iostream>

ScalarTopology::ScalarTopology(Polyhedron* poly)
{
	m_poly = poly;
	above_below = std::vector<bool>(m_poly->nverts, false);
	findMm(m_poly, scalar_max,scalar_min);
	calcCriticals();
}

ScalarTopology::~ScalarTopology()
{
}
void ScalarTopology::calcUserContour(double s)
{
	if (s <= scalar_min || s >= scalar_max)
	{
		std::cout << "Scalar value is out of range" << std::endl;
		return;
	}
	user_contour.clear();
	user_contour = calcContour(s);
	if (user_contour.empty())
		std::cout << "Contour is a single point " << std::endl;
	else
		std::cout << "Calculated Contour at " << s << std::endl;

}
void ScalarTopology::calcSpacedContours(int N)
{
	spaced_contours.clear();
	//divide the scalar rangge into N even intervals
	double step = (scalar_max - scalar_min) / (double)(N+1);
	for (int i = 0; i < N; i++)
	{
		double s = scalar_min + (double)(i + 1) * step;
		spaced_contours.push_back(calcContour(s));
	}
	std::cout << "Calculated " << spaced_contours.size() << " contours" << std::endl;
}

void ScalarTopology::getQuadValues(Quad* quad,
	double& x1, double& x2,
	double& y1, double& y2,
	double& f11, double& f12, double& f21, double& f22)
{
	x1 = y1 = INFINITY;
	x2 = y2 = -INFINITY;
	for (int j = 0; j < 4; j++)
	{
		if (quad->verts[j]->x < x1)
			x1 = quad->verts[j]->x;
		if (quad->verts[j]->y < y1)
			y1 = quad->verts[j]->y;
		if (quad->verts[j]->x > x2)
			x2 = quad->verts[j]->x;
		if (quad->verts[j]->y > y2)
			y2 = quad->verts[j]->y;
	}
	for (int j = 0; j < 4; j++)
	{
		if (quad->verts[j]->x == x1 && quad->verts[j]->y == y1)
			f11 = quad->verts[j]->scalar;
		else if (quad->verts[j]->x == x1 && quad->verts[j]->y == y2)
			f12 = quad->verts[j]->scalar;
		else if (quad->verts[j]->x == x2 && quad->verts[j]->y == y1)
			f21 = quad->verts[j]->scalar;
		else
			f22 = quad->verts[j]->scalar;
	}
}



void ScalarTopology::calcCriticals()
{
	//clear any saved critical points
	criticals.clear();
	saddle_map.clear();
	crit_contours.clear();

	//first loop through all the quads to find critical points occruing in the interior of a quad
	for (int i = 0; i < m_poly->nquads; i++)
	{
		Quad* quad = m_poly->qlist[i];
		double x1, x2, y1, y2, f11, f12, f21, f22;
		//find coordinates of a potential critical point
		getQuadValues(quad, x1, x2, y1, y2, f11, f12, f21, f22);
		if (f11 - f21 - f12 + f22 == 0)
			continue;
		double x0, y0, f00;
		x0 = (x2 * f11 - x1 * f21 - x2 * f12 + x1 * f22) / (f11 - f21 - f12 + f22);


		y0 = (y2 * f11 - y2 * f21 - y1 * f12 + y1 * f22) / (f11 - f21 - f12 + f22);
		// check if coordinates are outside the quad
		if (x0< x1 || x0 > x2 || y0 < y1 || y0 > y2)
			continue;
		//use bilinear interpolation to find the value of the critical point
		f00 = ((x2 - x0) * (y2 - y0) * f11 + (x0 - x1) * (y2 - y0) * f21 +
			(x2 - x0) * (y0 - y1) * f12 + (x0 - x1) * (y0 - y1) * f22) /
			((x2 - x1) * (y2 - y1));
		//create a critical point
		CriticalPoint crit;
		crit.loc = icVector3(x0, y0, f00);
		// the hessian matrix will help us classify the critical point
		double b = (f11 - f21 - f12 + f22) / ((x2 - x1) * (y2 - y1));
		//the Hessian matrix has zeros on the diagonal so the eigenvalues are -2b and +2b
		crit.type = CRIT_TYPE::SADDLE;
		criticals.push_back(crit);
		saddle_map.insert(std::make_pair(quad, crit));
	}
		//then loop through all the vertices to check for local mins and maxes
		for (int i = 0; i < m_poly->nverts; i++)
		{
			Vertex* vert = m_poly->vlist[i];
			bool min = true;
			bool max = true;
			// if a vertex has a scalar value less than or greater than
			// of its neighbors, it is a local min or max
			for (int j = 0; j < vert->nedges; j++)
			{
				Edge* edge = vert->edges[j];
				Vertex* other = (edge->verts[0] == vert) ? edge->verts[1] : edge->verts[0];
				if (vert->scalar <= other->scalar)
					max = false;
				else if (vert->scalar >= other->scalar)
					min = false;
			}
			if (!min && !max)
				continue;
			// create the new critical point
			CriticalPoint crit;
			crit.loc = icVector3(vert->x, vert->y, vert->scalar);
			if (min)
				crit.type = CRIT_TYPE::MINIMUM;
			else
				crit.type = CRIT_TYPE::MAXIMUM;

			criticals.push_back(crit);
		}
		//calculate contours at each critical point
		for (const CriticalPoint& cp : criticals)
		{
			double s = cp.loc.z;
			crit_contours.push_back(calcContour(s));
		}
	}


PolyLine ScalarTopology::calcContour(double s)
{
	// loop through all vertices and classify them as above or below s
	for (int i = 0; i < m_poly->nverts; i++)
	{
		if (m_poly->vlist[i]->scalar < s)
			above_below[i] = false;
		else
			above_below[i] = true;
	}
	//loop through all edges to find crossing points
	crossing_map.clear();
	for (int i = 0; i < m_poly->nedges; i++)
	{
		Edge* edge = m_poly->elist[i];
		Vertex* v1 = edge->verts[0];
		Vertex* v2 = edge->verts[1];
		if (above_below[v1->index] != above_below[v2->index])
		{
			// use linear interpolation to find the crossing point (Lecture_08 slide 51)
			double a = (s - v1->scalar) / (v2->scalar - v1->scalar);
			double cx = v1->x + a * (v2->x - v1->x);
			double cy = v1->y + a * (v2->y - v1->y);
			double cz = s;
			icVector3 cross = icVector3(cx, cy, cz);
			crossing_map.insert(std::make_pair(edge, cross));
		}
	}
	//loop through all quads and connect crossing points
	PolyLine contour;
	for (int i = 0; i < m_poly->nquads; i++)
	{
		Quad* quad = m_poly->qlist[i];
		std::vector<icVector3> cross_points;
		// get saved crossing points
		for (int j = 0; j < 4; j++)
		{
			Edge* edge = quad->edges[j];
			if (crossing_map.find(edge) != crossing_map.end())
				cross_points.push_back(crossing_map.at(edge));
		}

		// If there are only two crossing points, connect them with a line segment.
		// If there are 4 crossing points, we need to be more careful.
		if (cross_points.size() == 2)
		{
			LineSegment ls(cross_points[0], cross_points[1]);
			contour.push_back(ls);
		}
		else if (cross_points.size() == 4)
		{
			// if the quad contains a saddle point at the same height as the contour,
			// connect each crossing point to the saddle. Otherwise, we try connecting each pair of
			// crossing points and see which combinations give us line segment midpoints closest to the contour
			bool saddle_on_contour = false;
			CriticalPoint* saddle = NULL;
			if (saddle_map.find(quad) != saddle_map.end())
			{
				saddle = &saddle_map.at(quad);
				if (saddle->loc.z == s)
					saddle_on_contour = true;
			}

			if (saddle_on_contour)
			{
				icVector3 sloc = saddle_map.at(quad).loc;
				contour.push_back(LineSegment(cross_points[0], sloc));
				contour.push_back(LineSegment(cross_points[1], sloc));
				contour.push_back(LineSegment(cross_points[2], sloc));
				contour.push_back(LineSegment(cross_points[3], sloc));
			}
			else
			{
				// all possible line segments between adjacent crossing points
				LineSegment ls01(cross_points[0], cross_points[1]);
				LineSegment ls23(cross_points[2], cross_points[3]);
				LineSegment ls12(cross_points[1], cross_points[2]);
				LineSegment ls30(cross_points[3], cross_points[0]);

				//line segment candidate midpoints
				icVector3 m01 = ls01.midpoint();
				icVector3 m30 = ls30.midpoint();

				// use bilinear interpolation to get the scalar values at the midpoints
				double x1, x2, y1, y2, f11, f12, f21, f22;
				getQuadValues(quad, x1, x2, y1, y2, f11, f12, f21, f22);
				double s01 = ((x2 - m01.x) * (y2 - m01.y) * f11 + (m01.x - x1) * (y2 - m01.y) * f21 +
					(x2 - m01.x) * (m01.y - y1) * f12 + (m01.x - x1) * (m01.y - y1) * f22) /
					((x2 - x1) * (y2 - y1));
				double s30 = ((x2 - m30.x) * (y2 - m30.y) * f11 + (m30.x - x1) * (y2 - m30.y) * f21 +
					(x2 - m30.x) * (m30.y - y1) * f12 + (m30.x - x1) * (m30.y - y1) * f22) /
					((x2 - x1) * (y2 - y1));

				// if we have a saddle, check which sides of the saddle the contour passes through
				if (saddle)
				{
					double fs = saddle->loc.z;
					if ((s > fs && s01 > fs) || (s < fs && s01 < fs))
					{
						contour.push_back(ls01);
						contour.push_back(ls23);
					}
					else
					{
						contour.push_back(ls12);
						contour.push_back(ls30);
					}
				}
				// if there is no saddle, check which midpoints are closer to the contour value
				else
				{
					if (abs(s - s01) < abs(s - s30))
					{
						contour.push_back(ls01);
						contour.push_back(ls23);
					}
					else
					{
						contour.push_back(ls12);
						contour.push_back(ls30);
					}
				}
			}
		}
	}
	return contour;
}

void ScalarTopology::drawCriticalPoints(bool use_height) {
	for (const CriticalPoint& cp : criticals)
	{
		icVector3 loc = cp.loc;
		if (use_height)
			loc.z = getScaledHeight(loc.z);
		else
			loc.z = 0.0;
		switch (cp.type)
		{
		case CRIT_TYPE::MAXIMUM:
			drawSphere(loc.x, loc.y, loc.z, 0.1, 1.0, 0.0, 0.0);
			break;
		case CRIT_TYPE::MINIMUM:
			drawSphere(loc.x, loc.y, loc.z, 0.1, 0.0, 0.0, 1.0);
			break;
		case CRIT_TYPE::SADDLE:
			drawSphere(loc.x, loc.y, loc.z, 0.1, 1.0, 0.0, 1.0);
			break;
		default:
			break;
		}
	}
}
void ScalarTopology::drawCritContours(bool use_height) {
	if (crit_contours.empty())
		return;
	for (int i = 0; i < criticals.size(); i++)
	{
		CRIT_TYPE type = criticals[i].type;
		double s = criticals[i].loc.z;
		PolyLine contour = crit_contours[i];

		if (contour.empty())
			continue;

		if (use_height)
			s = getScaledHeight(s);
		else
			s = 0.0;
		for (int i = 0; i < contour.size(); i++)
		{
			contour.at(i).start.z = s;
			contour.at(i).end.z = s;
		}

		switch (type)
		{
		case CRIT_TYPE::MAXIMUM:
			//drawPolyLine(contour, 1.5, 1.0, 0.0, 0.0);
			break;
		case CRIT_TYPE::MINIMUM:
			//drawPolyLine(contour, 1.5, 0.0, 0.0, 1.0);
			break;
		case CRIT_TYPE::SADDLE:
			drawPolyLine(contour, 1.5, 1.0, 0.0, 1.0);
			break;
		default:
			break;
		}
	}
}
void ScalarTopology::drawUserContour(bool use_height) {
	if (user_contour.empty())
		return;

	PolyLine contour = user_contour;
	double s = contour[0].start.z;

	if (use_height)
		s = getScaledHeight(s);
	else
		s = 0.0;
	for (int i = 0; i < contour.size(); i++)
	{
		contour.at(i).start.z = s;
		contour.at(i).end.z = s;
	}

	drawPolyLine(contour, 2.0, 1.0, 1.0, 1.0);
}
void ScalarTopology::drawSpacedContours(bool use_height) {
	if (spaced_contours.empty())
		return;

	icVector3 color(0.0, 0.0, 0.0);
	for (PolyLine contour : spaced_contours)
	{
		if (contour.empty())
			continue;

		double s = contour[0].start.z;
		if (color_scheme == COLOR_SCHEME::GRAYSCALE)
			color = getGrayscaleColor(s);
		else if (color_scheme == COLOR_SCHEME::BICOLOR)
			color = getBiColor(s);
		else if (color_scheme == COLOR_SCHEME::RAINBOW)
			color = getRainbowColor(s);

		if (use_height)
			s = getScaledHeight(s);
		else
			s = 0.0;
		for (int i = 0; i < contour.size(); i++)
		{
			contour.at(i).start.z = s;
			contour.at(i).end.z = s;
		}

		drawPolyLine(contour, 1.0, color.x, color.y, color.z);
	}
}
icVector3 ScalarTopology::getGrayscaleColor(double s) {
	double gray = (s - scalar_min) / (scalar_max - scalar_min);
	return icVector3(gray, gray, gray);
}
icVector3 ScalarTopology::getBiColor(double s) {
	icVector3 c1(0.0, 0.0, 1.0);
	icVector3 c2(0.0, 1.0, 0.0);

	double left = (s - scalar_min) / (scalar_max - scalar_min);
	double right = (scalar_max - s) / (scalar_max - scalar_min);

	return c1 * left + c2 * right;
}
icVector3 ScalarTopology::getRainbowColor(double s) {
	icVector3 c1(0.0, 0.0, 1.0);
	icVector3 c2(1.0, 0.0, 0.0);
	icVector3 HSVc1, HSVc2;
	RGBtoHSV(HSVc1, c1);
	RGBtoHSV(HSVc2, c2);

	double left = (s - scalar_min) / (scalar_max - scalar_min);
	double right = (scalar_max - s) / (scalar_max - scalar_min);

	icVector3 HSVc = HSVc1 * left + HSVc2 * right;
	icVector3 c;
	HSVtoRGB(HSVc, c);

	return c;
}
double ScalarTopology::getScaledHeight(double s) {
	double new_s = (s - scalar_min) / (scalar_max - scalar_min);
	return new_s * 10;
}
