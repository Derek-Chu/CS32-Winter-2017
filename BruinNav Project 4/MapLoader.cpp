#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	vector<StreetSegment*> segments;
	size_t m_size;
};

MapLoaderImpl::MapLoaderImpl()
{
	m_size = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
	for (int i = 0; i < segments.size(); i++) {
		delete segments[i];
	}
	segments.clear();
}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream infile(mapFile);
	if ( ! infile )		        
	{
		return false;
	}
	string street;
	string coords = "";
	int c = 0;
	while (getline(infile, street)) {
		string startcoord1 = "";
		string startcoord2 = "";
		string endcoord1 = "";
		string endcoord2 = "";
		getline(infile, coords);
		int i = 0;
		while (isdigit(coords[i]) || coords[i] == '.' || coords[i] == '-') {
			startcoord1 += coords[i];
			i++;
		}
		i += 2;
		while (isdigit(coords[i]) || coords[i] == '.' || coords[i] == '-') {
			startcoord2 += coords[i];
			i++;
		}
		i++;
		while (isdigit(coords[i]) || coords[i] == '.' || coords[i] == '-') {
			endcoord1 += coords[i];
			i++;
		}
		i++;
		while (i < coords.size()) {
			endcoord2 += coords[i];
			i++;
		}
		GeoCoord start(startcoord1, startcoord2);
		GeoCoord end(endcoord1, endcoord2);
		GeoSegment segment(start, end);
		
		infile >> c;
		infile.ignore(10000, '\n');

		vector<Attraction> attractions;
		for (int j = 0; j < c; j++) {
			string att = "";
			string attractionName = "";
			string coord1 = "";
			string coord2 = "";
			getline(infile, att);
			int k = 0;
			while (att[k] != '|') {
				attractionName += att[k];
				k++;
			}
			k++;
			while (isdigit(att[k]) || att[k] == '.' || att[k] == '-') {
				coord1 += att[k];
				k++;
			}
			k += 2;
			while (k < att.size()) {
				coord2 += att[k];
				k++;
			}
			Attraction temp;
			temp.name = attractionName;
			GeoCoord coord(coord1, coord2);
			temp.geocoordinates = coord;
			attractions.push_back(temp);
		}
		StreetSegment* newSegment = new StreetSegment;
		newSegment->streetName = street;
		newSegment->segment = segment;
		newSegment->attractions = attractions;
		segments.push_back(newSegment);
		m_size++;
	}
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_size;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < 0 || segNum >= segments.size()) {
		return false;
	}
	seg = *(segments[segNum]);
	return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}

