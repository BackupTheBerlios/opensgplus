//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2003/09/25 16:19:44 $
//                                                                            
//=============================================================================

#ifndef OSGGVDOUBLETRAVERSERFIXED_H
#define OSGGVDOUBLETRAVERSERFIXED_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVGroup.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVDoubleTraverser.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base class for double traversal up to fixed depths.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixedBase 
: public DoubleTraverserBase<BasicTraits>
{
public:
   typedef DoubleTraverserBase<BasicTraits>   Inherited;
   /*---------------------------------------------------------------------*/
   /*! \name Depth.                                                       */
   /*! \{                                                                 */
   /*! Maximum depth in first hierarchy. */
   virtual inline unsigned getDepth0 () const = 0;
   /*! Maximum depth in second hierarchy. */
   virtual inline unsigned getDepth1 () const = 0;
   /*! Current depth in first hierarchy. Method should only be called during 
       a traversal! */
   virtual inline unsigned getCurrentDepth0 () const = 0;
   /*! Current depth in second hierarchy. Method should only be called during 
       a traversal! */
   virtual inline unsigned getCurrentDepth1 () const = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

typedef DoubleTraverserFixedBase<OpenSGTraits>         OSGDoubleTraverserFixed;


/*! \brief Double traverser for pairwise collision detection on binary hierarchies.
    The template argument DoubleTraits is used to define the traversal semantics.
    The traversal proceeds down to depths D0, D1 given in the remaining template 
    arguments.
    For this class there are two different implementations:
    If GV_META_DEPTHCOUNT is defined, then the depth counting is done 
    at compile-time with template member functions.
    Otherwise depth counting is done during traversal.
 */
template <class BasicTraits, class DoubleTraits, int D0, int D1>
class OSG_GENVISLIB_DLLMAPPING DoubleTraverserFixed 
: public DoubleTraverserFixedBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef DoubleTraverserFixedBase<BasicTraits>   Inherited;
   typedef typename DoubleTraits::BVol             BVol;
   typedef typename DoubleTraits::AdapterType      AdapterType;
   typedef typename DoubleTraits::GroupType        GroupType;
   typedef typename DoubleTraits::GeneralType      GeneralType;
   typedef typename DoubleTraits::ObjectT          ObjectT;
   typedef typename DoubleTraits::InitFunctorT        InitFunctorT;
   typedef typename DoubleTraits::InitDoubleFunctorT  InitDoubleFunctorT;
   typedef typename DoubleTraits::LeaveDoubleFunctorT LeaveDoubleFunctorT;
   typedef typename DoubleTraits::BVolBVolFunctorT BVolBVolFunctorT;
   typedef typename DoubleTraits::PrimBVolFunctorT PrimBVolFunctorT;
   typedef typename DoubleTraits::BVolPrimFunctorT BVolPrimFunctorT;
   typedef typename DoubleTraits::PrimPrimFunctorT PrimPrimFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline DoubleTraverserFixed ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name DataBase for traversal semantics and collision result.       */
   /*! \{                                                                 */
   virtual inline DataBase<BasicTraits>&       getData ();
   virtual inline const DataBase<BasicTraits>& getData () const;
   inline ObjectT&                             getDataTyped ();
   inline const ObjectT&                       getDataTyped () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Depth.                                                       */
   /*! \{                                                                 */
   virtual inline unsigned getDepth0 () const;
   virtual inline unsigned getDepth1 () const;
   virtual inline unsigned getCurrentDepth0 () const;
   virtual inline unsigned getCurrentDepth1 () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */
   virtual inline bool apply   (const OSG::NodePtr& node0, const OSG::NodePtr& node1);
   virtual inline bool apply   (CacheData& data0, BVolAdapterBase* node0, const TransformType& m0,
				CacheData& data1, BVolAdapterBase* node1, const TransformType& m1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Dispatching.                                                 */
   /*! \{                                                                 */
   inline void traverse (GeneralType* b0, AdapterType* p1);
   inline void traverse (AdapterType* p0, GeneralType* b1);
   inline void traverse (GeneralType* b0, GroupType*   g1);
   inline void traverse (GroupType*   g0, GeneralType* b1);
   inline void traverse (GeneralType* b0, GeneralType* b1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Final.                                                       */
   /*! \{                                                                 */
   inline void traverse (GroupType* g0,   GroupType* g1);
   inline void traverse (GroupType* g0,   AdapterType* p1);
   inline void traverse (AdapterType* p0, GroupType* g1);
   inline void traverse (AdapterType* p0, AdapterType* p1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   ObjectT          m_data;
   unsigned         m_d0;
   unsigned         m_d1;
   InitFunctorT        initFunc;
   InitDoubleFunctorT  initDoubleFunc;
   LeaveDoubleFunctorT leaveDoubleFunc;
   BVolBVolFunctorT bbFunc;
   PrimBVolFunctorT pbFunc;
   BVolPrimFunctorT bpFunc;
   PrimPrimFunctorT ppFunc;
};

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline unsigned DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDepth0 () const
{
   return D0;
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline unsigned DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDepth1 () const
{
   return D1;
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline unsigned DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getCurrentDepth0 () const
{
   return m_d0;
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline unsigned DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getCurrentDepth1 () const
{
   return m_d1;
}

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void   
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse 
(GroupType* g0,   GroupType* g1)
{
   if (bbFunc.call(g0, g1) == CONTINUE) {
      const typename GroupType::Container& sons0 = g0->getSons();
      const typename GroupType::Container& sons1 = g1->getSons();

      unsigned i0 = m_d0+1;
      unsigned i1 = m_d1+1;
      if (i0 <= getDepth0()) {	  
 	 assert(sons1.size() >= 2);
	 m_d0 = i0; m_d1 = i1;
	 traverse((GeneralType*)sons0[0], (GeneralType*)sons1[0]);
	 m_d0 = i0; m_d1 = i1;
	 traverse((GeneralType*)sons0[0], (GeneralType*)sons1[1]);
      }
      if (i1 <= getDepth1()) {	  
	 assert(sons0.size() >= 2);
	 m_d0 = i0; m_d1 = i1;
	 traverse((GeneralType*)sons0[1], (GeneralType*)sons1[0]);
	 m_d0 = i0; m_d1 = i1;
	 traverse((GeneralType*)sons0[1], (GeneralType*)sons1[1]);
      }
      return;
   }
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void   
  DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse (GroupType* g0,   AdapterType* p1)
{
   if (bpFunc.call(g0, p1) == CONTINUE) {
      const typename GroupType::Container& sons0 = g0->getSons();
      
      unsigned i0 = m_d0+1;
      unsigned i1 = m_d1;
      if (i0 <= getDepth0()) {	  
	 assert(sons0.size() >= 2);
	 m_d0 = i0; m_d1 = i1;
	 traverse((GeneralType*)sons0[0], p1);
	 m_d0 = i0; m_d1 = i1;
	 traverse((GeneralType*)sons0[1], p1);
      }
      return;
   }
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void   
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse (AdapterType* p0, GroupType* g1)
{
   if (pbFunc.call(p0, g1) == CONTINUE) {
      const typename GroupType::Container& sons1 = g1->getSons();
      
      unsigned i0 = m_d0;
      unsigned i1 = m_d1+1;
      if (i1 <= D1) {	  
	 assert(sons1.size() >= 2);
	 m_d0 = i0; m_d1 = i1;
	 traverse(p0, (GeneralType*)sons1[0]);
	 m_d0 = i0; m_d1 = i1;
	 traverse(p0, (GeneralType*)sons1[1]);
      }
      return;
   }
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void   
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse (AdapterType* p0, AdapterType* p1)
{
   bool result = (ppFunc.call(p0, p1) == CONTINUE);
   return;
}


template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse (GeneralType* b0, AdapterType* p1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverse((AdapterType*)b0, p1);	
   } else {
      traverse((GroupType*)b0,   p1);	
   }
}

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse  (AdapterType* p0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverse(p0, (AdapterType*)b1);	
   } else {
      traverse(p0, (GroupType*)b1);	
   }
}

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse  (GeneralType* b0, GroupType* g1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverse((AdapterType*)b0, g1);	
   } else {
      traverse((GroupType*)b0,   g1); 	
   }
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void 
  DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse  (GroupType* g0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverse(g0, (AdapterType*)b1);	
   } else {
      traverse(g0, (GroupType*)b1);	
   }
}

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline void 
  DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverse  (GeneralType* b0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverse((AdapterType*)b0, b1);	
   } else {
      traverse((GroupType*)b0,   b1);	
   }
}


template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline const DataBase<BasicTraits>& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getData () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline DataBase<BasicTraits>& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getData ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline const DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::ObjectT& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDataTyped () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::ObjectT& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDataTyped ()
{
   return m_data;
}


#ifdef GV_PROFILED
#include <pgouser.h>
#endif

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::DoubleTraverserFixed ()
  : Inherited(),
    m_data(),
    initFunc(DoubleTraits::createInitFunctor(&m_data)),
    initDoubleFunc(DoubleTraits::createInitDoubleFunctor(&m_data)),
    leaveDoubleFunc(DoubleTraits::createLeaveDoubleFunctor(&m_data)),
    bbFunc(DoubleTraits::createBVolBVolFunctor(&m_data)),
    pbFunc(DoubleTraits::createPrimBVolFunctor(&m_data)),
    bpFunc(DoubleTraits::createBVolPrimFunctor(&m_data)),
    ppFunc(DoubleTraits::createPrimPrimFunctor(&m_data))
{
   getData().setTraverser(this);
}

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline bool DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::apply 
(const NodePtr& node0, const NodePtr& node1)
{
   CacheData& data0 = getCache()[node0];
   CacheData& data1 = getCache()[node1];

   GroupType* root0 = static_cast<GroupType*>(*data0.getAdapter(BVolAdapterBase::getAdapterId()).begin());
   GroupType* root1 = static_cast<GroupType*>(*data1.getAdapter(BVolAdapterBase::getAdapterId()).begin());
   assert(root0 != NULL);
   assert(root1 != NULL);

   TransformType m0;
   if (data0.getAdapterMatrix(BVolAdapterBase::getAdapterId()) == TransformType::identity()) {
      m0.setValue(data0.getToWorldMatrix());
   } else {
      m0.invertFrom(data0.getAdapterMatrix(BVolAdapterBase::getAdapterId()));
      m0.multLeft(data0.getToWorldMatrix());
   }
   TransformType m1; 
   if (data1.getAdapterMatrix(BVolAdapterBase::getAdapterId()) == TransformType::identity()) {
      m1.setValue(data1.getToWorldMatrix());
   } else {
      m1.invertFrom(data1.getAdapterMatrix(BVolAdapterBase::getAdapterId()));
      m1.multLeft(data1.getToWorldMatrix());
   }
   m_d0 = m_d1 = 0;
#ifdef GV_PROFILED
   _PGOPTI_Prof_Reset();
#endif
   if (initFunc.call() &&
       initDoubleFunc.call(root0, m0, root1, m1)) { 
      traverse(root0, root1);
      leaveDoubleFunc.call(root0, m0, root1, m1);

#ifdef GV_PROFILED
      _PGOPTI_Prof_Dump();
#endif
      return true;
   } 

#ifdef GV_PROFILED
   _PGOPTI_Prof_Dump();
#endif
   return false;
}

template <class BasicTraits, class DoubleTraits, int D0, int D1>
inline bool DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::apply   
(CacheData&, BVolAdapterBase* node0, const TransformType& m0,
 CacheData&, BVolAdapterBase* node1, const TransformType& m1)
{
   GroupType* root0 = static_cast<GroupType*>(node0);
   GroupType* root1 = static_cast<GroupType*>(node1);
   assert(root0 != NULL);
   assert(root1 != NULL);

   m_d0 = m_d1 = 0;
#ifdef GV_PROFILED
   _PGOPTI_Prof_Reset();
#endif
   if (initDoubleFunc.call(root0, m0, root1, m1)) { 
      traverse(root0, root1);
      leaveDoubleFunc.call(root0, m0, root1, m1);

#ifdef GV_PROFILED
      _PGOPTI_Prof_Dump();
#endif
      return true;
   } 

#ifdef GV_PROFILED
   _PGOPTI_Prof_Dump();
#endif
   return false;
}

END_GENVIS_NAMESPACE
#endif



