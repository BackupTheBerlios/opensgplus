//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:29 $
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
bool AllTraverser<BasicTraits>::apply (const OSG::NodePtr& node0, const OSG::NodePtr& node1)
{
   CacheData& data0 = getCache()[node0];
   CacheData& data1 = getCache()[node1];

   Profiler::the().StartProfile("AllTraverser.Init");
   getData().Init();
   Profiler::the().EndProfile("AllTraverser.Init");

   Profiler::the().StartProfile("AllTraverser.Loop");
   bool result = false;
   for (typename CacheData::AdapterContainer::const_iterator 
        it0 = data0.getAdapter(BVolAdapterBase::getAdapterId()).begin();
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

      for (typename CacheData::AdapterContainer::const_iterator 
	   it1 = data1.getAdapter(BVolAdapterBase::getAdapterId()).begin();
	   it1 != data1.getAdapter(BVolAdapterBase::getAdapterId()).end();
	   ++it1) {
	 Profiler::the().StartProfile("AllTraverser.Prepare");
	 Profiler::the().StartProfile("AllTraverser.GetCacheData");
	 BVolAdapterBase* second = static_cast<BVolAdapterBase*>(*it1);
	 CacheData&     d1 = getCacheData(data1, second);
	 Profiler::the().EndProfile("AllTraverser.GetCacheData");
	 Profiler::the().StartProfile("AllTraverser.WorldMatrix");
	 TransformType m1 = d1.getToWorldMatrix();
	 Profiler::the().EndProfile("AllTraverser.WorldMatrix");
	 Profiler::the().EndProfile("AllTraverser.Prepare");
	 
	 Profiler::the().StartProfile("AllTraverser.TestSameNode");
	 bool same = (d0.getNode() == d1.getNode());
	 Profiler::the().EndProfile("AllTraverser.TestSameNode");
	 if (same) {
	    continue;
	 }

	 Profiler::the().StartProfile("AllTraverser.DoubleTraverser");
	 bool thisResult = getDoubleTraverser().apply(d0, first,  m0, 
						      d1, second, m1);
	 Profiler::the().EndProfile("AllTraverser.DoubleTraverser");
	 result |= thisResult;
      }
   }
   Profiler::the().EndProfile("AllTraverser.Loop");
   return result;
}
