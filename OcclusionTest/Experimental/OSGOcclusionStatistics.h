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
	bool _visualize;
	
	public:
	OcclusionStatisticsOn():_visualize(false){ };
	inline void add(const UInt16 s, const UInt16 v){ _statistics[s]+=v; };
	inline void clear(void){ for(int i=0; i<OcclusionStatistics::_last; i++) _statistics[i]=0; };
	inline void print(void){ for(int i=0; i<OcclusionStatistics::_last; i++) std::cerr << i << ": " << _statistics[i] << std::endl; };
	inline const bool statistics(void){ return(true); };
	inline const bool visualize(void){ return(_visualize); };
	inline void setVisualize(bool b){ _visualize=b; };
};

class OcclusionStatisticsOff{
	public:
	inline void add(const UInt16 s, const UInt16 v){ };
	inline void clear(void){ };
	inline void print(void){ };
	inline const bool statistics(void){ return(false); };
	inline const bool visualize(void){ return(false); };
	inline void setVisualize(bool){ };
};

OSG_END_NAMESPACE

#endif
