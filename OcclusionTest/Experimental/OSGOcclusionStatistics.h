// FIXME
// Use OpenSG statistics features...............

#ifndef _OSGOCCLUSIONSTATISTICS_H

#define _OSGOCCLUSIONSTATISTICS_H 1

#include<OpenSG/OSGConfig.h>

OSG_BEGIN_NAMESPACE

class OcclusionStatistics{
	public:
	enum { _visiblePixels=0, _visibleNodes=1, _tests=2,
	       _HPTests=3, _NVTests=4, _NVMultipleTests=5, _StencilTests=6, _ZBTests=7,
	       _visibleNodesHPTest=8, _visibleNodesNVTest=9, _visibleNodesNVMultipleTest=10, _visibleNodesStencilTest=11, _visibleNodesZBTest=12,
	       _SoftZTests=13, _visibleNodesSoftZTest=14,
	       _shadowTests=15, _visibleNodesShadowTest=16, _last=17 };
};

class OcclusionStatisticsOn{
	private:
	UInt16 _statistics[OcclusionStatistics::_last];
	
	public:
	inline void add(const UInt16 s, const UInt16 v){ _statistics[s]+=v; };
	inline void clear(void){ for(int i=0; i<OcclusionStatistics::numStatistics; i++) _statistics[i]=0; };
};

class OcclusionStatisticsOff{
	public:
	inline void add(const UInt16 s, const UInt16 v){ };
	inline void clear(void){ };
};

OSG_END_NAMESPACE

#endif
