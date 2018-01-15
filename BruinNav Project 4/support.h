#ifndef SUPPORT_H
#define SUPPORT_H
#include "provided.h"
#include <string>

bool operator>(const GeoCoord& coord1, const GeoCoord& coord2);
bool operator<(const GeoCoord& coord1, const GeoCoord& coord2);
bool operator==(const GeoCoord& coord1, const GeoCoord& coord2);
std::string doubleToDirection(double angle);
std::string directionTurn(double angle);

struct doubleSegment
{
	GeoSegment geoSeg1;
	GeoSegment geoSeg2;
	double distanceTravelledSoFar;
};
struct DistanceToEnd
{
	bool operator()(const doubleSegment& lhs, const doubleSegment& rhs) const
	{
		return lhs.distanceTravelledSoFar + distanceEarthMiles(lhs.geoSeg2.start, lhs.geoSeg2.end) > lhs.distanceTravelledSoFar + distanceEarthMiles(rhs.geoSeg2.start, rhs.geoSeg2.end);
	}
};
#endif