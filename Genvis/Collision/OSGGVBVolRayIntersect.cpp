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

#include "OSGGVBVolRayIntersect.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING BVolRayIntersectBase<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING BVolRayIntersect<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING BVolRayIntersectTraits<OpenSGTraits>;


template <class BasicTraits>
bool BVolRayIntersect<BasicTraits>::InitRayIntersect  (GeneralType*,
						       const TransformType& m)
{
   // transform ray object to local coord system
   TransformType inv; inv.invertFrom(m);
   inv.mult(getIntersect().getRay().getOrigin());
   inv.mult(getIntersect().getRay().getDirection());
   Real scale = getIntersect().getRay().getDirection().length();
   // set local intersection object
   m_local.init(getIntersect().getRay(),
		getIntersect().getFrom(),
	        getIntersect().getDist());
   return true;
}
template <class BasicTraits>
bool BVolRayIntersect<BasicTraits>::LeaveRayIntersect  (GeneralType*,
							const TransformType& m)
{
   Real scale = getIntersect().getRay().getDirection().length();
   m.mult(getIntersect().getRay().getOrigin());
   m.mult(getIntersect().getRay().getDirection());
   if (m_local.getTo() != NULL) {
      // update intersection distance
      if (getIntersect().updateDist(m_local.getDist())) {
	 getIntersect().setTo(m_local.getTo());
	 getIntersect().setData(m_local.getData());
      }
   }
   return true;
}
template <class BasicTraits>
typename BVolRayIntersect<BasicTraits>::ResultT  
BVolRayIntersect<BasicTraits>::InnerRayIntersect (GroupType* node)
{
   if (node->checkIntersect(m_local)) {
      return SingleTraverserBase<BasicTraits>::CONTINUE;
   }
   return SingleTraverserBase<BasicTraits>::QUIT;
}
template <class BasicTraits>
typename BVolRayIntersect<BasicTraits>::ResultT  
BVolRayIntersect<BasicTraits>::LeafRayIntersect  (AdapterType* prim)
{
   if (prim->checkIntersect(m_local)) {
      if (prim->calcIntersect(m_local)) {
	 return SingleTraverserBase<BasicTraits>::QUIT;
      }
   }
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits>
typename BVolRayIntersectTraits<BasicTraits>::InitFunctorT  
BVolRayIntersectTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitRayIntersect);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolRayIntersectTraits<BasicTraits>::InitFunctorT  
BVolRayIntersectTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::LeaveRayIntersect);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolRayIntersectTraits<BasicTraits>::InnerFunctorT 
BVolRayIntersectTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerRayIntersect);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolRayIntersectTraits<BasicTraits>::InnerFunctorT 
BVolRayIntersectTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeaveRayIntersect);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolRayIntersectTraits<BasicTraits>::LeafFunctorT  
BVolRayIntersectTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   typename LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafRayIntersect);
   return LeafFunctorT(obj, f);
}

