#pragma once
#include "polyhedron.h"
#include "drawUtil.h">
#include <unordered_map>


enum CRIT_TYPE
{
	MINIMUM,
	MAXIMUM,
	SADDLE
};
enum COLOR_SCHEME
{
	SOLID,
	GRAYSCALE,
	BICOLOR,
	RAINBOW
};
struct CriticalPoint
{
	icVector3 loc;
	CRIT_TYPE type;
};
class ScalarTopology
{
private:
	Polyhedron* m_poly;
	double scalar_min;
	double scalar_max;
	std::unordered_map<Quad*, CriticalPoint> saddle_map;
	std::vector<CriticalPoint> criticals;
	std::vector<PolyLine> crit_contours;

	std::vector<bool> above_below;
	std::unordered_map<Edge*, icVector3> crossing_map;
	PolyLine user_contour;
	std::vector<PolyLine> spaced_contours;
public:
	ScalarTopology(Polyhedron* poly);
	~ScalarTopology();

	void calcUserContour(double s);
	void calcSpacedContours(int N);

	void drawCriticalPoints(bool use_height = false);
	void drawCritContours(bool use_height = false);
	void drawUserContour(bool use_height = false);
	void drawSpacedContours(bool use_height = false);

	COLOR_SCHEME color_scheme = COLOR_SCHEME::SOLID;


private:

	void getQuadValues(Quad* quad,
		double& x1, double& x2,
		double& y1, double& y2,
		double& f11, double& f12, double& f21, double& f22);
	void calcCriticals();
	PolyLine calcContour(double s);

	icVector3 getGrayscaleColor(double s);
	icVector3 getBiColor(double s);
	icVector3 getRainbowColor(double s);
	double getScaledHeight(double s);
};

