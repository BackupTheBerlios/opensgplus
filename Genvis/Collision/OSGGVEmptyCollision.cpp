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

#include "OSGGVEmptyCollision.h"
#include "OSGGVReal.h"
#include "OSGGVGroup.h"
#include "OSGGVGeomFunctions.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE
OSG_USING_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING EmptyCollision<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollision<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollision<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollision<OpenSGTraits,K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollision<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollision<OpenSGTraits,K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING EmptyCollisionTraits<OpenSGTraits,K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollisionTraits<OpenSGTraits,K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollisionTraits<OpenSGTraits,K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollisionTraits<OpenSGTraits,K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollisionTraits<OpenSGTraits,K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING EmptyCollisionTraits<OpenSGTraits,K20Dop>;


template <class BasicTraits, class BVOL>
typename EmptyCollisionTraits<BasicTraits,BVOL>::InitFunctorT 
EmptyCollisionTraits<BasicTraits,BVOL>::createInitFunctor (ObjectT* obj) 
{
   typename InitFunctorT::InitMethodT f = &(ObjectT::Init);
   return InitFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename EmptyCollisionTraits<BasicTraits,BVOL>::InitDoubleFunctorT 
EmptyCollisionTraits<BasicTraits,BVOL>::createInitDoubleFunctor (ObjectT* obj) 
{
   typename InitDoubleFunctorT::InitMethodT f = &(ObjectT::InitDouble);
   return InitDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename EmptyCollisionTraits<BasicTraits,BVOL>::LeaveDoubleFunctorT 
EmptyCollisionTraits<BasicTraits,BVOL>::createLeaveDoubleFunctor (ObjectT* obj) 
{
   typename LeaveDoubleFunctorT::InitMethodT f = &(ObjectT::LeaveDouble);
   return LeaveDoubleFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename EmptyCollisionTraits<BasicTraits,BVOL>::BVolBVolFunctorT 
EmptyCollisionTraits<BasicTraits,BVOL>::createBVolBVolFunctor (ObjectT* obj) 
{
   typename BVolBVolFunctorT::DispatchMethodT f = &(ObjectT::BVolBVolCollision);
   return BVolBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename EmptyCollisionTraits<BasicTraits,BVOL>::PrimBVolFunctorT 
EmptyCollisionTraits<BasicTraits,BVOL>::createPrimBVolFunctor (ObjectT* obj) 
{
   typename PrimBVolFunctorT::DispatchMethodT f = &(ObjectT::PrimBVolCollision);
   return PrimBVolFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename EmptyCollisionTraits<BasicTraits,BVOL>::BVolPrimFunctorT 
EmptyCollisionTraits<BasicTraits,BVOL>::createBVolPrimFunctor (ObjectT* obj) 
{
   typename BVolPrimFunctorT::DispatchMethodT f = &(ObjectT::BVolPrimCollision);
   return BVolPrimFunctorT(obj, f);
}
template <class BasicTraits, class BVOL>
typename EmptyCollisionTraits<BasicTraits,BVOL>::PrimPrimFunctorT 
EmptyCollisionTraits<BasicTraits,BVOL>::createPrimPrimFunctor (ObjectT* obj) 
{
   typename PrimPrimFunctorT::DispatchMethodT f = &(ObjectT::PrimPrimCollision);
   return PrimPrimFunctorT(obj, f);
}

template<class BasicTraits, class BVOL>
bool EmptyCollision<BasicTraits,BVOL>::InitDouble 
(GroupType*, const TransformType&, const TransformType&, 
 GroupType*, const TransformType&, const TransformType&)
{
   return true;
}

template <class BasicTraits, class BVOL>
typename EmptyCollision<BasicTraits,BVOL>::ResultT
EmptyCollision<BasicTraits,BVOL>::BVolBVolCollision (GroupType*, GroupType*)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolBVolTests;

   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL>
typename EmptyCollision<BasicTraits,BVOL>::ResultT
EmptyCollision<BasicTraits,BVOL>::PrimPrimCollision (AdapterType*, AdapterType*)
{
   // Perform triangle-triangle overlap test
   ++m_numPrimPrimTests;

   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

template <class BasicTraits, class BVOL>
typename EmptyCollision<BasicTraits,BVOL>::ResultT
EmptyCollision<BasicTraits,BVOL>::BVolPrimCollision (GroupType*, AdapterType*)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numBVolPrimTests;

   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}
template <class BasicTraits, class BVOL>
typename EmptyCollision<BasicTraits,BVOL>::ResultT
EmptyCollision<BasicTraits,BVOL>::PrimBVolCollision (AdapterType*, GroupType*)
{
   // check if first collision already found
   if (getStopFirst() && getNumContacts()>0) {
      return DoubleTraverserBase<BasicTraits>::QUIT;
   }
   ++m_numPrimBVolTests;

   return DoubleTraverserBase<BasicTraits>::CONTINUE;
}

