//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#include "OSGGVNodeTraverser.h"

OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING NodeAllTraverser<OpenSGTraits>;

template <class BasicTraits>
bool NodeAllTraverser<BasicTraits>::apply   
(const OSG::NodePtr& node0, const OSG::NodePtr& node1)
{
   getDataTyped().getContacts().clear();
   getDataTyped().getNumBVolTests() = 0;
   CacheData& data0 = getCache()[node0];
   CacheData& data1 = getCache()[node1];
   u32 i;

   DynamicVolume volume0;
   std::vector<DynamicVolume> volume1; 
   volume1.resize(data1.getAdapter(ObjectAdapterType::getAdapterId()).size());
   CacheData::AdapterContainer::const_iterator it1;
   for (it1=data1.getAdapter(ObjectAdapterType::getAdapterId()).begin(), i=0;
	it1 != data1.getAdapter(ObjectAdapterType::getAdapterId()).end();
	++it1, ++i) {
      ObjectAdapterType* node1 = static_cast<ObjectAdapterType*>(*it1);
      node1->getOriginal()->getWorldVolume(volume1[i]);
   }

   bool result = false;
   CacheData::AdapterContainer::const_iterator it0;
   for (it0 = data0.getAdapter(ObjectAdapterType::getAdapterId()).begin();
	it0 != data0.getAdapter(ObjectAdapterType::getAdapterId()).end();
	++it0) {
      ObjectAdapterType* node0 = static_cast<ObjectAdapterType*>(*it0);
      node0->getOriginal()->getWorldVolume(volume0);
      for (it1=data1.getAdapter(ObjectAdapterType::getAdapterId()).begin(), i=0;
	   it1 != data1.getAdapter(ObjectAdapterType::getAdapterId()).end();
	   ++it1, ++i) {
	 ObjectAdapterType* node1 = static_cast<ObjectAdapterType*>(*it1);
	 ++getDataTyped().getNumBVolTests();
	 if (volume0.intersect(volume1[i])) {
	    // reporting this collision
 	    getDataTyped().getContacts().push_back(CollisionPair(node0, node1));
	    result = true;
	 }
      }
   }
   return result;
}
