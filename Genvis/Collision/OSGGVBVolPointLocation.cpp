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
BVolPointLocation<BasicTraits>::ResultT  
BVolPointLocation<BasicTraits>::InnerPointLocation 
(GroupType* node)
{
   if (node->getBoundingVolume().testIntersect(m_local)) {
      m_hit = node;
      return SingleTraverserBase<BasicTraits>::CONTINUE;
   }
   return SingleTraverserBase<BasicTraits>::QUIT;
}
template <class BasicTraits>
BVolPointLocation<BasicTraits>::ResultT  
BVolPointLocation<BasicTraits>::LeafPointLocation  
(AdapterType* prim)
{
   if (prim->getBoundingVolume().testIntersect(m_local)) {
      m_hit = prim;
      return SingleTraverserBase<BasicTraits>::CONTINUE;
   }
   return SingleTraverserBase<BasicTraits>::QUIT;
}

template <class BasicTraits>
BVolPointLocationTraits<BasicTraits>::InitFunctorT  
BVolPointLocationTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   InitFunctorT::InitMethodT f = &(ObjectT::InitPointLocation);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
BVolPointLocationTraits<BasicTraits>::InitFunctorT  
BVolPointLocationTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   InitFunctorT::InitMethodT f = &(ObjectT::LeavePointLocation);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
BVolPointLocationTraits<BasicTraits>::InnerFunctorT 
BVolPointLocationTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerPointLocation);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
BVolPointLocationTraits<BasicTraits>::InnerFunctorT 
BVolPointLocationTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeavePointLocation);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
BVolPointLocationTraits<BasicTraits>::LeafFunctorT  
BVolPointLocationTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafPointLocation);
   return LeafFunctorT(obj, f);
}

#if 0
// explicit traversal instantiations
#include "OSGGVSingleTraverser.cpp"
template class OSG_GENVISLIB_DLLMAPPING SingleTraverser<OpenSGTraits,BVolPointLocationTraits<OpenSGTraits> >;
template class OSG_GENVISLIB_DLLMAPPING SingleEnterLeaveTraverser<OpenSGTraits,BVolPointLocationTraits<OpenSGTraits> >;
#endif
