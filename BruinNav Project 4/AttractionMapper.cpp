#include "provided.h"
#include <string>
#include "MyMap.h"
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> attractions;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	for (int i = 0; i < ml.getNumSegments(); i++) {
		StreetSegment temp;
		ml.getSegment(i, temp);
		if (temp.attractions.size() > 0) {
			for (int j = 0; j < temp.attractions.size(); j++) {
				string name;
				for (int k = 0; k < temp.attractions[j].name.size(); k++) {
					name += tolower(temp.attractions[j].name[k]);
				}
				attractions.associate(name, temp.attractions[j].geocoordinates);
			}
		}
	}
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	string standard;
	for (int i = 0; i < attraction.size(); i++) {
		standard += tolower(attraction[i]);
	}
	const GeoCoord* temp = attractions.find(standard);
	if (temp == nullptr) {
		return false;
	}
	gc = *temp;
	return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
