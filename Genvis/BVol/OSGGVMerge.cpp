//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/20 15:57:11 $
//                                                                            
//=============================================================================

#include "OSGGVMerge.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING Merge<OpenSGTraits>;
template class OSG_GENVISLIB_DLLMAPPING MergeTraits<OpenSGTraits>;

template <class BasicTraits>
bool     Merge<BasicTraits>::InitMerge  (GeneralType*, const TransformType&)
{
  if (getNumLevels() == 0) { // nothing to do
      return false;
   }
   assert(m_addStack.empty());
   m_current = getNumLevels();
   return true;
}
template <class BasicTraits>
bool     Merge<BasicTraits>::LeaveMerge  (GeneralType*, const TransformType&)
{
   return true;
}
template <class BasicTraits>
typename Merge<BasicTraits>::ResultT  
Merge<BasicTraits>::InnerMerge (GroupType* scene)
{
   if (m_current < getNumLevels()) { 
      ++m_current;
      return SingleTraverserBase<BasicTraits>::CONTINUE;
   }
   if (!m_addStack.empty()) {
      m_addStack.top().push_back(scene);
   }
   m_current = 0;
   m_addStack.push(AddList());
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits>
typename Merge<BasicTraits>::ResultT  
Merge<BasicTraits>::InnerLeaveMerge (GroupType* scene)
{
   if (m_current == 0) {
      assert(!m_addStack.empty());
      scene->getSons().swap(m_addStack.top()); 
      m_addStack.pop();
      m_current = getNumLevels();
   } else {
      --m_current;
   }
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits>
typename Merge<BasicTraits>::ResultT  
Merge<BasicTraits>::LeafMerge  (AdapterType* p)
{
   m_addStack.top().push_back(p);
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}


template <class BasicTraits>
typename MergeTraits<BasicTraits>::InitFunctorT  
MergeTraits<BasicTraits>::createInitFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::InitMerge);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename MergeTraits<BasicTraits>::InitFunctorT  
MergeTraits<BasicTraits>::createInitLeaveFunctor  (ObjectT* obj)
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::LeaveMerge);
   return InitFunctorT(obj, f);
}
template <class BasicTraits>
typename MergeTraits<BasicTraits>::InnerFunctorT 
MergeTraits<BasicTraits>::createInnerFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerMerge);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename MergeTraits<BasicTraits>::InnerFunctorT 
MergeTraits<BasicTraits>::createInnerLeaveFunctor (ObjectT* obj)
{
   typename InnerFunctorT::DispatchMethodT f = &(ObjectT::InnerLeaveMerge);
   return InnerFunctorT(obj, f);
}
template <class BasicTraits>
typename MergeTraits<BasicTraits>::LeafFunctorT  
MergeTraits<BasicTraits>::createLeafFunctor  (ObjectT* obj)
{
   typename LeafFunctorT::DispatchMethodT f = &(ObjectT::LeafMerge);
   return LeafFunctorT(obj, f);
}
