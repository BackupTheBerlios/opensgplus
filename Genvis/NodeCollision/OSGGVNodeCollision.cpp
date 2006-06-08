//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2006/06/08 17:03:30 $
//                                                                            
//=============================================================================

#include "OSGGVNodeCollision.h"
OSG_USING_NAMESPACE
USING_GENVIS_NAMESPACE

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits>
bool NodeCollision<BasicTraits>::Init ()
{
   // clears getContacts()
   bool result = Inherited::Init();
#if 0
   // precomputation of world-volumes into map
   m_volume.clear();
   DynamicVolume volume;
   typename CacheData::AdapterContainer::const_iterator it;
   for (it=data0.getAdapter(AdapterType::getAdapterId()).begin();
	it != data0.getAdapter(AdapterType::getAdapterId()).end();
	++it) {
      AdapterType* node = static_cast<AdapterType*>(*it);
      node->getOriginal()->getWorldVolume(volume);
      m_volume[node] = volume;
   }
   for (it=data1.getAdapter(AdapterType::getAdapterId()).begin();
	it != data1.getAdapter(AdapterType::getAdapterId()).end();
	++it) {
      AdapterType* node = static_cast<AdapterType*>(*it);
      node->getOriginal()->getWorldVolume(volume);
      m_volume[node] = volume;
   }
#endif
   return result;
}

template <class BasicTraits>
bool NodeCollision<BasicTraits>::InitDouble  
(GroupType* root0, const TransformType& m0, const TransformType& f0, 
 GroupType* root1, const TransformType& m1, const TransformType& f1)
{
   return true;
}

template <class BasicTraits>
bool NodeCollision<BasicTraits>::LeaveDouble 
(GroupType* root0, const TransformType& m0, const TransformType& f0, 
 GroupType* root1, const TransformType& m1, const TransformType& f1)
{
   return true;
}

template <class BasicTraits>
void NodeCollision<BasicTraits>::dump (std::ostream& os)
{
   os << "NodeCollision" << std::flush;
}

template <class BasicTraits>
typename NodeCollision<BasicTraits>::ResultT 
NodeCollision<BasicTraits>::BVolBVolCollision (GroupType* node0, GroupType* node1)
{
  if (!getStopFirst() || getNumContacts() == 0) {
      ++getNumBVolTests();

#if 1
      DynamicVolume volume0, volume1;
      node0->getOriginal()->getWorldVolume(volume0);
      node1->getOriginal()->getWorldVolume(volume1);
#else
      DynamicVolume& volume0 = m_volume[node0];
      DynamicVolume& volume1 = m_volume[node1];
#endif
 
      if (volume0.intersect(volume1)) {
	getContacts().push_back(CollisionPair(node0, node1));
	return DoubleTraverserBase<BasicTraits>::CONTINUE;;
      }
   }
   return DoubleTraverserBase<BasicTraits>::QUIT;
}

template <class BasicTraits>
bool NodeCollision<BasicTraits>::apply 
(const GeomObjectType& node0, const GeomObjectType& node1)
{
   CacheData& data0 = Cache::the()[node0];
   CacheData& data1 = Cache::the()[node1];

   AdapterType* root0 = (AdapterType*)(*data0.getAdapter(AdapterType::getAdapterId()).begin());
   AdapterType* root1 = (AdapterType*)(*data1.getAdapter(AdapterType::getAdapterId()).begin());
   assert(root0 != NULL);
   assert(root1 != NULL);

   TransformType m0;
   TransformType m1; 
#if 1
   if (data0.getAdapterMatrix(AdapterType::getAdapterId()) == TransformType::identity()) {
      m0.setValue(data0.getToWorldMatrix());
   } else {
      m0.invertFrom(data0.getAdapterMatrix(AdapterType::getAdapterId()));
      m0.multLeft(data0.getToWorldMatrix());
   }
   if (data1.getAdapterMatrix(AdapterType::getAdapterId()) == TransformType::identity()) {
      m1.setValue(data1.getToWorldMatrix());
   } else {
      m1.invertFrom(data1.getAdapterMatrix(AdapterType::getAdapterId()));
      m1.multLeft(data1.getToWorldMatrix());
   }
#endif
   if (InitDouble(root0, m0, data0.getFrameMatrix(), 
		  root1, m1, data1.getFrameMatrix())) {
      BVolBVolCollision (root0, root1);
      return true;
   }
   return false;
}
template <class BasicTraits>
bool NodeCollision<BasicTraits>::apply 
(CacheData& parent0, CacheData& data0, BVolAdapterBase* node0, 
 CacheData& parent1, CacheData& data1, BVolAdapterBase* node1, 
 const TransformType& m0, const TransformType& m1)
{
   AdapterType* root0 = (AdapterType*)(node0);
   AdapterType* root1 = (AdapterType*)(node1);
   if (InitDouble(root0, m0, parent0.getFrameMatrix(), 
		  root1, m1, parent1.getFrameMatrix())) {
      BVolBVolCollision (root0, root1);
      return true;
   }
   return false;
}


template <class BasicTraits>
typename NodeCollisionTraits<BasicTraits>::InitFunctorT 
NodeCollisionTraits<BasicTraits>::createInitFunctor (ObjectT* obj) 
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename NodeCollisionTraits<BasicTraits>::InitDoubleFunctorT 
NodeCollisionTraits<BasicTraits>::createInitDoubleFunctor (ObjectT* obj) 
{
   typename InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits>
typename NodeCollisionTraits<BasicTraits>::LeaveDoubleFunctorT 
NodeCollisionTraits<BasicTraits>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   typename LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits>
typename NodeCollisionTraits<BasicTraits>::BVolBVolFunctorT 
NodeCollisionTraits<BasicTraits>::createBVolBVolFunctor (ObjectT* obj) 
{
   typename BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING NodeCollision<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING NodeCollisionTraits<OpenSGTraits>;

END_GENVIS_NAMESPACE
