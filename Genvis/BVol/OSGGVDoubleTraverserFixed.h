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
//   $Revision: 1.4 $
//   $Date: 2004/03/12 13:16:55 $
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

/*! \brief Base class for parallel traversal of two bounding volume hierarchies 
    down to fixed depths.
 */
template <class BasicTraits>
class DoubleTraverserFixedBase 
: public DoubleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef DoubleTraverserBase<BasicTraits>    Inherited;
   typedef typename Inherited::Cache           Cache;
   typedef typename Inherited::CacheData       CacheData;
   typedef typename Inherited::GeomObjectType  GeomObjectType;
   typedef typename Inherited::TransformType   TransformType;
   typedef typename Inherited::ResultType      ResultType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Depth.                                                       */
   /*! \{                                                                 */
   /*! Maximum depth in first hierarchy. */
   virtual inline u32 getDepth0 () const = 0;
   /*! Maximum depth in second hierarchy. */
   virtual inline u32 getDepth1 () const = 0;
   /*! Current depth in first hierarchy. Method should only be called during 
       a traversal! */
   virtual inline u32 getCurrentDepth0 () const = 0;
   /*! Current depth in second hierarchy. Method should only be called during 
       a traversal! */
   virtual inline u32 getCurrentDepth1 () const = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

typedef DoubleTraverserFixedBase<OpenSGTraits>         OSGDoubleTraverserFixed;


