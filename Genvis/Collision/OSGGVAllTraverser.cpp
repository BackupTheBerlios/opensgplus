//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.2 $
//   $Date: 2004/03/12 13:18:34 $
//                                                                            
//=============================================================================

#include "OSGGVAllTraverser.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVProfiler.h"

USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING AllTraverserBase<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING AllTraverser<OpenSGTraits>;

template <class BasicTraits>
bool AllTraverser<BasicTraits>::apply (const GeomObjectType& node0, const GeomObjectType& node1)
{
   CacheData& data0 = Cache::the()[node0];
   CacheData& data1 = Cache::the()[node1];

   Profiler::the().StartProfile("AllTraverser.Init");
   getData().Init();
   Profiler::the().EndProfile("AllTraverser.Init");

   Profiler::the().StartProfile("AllTraverser.Loop");
   typename CacheData::AdapterContainer::const_iterator it0;
   typename CacheData::AdapterContainer::const_iterator it1;
#if 0
   // compute to world matrices once
   std::vector<TransformType> all1; all1.reserve(data1.getAdapter(BVolAdapterBase::getAdapterId()).size());
   for (it1 = data1.getAdapter(BVolAdapterBase::getAdapterId()).begin();
	it1 != data1.getAdapter(BVolAdapterBase::getAdapterId()).end();
	++it1) {
	 BVolAdapterBase* second = static_cast<BVolAdapterBase*>(*it1);
	 CacheData&       d1 = getCacheData(data1, second);
	 all1.push_back(d1.getToWorldMatrix());
   }
#endif
   // collision loop
   u32 i;
   bool result = false;
   for (it0 = data0.getAdapter(BVolAdapterBase::getAdapterId()).begin();
	it0 != data0.getAdapter(BVolAdapterBase::getAdapterId()).end();
	++it0) {
      Profiler::the().StartProfile("AllTraverser.Prepare");
      Profiler::the().StartProfile("AllTraverser.GetCacheData");
      BVolAdapterBase* first = static_cast<BVolAdapterBase*>(*it0);
      CacheData&    d0 = getCacheData(data0, first);
      Profiler::the().EndProfile("AllTraverser.GetCacheData");
      Profiler::the().StartProfile("AllTraverser.WorldMatrix");
      TransformType m0 = d0.getToWorldMatrix();
      Profiler::the().EndProfile("AllTraverser.WorldMatrix");
      Profiler::the().EndProfile("AllTraverser.Prepare");

      for (it1 = data1.getAdapter(BVolAdapterBase::getAdapterId()).begin(), i=0;
	   it1 != data1.getAdapter(BVolAdapterBase::getAdapterId()).end();
	   ++it1, ++i) {
	 Profiler::the().StartProfile("AllTraverser.Prepare");
	 Profiler::the().StartProfile("AllTraverser.GetCacheData");
	 BVolAdapterBase* second = static_cast<BVolAdapterBase*>(*it1);
	 CacheData&     d1 = getCacheData(data1, second);
	 Profiler::the().EndProfile("AllTraverser.GetCacheData");
	 Profiler::the().StartProfile("AllTraverser.WorldMatrix");
	 TransformType m1 = d1.getToWorldMatrix();
	 //const TransformType& m1 = all1[i];
	 Profiler::the().EndProfile("AllTraverser.WorldMatrix");
	 Profiler::the().EndProfile("AllTraverser.Prepare");
	 
	 Profiler::the().StartProfile("AllTraverser.TestSameNode");
	 bool same = (d0.getNode() == d1.getNode());
	 Profiler::the().EndProfile("AllTraverser.TestSameNode");
	 if (same) {
	    continue;
	 }

	 Profiler::the().StartProfile("AllTraverser.DoubleTraverser");
	 bool thisResult = getDoubleTraverser().apply(data0, d0, first,  
						      data1, d1, second, 
						      m0, m1);
	 Profiler::the().EndProfile("AllTraverser.DoubleTraverser");
	 result |= thisResult;
      }
   }
   Profiler::the().EndProfile("AllTraverser.Loop");
   return result;
}
