#include "provided.h"
#include <vector>
#include "MyMap.h"
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<GeoCoord, vector<StreetSegment>> segs;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment seg;
        ml.getSegment(i, seg);
        GeoSegment geoSeg = seg.segment;
        GeoCoord start = geoSeg.start;
        GeoCoord end = geoSeg.end;
        vector<StreetSegment>* map = segs.find(start);
        if (map != nullptr)
        {
            map->push_back(seg);
        }
        else
        {
            vector<StreetSegment> insert;
            insert.push_back(seg);
            segs.associate(start, insert);
        }
        map = segs.find(end);
        if (map != nullptr)
        {
            map->push_back(seg);
        }
        else
        {
            vector<StreetSegment> insert;
            insert.push_back(seg);
            segs.associate(end, insert);
        }
        vector<Attraction> attr = seg.attractions;
        for (int j = 0; j < attr.size(); j++)
        {
            map = segs.find(attr[j].geocoordinates);
            if (map != nullptr)
            {
                map->push_back(seg);
            }
            else
            {
                vector<StreetSegment> insert;
                insert.push_back(seg);
                segs.associate(attr[j].geocoordinates, insert);
            }
        }
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	vector<StreetSegment>* segments;
	segments = const_cast<vector<StreetSegment>*>(segs.find(gc));
	return *segments;  
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