/*! \brief Depth-first traversal of two binary bounding volume hierarchies.
    The template argument DoubleTraits is used to define the traversal semantics.
    The traversal proceeds down to depths D0, D1 given in the remaining template 
    arguments.
 */
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
class DoubleTraverserFixed 
: public DoubleTraverserFixedBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef DoubleTraverserFixedBase<BasicTraits>   Inherited;
   typedef typename Inherited::Cache               Cache;
   typedef typename Inherited::CacheData           CacheData;
   typedef typename Inherited::GeomObjectType      GeomObjectType;
   typedef typename Inherited::TransformType       TransformType;
   typedef typename Inherited::ResultType          ResultType;

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
   inline void        setDepth0 (u32 value);
   inline void        setDepth1 (u32 value);
   virtual inline u32 getDepth0 () const;
   virtual inline u32 getDepth1 () const;
   virtual inline u32 getCurrentDepth0 () const;
   virtual inline u32 getCurrentDepth1 () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */
   virtual inline bool apply   (const GeomObjectType& node0, 
				const GeomObjectType& node1);
   virtual inline bool apply   (CacheData& parent0, CacheData& data0, BVolAdapterBase* node0, 
				CacheData& parent1, CacheData& data1, BVolAdapterBase* node1, 
				const TransformType& m0 = TransformType::identity(), 
				const TransformType& m1 = TransformType::identity());
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Dispatching.                                                 */
   /*! \{                                                                 */
   inline void traverseGeneralLeaf   (GeneralType* b0, AdapterType* p1);
   inline void traverseLeafGeneral   (AdapterType* p0, GeneralType* b1);
   inline void traverseGeneralInner  (GeneralType* b0, GroupType*   g1);
   inline void traverseInnerGeneral  (GroupType*   g0, GeneralType* b1);
   inline void traverseGeneralGeneral(GeneralType* b0, GeneralType* b1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Final.                                                       */
   /*! \{                                                                 */
   inline void traverseInnerInner(GroupType* g0,   GroupType* g1);
   inline void traverseInnerLeaf (GroupType* g0,   AdapterType* p1);
   inline void traverseLeafInner (AdapterType* p0, GroupType* g1);
   inline void traverseLeafLeaf  (AdapterType* p0, AdapterType* p1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   ObjectT          m_data;
   u32              m_d0;
   u32              m_d1;
   u32              m_current0;
   u32              m_current1;
   InitFunctorT        initFunc;
   InitDoubleFunctorT  initDoubleFunc;
   LeaveDoubleFunctorT leaveDoubleFunc;
   BVolBVolFunctorT bbFunc;
   PrimBVolFunctorT pbFunc;
   BVolPrimFunctorT bpFunc;
   PrimPrimFunctorT ppFunc;
};

template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::setDepth0 (u32 value)
{
   m_d0 = value;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::setDepth1 (u32 value)
{
   m_d1 = value;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline u32 DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDepth0 () const
{
   return m_d0;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline u32 DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDepth1 () const
{
   return m_d1;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline u32 DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getCurrentDepth0 () const
{
   return m_current0;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline u32 DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getCurrentDepth1 () const
{
   return m_current1;
}

template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void   
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseInnerInner
(GroupType* g0,   GroupType* g1)
{
   if (bbFunc.call(g0, g1) == CONTINUE) {
      u32 i0 = m_current0+1;
      u32 i1 = m_current1+1;
      if (i0 > getDepth0() || i1 > getDepth1()) {	  
	 return;
      }

      assert(g0->size() >= 2);
      GeneralType *son00, *son01;
      if (i0 <= getDepth0()) {
	 son00 = (GeneralType*)g0->getSon(0);
	 son01 = (GeneralType*)g0->getSon(1);
      } else {
	 son00 = son01 = (GeneralType*)g0;
      }
      assert(g1->size() >= 2);
      GeneralType *son10, *son11;
      if (i1 <= getDepth1()) {
	 son10 = (GeneralType*)g1->getSon(0);
	 son11 = (GeneralType*)g1->getSon(1);
      } else {
	 son10 = son11 = (GeneralType*)g1;
      }
      if (i1 <= getDepth1()) {
	 m_current0 = i0; m_current1 = i1;
	 traverseGeneralGeneral(son00, son10);
	 m_current0 = i0; m_current1 = i1;
	 traverseGeneralGeneral(son00, son11);
      }
      if (i0 <= getDepth0()) {	  
	 m_current0 = i0; m_current1 = i1;
	 traverseGeneralGeneral(son01, son10);
	 m_current0 = i0; m_current1 = i1;
	 traverseGeneralGeneral(son01, son11);
      }
      return;
   }
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void   
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseInnerLeaf 
(GroupType* g0,   AdapterType* p1)
{
   if (bpFunc.call(g0, p1) == CONTINUE) {
      u32 i0 = m_current0+1;
      u32 i1 = m_current1;
      if (i0 <= getDepth0()) {	  
	 assert(g0->size() >= 2);
	 GeneralType* son00 = (GeneralType*)g0->getSon(0);
	 GeneralType* son01 = (GeneralType*)g0->getSon(1);

	 m_current0 = i0; m_current1 = i1;
	 traverseGeneralLeaf(son00, p1);
	 m_current0 = i0; m_current1 = i1;
	 traverseGeneralLeaf(son01, p1);
      }
      return;
   }
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void   
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseLeafInner 
(AdapterType* p0, GroupType* g1)
{
   if (pbFunc.call(p0, g1) == CONTINUE) {
      u32 i0 = m_current0;
      u32 i1 = m_current1+1;
      if (i1 <= D1) {	  
	 assert(g1->size() >= 2);
	 GeneralType* son10 = (GeneralType*)g1->getSon(0);
	 GeneralType* son11 = (GeneralType*)g1->getSon(1);

	 m_current0 = i0; m_current1 = i1;
	 traverseLeafGeneral(p0, son10);
	 m_current0 = i0; m_current1 = i1;
	 traverseLeafGeneral(p0, son11);
      }
      return;
   }
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void   
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseLeafLeaf 
(AdapterType* p0, AdapterType* p1)
{
   bool result = (ppFunc.call(p0, p1) == CONTINUE);
   return;
}


template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseGeneralLeaf 
(GeneralType* b0, AdapterType* p1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafLeaf((AdapterType*)(b0), p1);	
   } else {
      traverseInnerLeaf((GroupType*)(b0),   p1);	
   }
}

template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseLeafGeneral  
(AdapterType* p0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverseLeafLeaf(p0, (AdapterType*)(b1));	
   } else {
      traverseLeafInner(p0, (GroupType*)(b1));	
   }
}

template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseGeneralInner
(GeneralType* b0, GroupType* g1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafInner((AdapterType*)(b0), g1);	
   } else {
      traverseInnerInner((GroupType*)(b0),   g1); 	
   }
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseInnerGeneral
(GroupType* g0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverseInnerLeaf(g0, (AdapterType*)(b1));	
   } else {
      traverseInnerInner(g0, (GroupType*)(b1));	
   }
}

template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline void 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::traverseGeneralGeneral
(GeneralType* b0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafGeneral((AdapterType*)(b0), b1);	
   } else {
      traverseInnerGeneral((GroupType*)(b0),   b1);	
   }
}


template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline const DataBase<BasicTraits>& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getData () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline DataBase<BasicTraits>& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getData ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline const typename DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::ObjectT& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDataTyped () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline typename DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::ObjectT& 
DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::getDataTyped ()
{
   return m_data;
}


template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
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
   m_d0 = D0;
   m_d1 = D1;
   getData().setTraverser(this);
}

template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline bool DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::apply 
(const GeomObjectType& node0, const GeomObjectType& node1)
{
   CacheData& data0 = Cache::the()[node0];
   CacheData& data1 = Cache::the()[node1];

   GroupType* root0 = static_cast<GroupType*>(*data0.getAdapter(AdapterType::getAdapterId()).begin());
   GroupType* root1 = static_cast<GroupType*>(*data1.getAdapter(AdapterType::getAdapterId()).begin());
   assert(root0 != NULL);
   assert(root1 != NULL);

   TransformType m0;
   if (data0.getAdapterMatrix(AdapterType::getAdapterId()) == TransformType::identity()) {
      m0.setValue(data0.getToWorldMatrix());
   } else {
      m0.invertFrom(data0.getAdapterMatrix(AdapterType::getAdapterId()));
      m0.multLeft(data0.getToWorldMatrix());
   }
   TransformType m1; 
   if (data1.getAdapterMatrix(AdapterType::getAdapterId()) == TransformType::identity()) {
      m1.setValue(data1.getToWorldMatrix());
   } else {
      m1.invertFrom(data1.getAdapterMatrix(AdapterType::getAdapterId()));
      m1.multLeft(data1.getToWorldMatrix());
   }
   m_current0 = m_current1 = 0;
   if (initFunc.call() &&
       initDoubleFunc.call(root0, m0, data0.getFrameMatrix(),
			   root1, m1, data1.getFrameMatrix())) { 
      traverseInnerInner(root0, root1);
      leaveDoubleFunc.call(root0, m0, data0.getFrameMatrix(),
			   root1, m1, data1.getFrameMatrix());
      return true;
   } 
   return false;
}

template <class BasicTraits, class DoubleTraits, u32 D0, u32 D1>
inline bool DoubleTraverserFixed<BasicTraits,DoubleTraits,D0,D1>::apply   
(CacheData& parent0, CacheData& data0, BVolAdapterBase* node0,
 CacheData& parent1, CacheData& data1, BVolAdapterBase* node1,
 const TransformType& m0, const TransformType& m1)
{
   GroupType* root0 = static_cast<GroupType*>(node0);
   GroupType* root1 = static_cast<GroupType*>(node1);
   assert(root0 != NULL);
   assert(root1 != NULL);

   m_current0 = m_current1 = 0;
   if (initDoubleFunc.call(root0, m0, parent0.getFrameMatrix(), 
			   root1, m1, parent1.getFrameMatrix())) { 
      traverseInnerInner(root0, root1);
      leaveDoubleFunc.call(root0, m0, parent0.getFrameMatrix(), 
			   root1, m1, parent1.getFrameMatrix());
      return true;
   } 
   return false;
}

END_GENVIS_NAMESPACE
#endif



