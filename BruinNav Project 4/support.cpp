#include "support.h"
using namespace std;

bool operator>(const GeoCoord& coord1, const GeoCoord& coord2) {
	if (coord1.latitudeText > coord2.latitudeText) {
		return true;
	}
	else if (coord1.latitudeText == coord2.latitudeText) {
		if (coord1.longitudeText <= coord2.longitudeText) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

bool operator<(const GeoCoord& coord1, const GeoCoord& coord2) {
	if (coord1.latitudeText < coord2.latitudeText) {
		return true;
	}
	else if (coord1.latitudeText == coord2.latitudeText) {
		if (coord1.longitudeText >= coord2.longitudeText) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

bool operator==(const GeoCoord& coord1, const GeoCoord& coord2) {
	if (coord1.latitudeText == coord2.latitudeText) {
		if (coord1.longitudeText == coord2.longitudeText) {
			return true;
		}
	}
	return false;
}

string doubleToDirection(double angle) {
	if (angle >= 0 && angle <= 22.5) {
		return "east";
	}
	else if (angle > 22.5 && angle <= 67.5) {
		return "northeast";
	}
	else if (angle > 67.5 && angle <= 112.5) {
		return "north";
	}
	else if (angle > 112.5 && angle <= 157.5) {
		return "northwest";
	}
	else if (angle > 157.5 && angle <= 202.5) {
		return "west";
	}
	else if (angle > 202.5 && angle <= 247.5) {
		return "southwest";
	}
	else if (angle > 247.5 && angle <= 292.5) {
		return "south";
	}
	else if (angle > 292.5 && angle <= 337.5) {
		return "southeast";
	}
	else if (angle > 337.5 && angle <= 360) {
		return "east";
	}
}

string directionTurn(double angle){
	if (angle > 180){
		return "right";
	}
	else{
		return "left";
	}
}