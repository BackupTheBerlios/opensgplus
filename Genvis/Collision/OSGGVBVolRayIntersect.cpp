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
		getIntersect().getDist()*scale);
   return true;
}
template <class BasicTraits>
bool BVolRayIntersect<BasicTraits>::LeaveRayIntersect  (GeneralType*         a,
							const TransformType& m)
{
   // transform ray object to world coord system
   Real scale = getIntersect().getRay().getDirection().length();
   m.mult(getIntersect().getRay().getOrigin());
   m.mult(getIntersect().getRay().getDirection());
   if (m_local.getTo() != NULL) {
      // update intersection distance
      if (getIntersect().updateDist(m_local.getDist()/scale)) {
	 getIntersect().setTo(m_local.getTo());
	 getIntersect().setData(m_local.getData());
      }
   }
   return true;
}
template <class BasicTraits>
BVolRayIntersect<BasicTraits>::ResultT  
BVolRayIntersect<BasicTraits>::InnerRayIntersect 
(GroupType* node)
{
   if (node->checkIntersect(getRay())) {
      return SingleTraverserBase<BasicTraits>::CONTINUE;
   }
   return SingleTraverserBase<BasicTraits>::QUIT;
}
template <class BasicTraits>
BVolRayIntersect<BasicTraits>::ResultT  
BVolRayIntersect<BasicTraits>::LeafRayIntersect  
(AdapterType* prim)
{
   if (prim->checkIntersect(getRay())) {
      if (prim->calcIntersect(m_local)) {
	 return SingleTraverserBase<BasicTraits>::QUIT;
      }
   }
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits>
BVolRayIntersectTraits<BasicTraits>::InitFunctorT  
BVolRayIntersectTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   InitFunctorT::InitMethodT f = &(ObjectT::InitRayIntersect);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
BVolRayIntersectTraits<BasicTraits>::InitFunctorT  
BVolRayIntersectTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   InitFunctorT::InitMethodT f = &(ObjectT::LeaveRayIntersect);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
BVolRayIntersectTraits<BasicTraits>::InnerFunctorT 
BVolRayIntersectTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerRayIntersect);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
BVolRayIntersectTraits<BasicTraits>::InnerFunctorT 
BVolRayIntersectTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeaveRayIntersect);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
BVolRayIntersectTraits<BasicTraits>::LeafFunctorT  
BVolRayIntersectTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafRayIntersect);
   return LeafFunctorT(obj, f);
}

#if 0
// explicit template instantiations
#include "OSGGVSingleTraverser.cpp"
template class OSG_GENVISLIB_DLLMAPPING SingleTraverser<OpenSGTraits,BVolRayIntersectTraits<OpenSGTraits> >;
template class OSG_GENVISLIB_DLLMAPPING SingleEnterLeaveTraverser<OpenSGTraits,BVolRayIntersectTraits<OpenSGTraits> >;
#endif
