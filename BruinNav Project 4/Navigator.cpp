#include "provided.h"
#include <string>
#include <vector>
#include "MyMap.h"
#include <queue>
#include <algorithm>
using namespace std;

class NavigatorImpl
{
public:
	NavigatorImpl();
	~NavigatorImpl();
	bool loadMapData(string mapFile);
	NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	MapLoader ourMap;
	AttractionMapper ourAttractions;
	SegmentMapper ourStreetSegments;
	vector<NavSegment> reconstruct_path(const GeoCoord& start, const GeoCoord& end) const;
	MyMap<GeoCoord, GeoCoord>* mapForRetracing;
};

NavigatorImpl::NavigatorImpl()
{
	mapForRetracing = new MyMap<GeoCoord, GeoCoord>;
}

NavigatorImpl::~NavigatorImpl()
{
	delete mapForRetracing;
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	if (ourMap.load(mapFile))
	{
		ourAttractions.init(ourMap);
		ourStreetSegments.init(ourMap);
		return true;
	}
	return false;  // This compiles, but may not be correct
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	GeoCoord startCoordinate;
	GeoCoord endCoordinate;
	if (!ourAttractions.getGeoCoord(start, startCoordinate)){
		return NAV_BAD_SOURCE;
	}
	if (!ourAttractions.getGeoCoord(end, endCoordinate)){
		return NAV_BAD_DESTINATION;
	}

	vector<StreetSegment> firstSegs = ourStreetSegments.getSegments(startCoordinate);
	GeoSegment geoSeg1 = firstSegs[0].segment;
	vector<StreetSegment> secondSegs = ourStreetSegments.getSegments(endCoordinate);
	GeoSegment geoSeg2 = secondSegs[0].segment;
	if (geoSeg1.start == geoSeg2.start&&geoSeg1.end == geoSeg2.end)
	{
		GeoSegment angleLine(startCoordinate, endCoordinate);
		double angle = angleOfLine(angleLine);
		string direction = doubleToDirection(angle);
		NavSegment navSeg(direction, firstSegs[0].streetName, distanceEarthMiles(startCoordinate, endCoordinate), angleLine);
		directions.push_back(navSeg);
		return NAV_SUCCESS;
	}
	else
	{
		GeoCoord startGeo = geoSeg1.start;
		GeoCoord endGeo = geoSeg1.end;
		GeoSegment startGeoToEnd(startGeo, endCoordinate);
		GeoSegment endGeoToEnd(endGeo, endCoordinate);
		GeoSegment startToStartGeo(startCoordinate, startGeo);
		GeoSegment startToEndGeo(startCoordinate, endGeo);
		priority_queue<doubleSegment, vector<doubleSegment>, DistanceToEnd> openList;
		MyMap<GeoCoord, GeoCoord> closedList;
		doubleSegment firstDoubleSegment;
		doubleSegment secondDoubleSegment;
		double distance1 = 0;
		distance1 = distanceEarthMiles(startCoordinate, startGeo);
		double distance2 = 0;
		distance2 = distanceEarthMiles(startCoordinate, endGeo);
		firstDoubleSegment.geoSeg1 = startToStartGeo;
		firstDoubleSegment.geoSeg2 = startGeoToEnd;
		firstDoubleSegment.distanceTravelledSoFar = distance1;
		secondDoubleSegment.geoSeg1 = startToEndGeo;
		secondDoubleSegment.geoSeg2 = endGeoToEnd;
		secondDoubleSegment.distanceTravelledSoFar = distance2;
		openList.push(firstDoubleSegment);
		openList.push(secondDoubleSegment);
		mapForRetracing->associate(startGeo, startCoordinate);
		mapForRetracing->associate(endGeo, startCoordinate);
		vector<GeoCoord*> checker;
		checker.push_back(&firstDoubleSegment.geoSeg1.end);
		checker.push_back(&secondDoubleSegment.geoSeg1.end);
		while (!openList.empty())
		{
			double prevDistance;
			doubleSegment doubSeg = openList.top();
			prevDistance = doubSeg.distanceTravelledSoFar;
			openList.pop();
			GeoCoord curr = doubSeg.geoSeg1.end;
			vector<StreetSegment> segs3 = ourStreetSegments.getSegments(curr);
			for (int i = 0; i < segs3.size(); i++)
			{
				GeoSegment geoSeg3 = segs3[i].segment;
				if (geoSeg3.start == geoSeg2.start&&geoSeg3.end == geoSeg2.end)
				{

					mapForRetracing->associate(endCoordinate, curr);
					directions = reconstruct_path(startCoordinate, endCoordinate);
					return NAV_SUCCESS;
				}

				else
				{

					if (closedList.find(geoSeg3.start) != nullptr&& closedList.find(geoSeg3.end) != nullptr)
					{

					}
					else
					{
						
						GeoCoord* test1 = mapForRetracing->find(geoSeg3.start);
						GeoCoord* test2 = mapForRetracing->find(geoSeg3.end);

						vector<GeoCoord*>::iterator itr1 = find(checker.begin(), checker.end(), test1);
						GeoCoord* coordinate = nullptr;
						GeoCoord* coordinate2 = nullptr;
						if (itr1 != checker.end())
						{
							 coordinate = *itr1;
						}
						itr1 = find(checker.begin(), checker.end(), test2);

						if (itr1 != checker.end())
						{
							 coordinate2 = *itr1;
						}
						
						
						if (test1 == nullptr || test2 == nullptr)
						{
							if (test1 == nullptr)
							{
								if (coordinate == nullptr)
								{
									closedList.associate(geoSeg3.start, geoSeg3.start);
									mapForRetracing->associate(geoSeg3.start, curr);
									GeoSegment addToList1(startCoordinate, geoSeg3.start);
									GeoSegment addToList2(geoSeg3.start, endCoordinate);
									doubleSegment firstDoubleSegmentAddToList;
									firstDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance+distanceEarthMiles(curr,geoSeg3.start);
									firstDoubleSegmentAddToList.geoSeg1 = addToList1;
									firstDoubleSegmentAddToList.geoSeg2 = addToList2;
									openList.push(firstDoubleSegmentAddToList);
								}
							}
							if (test2 == nullptr)
							{
								if (coordinate2 == nullptr)
								{
									closedList.associate(geoSeg3.end, geoSeg3.end);

									mapForRetracing->associate(geoSeg3.end, curr);

									GeoSegment addToList3(startCoordinate, geoSeg3.end);
									GeoSegment addToList4(geoSeg3.end, endCoordinate);

									doubleSegment secondDoubleSegmentAddToList;
									secondDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance + distanceEarthMiles(curr, geoSeg3.end);
									secondDoubleSegmentAddToList.geoSeg1 = addToList3;
									secondDoubleSegmentAddToList.geoSeg2 = addToList4;
									openList.push(secondDoubleSegmentAddToList);
								}
							}


						}
						else if (!((*test1 == curr) || *test2 == curr))
						{
							if (coordinate == nullptr&&coordinate2 == nullptr)
							{
								closedList.associate(geoSeg3.start, geoSeg3.start);
								closedList.associate(geoSeg3.end, geoSeg3.end);
								mapForRetracing->associate(geoSeg3.start, curr);
								mapForRetracing->associate(geoSeg3.end, curr);
								GeoSegment addToList1(startCoordinate, geoSeg3.start);
								GeoSegment addToList2(geoSeg3.start, endCoordinate);
								GeoSegment addToList3(startCoordinate, geoSeg3.end);
								GeoSegment addToList4(geoSeg3.end, endCoordinate);
								doubleSegment firstDoubleSegmentAddToList;
								firstDoubleSegmentAddToList.geoSeg1 = addToList1;
								firstDoubleSegmentAddToList.geoSeg2 = addToList2;
								firstDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance + distanceEarthMiles(curr, geoSeg3.start);
								doubleSegment secondDoubleSegmentAddToList;
								secondDoubleSegmentAddToList.geoSeg1 = addToList3;
								secondDoubleSegmentAddToList.geoSeg2 = addToList4;
								secondDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance + distanceEarthMiles(curr, geoSeg3.end);
								openList.push(firstDoubleSegmentAddToList);
								openList.push(secondDoubleSegmentAddToList);
							}
							else if (coordinate == nullptr&&coordinate2 != nullptr)
							{
								closedList.associate(geoSeg3.start, geoSeg3.start);
								mapForRetracing->associate(geoSeg3.start, curr);
								GeoSegment addToList1(startCoordinate, geoSeg3.start);
								GeoSegment addToList2(geoSeg3.start, endCoordinate);
								doubleSegment firstDoubleSegmentAddToList;
								firstDoubleSegmentAddToList.geoSeg1 = addToList1;
								firstDoubleSegmentAddToList.geoSeg2 = addToList2;
								firstDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance + distanceEarthMiles(curr, geoSeg3.start);
								openList.push(firstDoubleSegmentAddToList);
							}
							else if (coordinate != nullptr&&coordinate2 == nullptr)
							{
								closedList.associate(geoSeg3.end, geoSeg3.end);
								mapForRetracing->associate(geoSeg3.end, curr);
								GeoSegment addToList3(startCoordinate, geoSeg3.end);
								GeoSegment addToList4(geoSeg3.end, endCoordinate);
								doubleSegment secondDoubleSegmentAddToList;
								secondDoubleSegmentAddToList.geoSeg1 = addToList3;
								secondDoubleSegmentAddToList.geoSeg2 = addToList4;
								secondDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance + distanceEarthMiles(curr, geoSeg3.end);
								openList.push(secondDoubleSegmentAddToList);
							}
						}
						else
						{
							if (*test1 == curr && !(*test2 == curr))
							{
								mapForRetracing->associate(geoSeg3.end, curr);
								GeoSegment addToList3(startCoordinate, geoSeg3.end);
								GeoSegment addToList4(geoSeg3.end, endCoordinate);
								doubleSegment secondDoubleSegmentAddToList;
								secondDoubleSegmentAddToList.geoSeg1 = addToList3;
								secondDoubleSegmentAddToList.geoSeg2 = addToList4;
								secondDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance + distanceEarthMiles(curr, geoSeg3.end);
								openList.push(secondDoubleSegmentAddToList);

							}
							else if (!(*test1 == curr) && (*test2 == curr))
							{
								mapForRetracing->associate(geoSeg3.start, curr);
								GeoSegment addToList1(startCoordinate, geoSeg3.start);
								GeoSegment addToList2(geoSeg3.start, endCoordinate);
								doubleSegment firstDoubleSegmentAddToList;
								firstDoubleSegmentAddToList.geoSeg1 = addToList1;
								firstDoubleSegmentAddToList.geoSeg2 = addToList2;
								firstDoubleSegmentAddToList.distanceTravelledSoFar = prevDistance + distanceEarthMiles(curr, geoSeg3.start);
								openList.push(firstDoubleSegmentAddToList);

							}
							else
							{

							}

						}

					}

				}
			}
		}


		return NAV_NO_ROUTE;  // This compiles, but may not be correct
	}
}
vector<NavSegment> NavigatorImpl::reconstruct_path(const GeoCoord& start, const GeoCoord& end) const
{
	vector<NavSegment> segs;
	vector<GeoCoord> geoCoords;
	geoCoords.push_back(end);
	GeoCoord temp = end;
	GeoCoord* curr = mapForRetracing->find(end);
	string streetName = "";
	bool prevTurn = false;
	while (curr != nullptr)
	{
		if (streetName != "")
		{
			for (int j = 0; curr!=nullptr&& j < ourStreetSegments.getSegments(*curr).size(); j++)
			{
				if (ourStreetSegments.getSegments(*curr)[j].streetName != streetName)
				{
					GeoCoord* temporary = mapForRetracing->find(*curr);

					if (temporary == nullptr)
					{
						curr = nullptr;
					}
					else
					{
						for (int i = 0; i < ourStreetSegments.getSegments(*temporary).size(); i++)
						{
							string name = ourStreetSegments.getSegments(*temporary)[i].streetName;
							if (ourStreetSegments.getSegments(*temporary)[i].streetName == streetName)
							{
								GeoSegment addToVector(*curr, *temporary);
								NavSegment add(doubleToDirection(angleOfLine(addToVector)), ourStreetSegments.getSegments(*curr)[j].streetName, distanceEarthMiles(*curr, *temporary), addToVector);
								prevTurn = false;
								curr = temporary;
							}
							else if (ourStreetSegments.getSegments(*temporary)[i].streetName == ourStreetSegments.getSegments(*curr)[j].streetName)
							{
								for (int k = 0; k < ourStreetSegments.getSegments(temp).size(); k++)
								{
									if ((ourStreetSegments.getSegments(*curr)[j].segment.start.latitudeText == ourStreetSegments.getSegments(temp)[k].segment.start.latitudeText))
									{
										if ((ourStreetSegments.getSegments(*curr)[j].segment.start.longitudeText == ourStreetSegments.getSegments(temp)[k].segment.start.longitudeText))
										{
											if (!prevTurn)
											{
												NavSegment addTurn(directionTurn(angleBetween2Lines(ourStreetSegments.getSegments(*curr)[j].segment, ourStreetSegments.getSegments(temp)[k].segment)), streetName);
												segs.push_back(addTurn);
												streetName = ourStreetSegments.getSegments(*curr)[j].streetName;
												prevTurn = true;
												temp = *curr;
												curr = mapForRetracing->find(*curr);
												k = ourStreetSegments.getSegments(temp).size();
												i = ourStreetSegments.getSegments(*temporary).size();
												j = ourStreetSegments.getSegments(*curr).size();
											}
										}
									}
									else if ((ourStreetSegments.getSegments(*curr)[j].segment.end.latitudeText == ourStreetSegments.getSegments(temp)[k].segment.start.latitudeText))
									{
										if ((ourStreetSegments.getSegments(*curr)[j].segment.end.longitudeText == ourStreetSegments.getSegments(temp)[k].segment.start.longitudeText))
										{
											if (!prevTurn)
											{
												NavSegment addTurn(directionTurn(angleBetween2Lines(ourStreetSegments.getSegments(*curr)[j].segment, ourStreetSegments.getSegments(temp)[k].segment)), streetName);
												segs.push_back(addTurn);
												streetName = ourStreetSegments.getSegments(*curr)[j].streetName;
												prevTurn = true;
												temp = *curr;
												curr = mapForRetracing->find(*curr);
												k = ourStreetSegments.getSegments(temp).size();
												i = ourStreetSegments.getSegments(*temporary).size();
												j = ourStreetSegments.getSegments(*curr).size();
											}
										}
									}
									else if ((ourStreetSegments.getSegments(*curr)[j].segment.end.latitudeText == ourStreetSegments.getSegments(temp)[k].segment.end.latitudeText))
									{
										if ((ourStreetSegments.getSegments(*curr)[j].segment.end.longitudeText == ourStreetSegments.getSegments(temp)[k].segment.end.longitudeText))
										{
											if (!prevTurn)
											{
												NavSegment addTurn(directionTurn(angleBetween2Lines(ourStreetSegments.getSegments(*curr)[j].segment, ourStreetSegments.getSegments(temp)[k].segment)), streetName);
												segs.push_back(addTurn);
												streetName = ourStreetSegments.getSegments(*curr)[j].streetName;
												prevTurn = true;
												temp = *curr;
												curr = mapForRetracing->find(*curr);
												k = ourStreetSegments.getSegments(temp).size();
												i = ourStreetSegments.getSegments(*temporary).size();
												j = ourStreetSegments.getSegments(*curr).size();
											}
										}
									}
									else if ((ourStreetSegments.getSegments(*curr)[j].segment.start.latitudeText == ourStreetSegments.getSegments(temp)[k].segment.end.latitudeText))
									{
										if ((ourStreetSegments.getSegments(*curr)[j].segment.start.longitudeText == ourStreetSegments.getSegments(temp)[k].segment.end.longitudeText))
										{
											if (!prevTurn)
											{
												NavSegment addTurn(directionTurn(angleBetween2Lines(ourStreetSegments.getSegments(*curr)[j].segment, ourStreetSegments.getSegments(temp)[k].segment)), streetName);
												segs.push_back(addTurn);
												streetName = ourStreetSegments.getSegments(*curr)[j].streetName;
												prevTurn = true;
												temp = *curr;
												curr = mapForRetracing->find(*curr);
												k = ourStreetSegments.getSegments(temp).size();
												i = ourStreetSegments.getSegments(*temporary).size();
												j = ourStreetSegments.getSegments(*curr).size();
											}
										}
									}
									else
									{
										GeoSegment seg = ourStreetSegments.getSegments(*curr)[j].segment;
										GeoSegment addToVector(*curr, temp);
										NavSegment add(doubleToDirection(angleOfLine(addToVector)), ourStreetSegments.getSegments(temp)[k].streetName, distanceEarthMiles(*curr, temp), addToVector);
										prevTurn = false;
										segs.push_back(add);
										streetName = ourStreetSegments.getSegments(temp)[j].streetName;
										temp = *curr;
										curr = mapForRetracing->find(*curr);
										k = ourStreetSegments.getSegments(temp).size();
										i = ourStreetSegments.getSegments(*temporary).size();
										
									}
								}
							}
							else
							{

							}
						}

					}
				}
				else
				{
					if (mapForRetracing->find(*curr) != curr)
					{
						GeoSegment seg = ourStreetSegments.getSegments(*curr)[j].segment;
						GeoSegment addToVector(*curr, temp);
						NavSegment add(doubleToDirection(angleOfLine(addToVector)), ourStreetSegments.getSegments(*curr)[j].streetName, distanceEarthMiles(*curr, temp), addToVector);
						prevTurn = false;
						segs.push_back(add);
						streetName = ourStreetSegments.getSegments(*curr)[j].streetName;
						temp = *curr;
						curr = mapForRetracing->find(*curr);
						j = ourStreetSegments.getSegments(*curr).size();
					}
					else
					{
					
						
							j = ourStreetSegments.getSegments(*curr).size();
							curr = nullptr;
						

					}
				}

			}
		}
		else
		{
			GeoSegment seg = ourStreetSegments.getSegments(*curr)[0].segment;
			GeoSegment addToVector(seg.start, temp);
			GeoSegment addToVector1(temp, seg.start);
			NavSegment add(doubleToDirection(angleOfLine(addToVector1)), ourStreetSegments.getSegments(*curr)[0].streetName, distanceEarthMiles(*curr, temp), addToVector1);
			streetName = ourStreetSegments.getSegments(*curr)[0].streetName;
		
			segs.push_back(add);
			temp = *curr;
			curr = mapForRetracing->find(*curr);
		}

	}
	GeoSegment seg = ourStreetSegments.getSegments(start)[0].segment;
	GeoSegment addToVector(start, temp);
	GeoSegment addToVector1(temp, seg.start);
	
	NavSegment add(doubleToDirection(angleOfLine(addToVector)), ourStreetSegments.getSegments(start)[0].streetName, distanceEarthMiles(start, temp), addToVector);
	segs.push_back(add);
	if (segs.size() != 0)
	{
		reverse(segs.begin(), segs.end());
	}
	return segs;

}
//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
	m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
	delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
	return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
	return m_impl->navigate(start, end, directions);
}
