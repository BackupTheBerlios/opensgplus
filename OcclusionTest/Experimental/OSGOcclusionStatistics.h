// FIXME
// Use OpenSG statistics features...............

#ifndef _OSGOCCLUSIONSTATISTICS_H

#define _OSGOCCLUSIONSTATISTICS_H 1

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE

class OcclusionStatistics{
	public:
	enum { _visiblePixels=0, _visibleNodes=1, _tests=2, _last=3 };
};

class OcclusionStatisticsOn{
	private:
	UInt16 _statistics[OcclusionStatistics::_last];
	
	public:
	inline void add(const UInt16 s, const UInt16 v){ _statistics[s]+=v; };
	inline void clear(void){ for(int i=0; i<OcclusionStatistics::_last; i++) _statistics[i]=0; };
	inline void print(void){ for(int i=0; i<OcclusionStatistics::_last; i++) cerr << i << ": " << _statistics[i] << endl; };
};

class OcclusionStatisticsOff{
	public:
	inline void add(const UInt16 s, const UInt16 v){ };
	inline void clear(void){ };
	inline void print(void){ };
};

OSG_END_NAMESPACE

#endif
