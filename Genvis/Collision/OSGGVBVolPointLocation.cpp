//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:57:44 $
//                                                                            
//=============================================================================

#include "OSGGVBVolPointLocation.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING BVolPointLocationBase<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING BVolPointLocation<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING BVolPointLocationTraits<OpenSGTraits>;


template <class BasicTraits>
bool BVolPointLocation<BasicTraits>::InitPointLocation  (GeneralType*, 
							 const TransformType& m)
{
   m_hit = NULL;
   TransformType inv; inv.invertFrom(m);
   inv.mult(getPoint(), m_local);
   return true;
}
template <class BasicTraits>
bool BVolPointLocation<BasicTraits>::LeavePointLocation (GeneralType*, 
							 const TransformType&)
{
   return true;
}
template <class BasicTraits>
typename BVolPointLocation<BasicTraits>::ResultT  
BVolPointLocation<BasicTraits>::InnerPointLocation (GroupType* node)
{
   if (node->getBoundingVolume().checkIntersect(m_local)) {
      m_hit = node;
      return SingleTraverserBase<BasicTraits>::CONTINUE;
   }
   return SingleTraverserBase<BasicTraits>::QUIT;
}
template <class BasicTraits>
typename BVolPointLocation<BasicTraits>::ResultT  
BVolPointLocation<BasicTraits>::LeafPointLocation  (AdapterType* prim)
{
   if (prim->getBoundingVolume().checkIntersect(m_local)) {
      m_hit = prim;
      return SingleTraverserBase<BasicTraits>::CONTINUE;
   }
   return SingleTraverserBase<BasicTraits>::QUIT;
}

template <class BasicTraits>
typename BVolPointLocationTraits<BasicTraits>::InitFunctorT  
BVolPointLocationTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitPointLocation);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolPointLocationTraits<BasicTraits>::InitFunctorT  
BVolPointLocationTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::LeavePointLocation);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolPointLocationTraits<BasicTraits>::InnerFunctorT 
BVolPointLocationTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerPointLocation);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolPointLocationTraits<BasicTraits>::InnerFunctorT 
BVolPointLocationTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeavePointLocation);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename BVolPointLocationTraits<BasicTraits>::LeafFunctorT  
BVolPointLocationTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   typename LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafPointLocation);
   return LeafFunctorT(obj, f);
}

