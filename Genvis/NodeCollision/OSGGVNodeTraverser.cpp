//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/03/12 13:27:49 $
//                                                                            
//=============================================================================

#include "OSGGVNodeTraverser.h"
#include "OSGGVProfiler.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING NodeAllTraverser<OpenSGTraits>;

template <class BasicTraits>
bool NodeAllTraverser<BasicTraits>::apply   
(const OSG::NodePtr& node0, const OSG::NodePtr& node1)
{
   Profiler::the().StartProfile("NodeAllTraverser.Init");
   getDataTyped().getContacts().clear();
   getDataTyped().getNumBVolTests() = 0;
   Profiler::the().EndProfile("NodeAllTraverser.Init");

   Profiler::the().StartProfile("NodeAllTraverser.Loop");
   Profiler::the().StartProfile("NodeAllTraverser.Prepare");
   DynamicVolume volume0;

   CacheData& data0 = Cache::the()[node0];
   CacheData& data1 = Cache::the()[node1];
   std::vector<DynamicVolume> volume1; 
   volume1.resize(data1.getAdapter(AdapterType::getAdapterId()).size());
   u32 i;
   typename CacheData::AdapterContainer::const_iterator it1;
   for (it1=data1.getAdapter(AdapterType::getAdapterId()).begin(), i=0;
	it1 != data1.getAdapter(AdapterType::getAdapterId()).end();
	++it1, ++i) {
      AdapterType* node1 = static_cast<AdapterType*>(*it1);
      node1->getOriginal()->getWorldVolume(volume1[i]);
   }
   Profiler::the().EndProfile  ("NodeAllTraverser.Prepare");

   bool result = false;
   typename CacheData::AdapterContainer::const_iterator it0;
   for (it0 = data0.getAdapter(AdapterType::getAdapterId()).begin();
	it0 != data0.getAdapter(AdapterType::getAdapterId()).end();
	++it0) {
      Profiler::the().StartProfile("NodeAllTraverser.Prepare");
      AdapterType* node0 = static_cast<AdapterType*>(*it0);
      node0->getOriginal()->getWorldVolume(volume0);
      Profiler::the().EndProfile  ("NodeAllTraverser.Prepare");

      for (it1=data1.getAdapter(AdapterType::getAdapterId()).begin(), i=0;
	   it1 != data1.getAdapter(AdapterType::getAdapterId()).end();
	   ++it1, ++i) {
	 AdapterType* node1 = static_cast<AdapterType*>(*it1);

	 Profiler::the().StartProfile("NodeAllTraverser.TestSameNode");
	 bool same = (node0->getOriginal() == node1->getOriginal());
	 Profiler::the().EndProfile("NodeAllTraverser.TestSameNode");
	 if (same) {
	    continue;
	 }

	 Profiler::the().StartProfile("NodeAllTraverser.OSGDynamicVolumeCollide");
	 ++getDataTyped().getNumBVolTests();
	 if (volume0.intersect(volume1[i])) {
	    // reporting this collision
 	    getDataTyped().getContacts().push_back(CollisionPair(node0, node1));
	    result = true;
	 }
	 Profiler::the().EndProfile  ("NodeAllTraverser.OSGDynamicVolumeCollide");
      }
   }
   Profiler::the().EndProfile("NodeAllTraverser.Loop");
   return result;
}
