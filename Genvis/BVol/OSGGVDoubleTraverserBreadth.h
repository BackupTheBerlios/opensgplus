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
//   $Revision: 1.1 $
//   $Date: 2004/03/12 13:16:55 $
//                                                                            
//=============================================================================

#ifndef OSGGVDOUBLETRAVERSERBREADTH_H
#define OSGGVDOUBLETRAVERSERBREADTH_H

#include <assert.h>
#include <queue>
#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVGroup.h"
#include "OSGGVCollisionData.h"
#include "OSGGVDoubleTraverser.h"
#include "OSGGVBVolAdapterExt.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Breadth-first traversal of two n-ary bounding volume hierarchies.
    The template argument DoubleTraits is used to define the traversal semantics. 
    DoubleTraits has to specify the following datatypes
    BVol, GeneralType, GroupType, AdapterType, ObjectT
    and functor types
    \begin{description}
    \item[InitFunctorT]       with signature bool initFunc()
    \item[InitDoubleFunctorT] with signature bool initDoubleFunc(GroupType*, TransformType, GroupType*, TransformType)
    \item[RoundFunctorT]      with signature bool roundFunc()
    \item[BVolBVolFunctorT]   with signature ResultType bbFunc(GroupType*, GroupType*)
    \item[PrimBVolFunctorT]   with signature ResultType pbFunc(AdapterType*, GroupType*)
    \item[BVolPrimFunctorT]   with signature ResultType bpFunc(GroupType*, AdapterType*)
    \item[PrimPrimFunctorT]   with signature ResultType ppFunc(AdapterType*, AdapterType*)
    \end{description}
    For each functor type a creation function FUNCTORT createFUNCTOR (ObjectT*) 
    is required in DoubleTraits.
 */
template <class BasicTraits, class DoubleTraits>
class DoubleTraverserBreadth 
: public DoubleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef DoubleTraverserBase<BasicTraits>        Inherited;
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
   typedef typename DoubleTraits::InitFunctorT       InitFunctorT;
   typedef typename DoubleTraits::InitDoubleFunctorT InitDoubleFunctorT;
   typedef typename DoubleTraits::RoundFunctorT    RoundFunctorT;
   typedef typename DoubleTraits::BVolBVolFunctorT BVolBVolFunctorT;
   typedef typename DoubleTraits::PrimBVolFunctorT PrimBVolFunctorT;
   typedef typename DoubleTraits::BVolPrimFunctorT BVolPrimFunctorT;
   typedef typename DoubleTraits::PrimPrimFunctorT PrimPrimFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline DoubleTraverserBreadth ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline u32 getDepth () const;
   inline u32 getNumRemaining () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name DataBase for traversal semantics.                            */
   /*! \{                                                                 */
   virtual inline DataBase<BasicTraits>&       getData ();
   virtual inline const DataBase<BasicTraits>& getData () const;
   inline ObjectT&                             getDataTyped ();
   inline const ObjectT&                       getDataTyped () const;
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
   inline void   traverseGeneralGeneral (GeneralType* n0, GeneralType* n1);
   inline void   traverseInnerGeneral   (GroupType*   n0, GeneralType* n1);
   inline void   traverseGeneralInner   (GeneralType* n0, GroupType*   n1);
   inline void   traverseGeneralLeaf    (GeneralType* n0, AdapterType* n1);
   inline void   traverseLeafGeneral    (AdapterType* n0, GeneralType* n1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Final.                                                       */
   /*! \{                                                                 */
   inline void   traverseInnerInner (GroupType* n0,   GroupType* n1);
   inline void   traverseInnerLeaf  (GroupType* n0,   AdapterType* n1);
   inline void   traverseLeafInner  (AdapterType* n0, GroupType* n1);
   inline void   traverseLeafLeaf   (AdapterType* n0, AdapterType* n1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   typedef std::queue<Adapter*> AdapterQueue;
   AdapterQueue m_cache0;
   AdapterQueue m_cache1;
   u32 m_depth;
   u32 m_num;

   ObjectT          m_data;
   InitFunctorT       initFunc;
   InitDoubleFunctorT initDoubleFunc;
   RoundFunctorT    roundFunc;
   BVolBVolFunctorT bbFunc;
   PrimBVolFunctorT pbFunc;
   BVolPrimFunctorT bpFunc;
   PrimPrimFunctorT ppFunc;
};

template <class BasicTraits, class DoubleTraits>
inline u32 DoubleTraverserBreadth<BasicTraits,DoubleTraits>::getDepth () const
{
   return m_depth;
}
template <class BasicTraits, class DoubleTraits>
inline u32 DoubleTraverserBreadth<BasicTraits,DoubleTraits>::getNumRemaining () const
{
   return m_num;
}

template <class BasicTraits, class DoubleTraits>
inline DataBase<BasicTraits>& DoubleTraverserBreadth<BasicTraits,DoubleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const DataBase<BasicTraits>& DoubleTraverserBreadth<BasicTraits,DoubleTraits>::getData () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline typename DoubleTraverserBreadth<BasicTraits,DoubleTraits>::ObjectT&      
DoubleTraverserBreadth<BasicTraits,DoubleTraits>::getDataTyped ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const typename DoubleTraverserBreadth<BasicTraits,DoubleTraits>::ObjectT& 
DoubleTraverserBreadth<BasicTraits,DoubleTraits>::getDataTyped () const
{
   return m_data;
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseGeneralLeaf (GeneralType* b0, AdapterType* p1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafLeaf((AdapterType*)b0, p1);	
   } else {
      traverseInnerLeaf((GroupType*)b0,   p1);	
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseLeafGeneral  (AdapterType* p0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverseLeafLeaf(p0, (AdapterType*)b1);	
   } else {
      traverseLeafInner(p0, (GroupType*)b1);	
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseGeneralInner  (GeneralType* b0, GroupType* g1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafInner((AdapterType*)b0, g1);	
   } else {
      traverseInnerInner((GroupType*)b0,   g1); 	
   }
}
template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseInnerGeneral  (GroupType* g0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverseInnerLeaf(g0, (AdapterType*)b1);	
   } else {
      traverseInnerInner(g0, (GroupType*)b1);	
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseGeneralGeneral  (GeneralType* b0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafGeneral((AdapterType*)b0, b1);	
   } else {
      traverseInnerGeneral((GroupType*)b0,   b1);	
   }
}


template <class BasicTraits, class DoubleTraits>
inline DoubleTraverserBreadth<BasicTraits,DoubleTraits>::DoubleTraverserBreadth ()
  : Inherited(),
    m_data(),
    initFunc(DoubleTraits::createInitFunctor(&m_data)),
    initDoubleFunc(DoubleTraits::createInitDoubleFunctor(&m_data)),
    roundFunc(DoubleTraits::createRoundFunctor(&m_data)),
    bbFunc(DoubleTraits::createBVolBVolFunctor(&m_data)),
    pbFunc(DoubleTraits::createPrimBVolFunctor(&m_data)),
    bpFunc(DoubleTraits::createBVolPrimFunctor(&m_data)),
    ppFunc(DoubleTraits::createPrimPrimFunctor(&m_data))
{
}

template <class BasicTraits, class DoubleTraits>
inline bool DoubleTraverserBreadth<BasicTraits,DoubleTraits>::apply 
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
   if (initFunc.call() && 
       initDoubleFunc.call(root0, m0, data0.getFrameMatrix(), 
			   root1, m1, data1.getFrameMatrix())) { 
      m_depth = 0; m_num = 1;
      m_cache0.push(root0); m_cache1.push(root1);
      if (roundFunc() == false) {
	 return false;
      }
      // mixed pairs
      while (!m_cache0.empty() && !m_cache1.empty()) {
	 traverseGeneralGeneral(static_cast<GeneralType*>(m_cache0.front()), 
				static_cast<GeneralType*>(m_cache1.front()));
	 m_cache0.pop(); m_cache1.pop();
	 if (--m_num == 0) {
	    ++m_depth; m_num = m_cache1.depth();
	    if (roundFunc() == false) {
	       return false;
	    }
	 }
      }
      return true;
   } 
   return false;
}

template <class BasicTraits, class DoubleTraits>
inline bool DoubleTraverserBreadth<BasicTraits,DoubleTraits>::apply   
(CacheData& parent0, CacheData& data0, BVolAdapterBase* node0, 
 CacheData& parent1, CacheData& data1, BVolAdapterBase* node1, 
 const TransformType& m0, const TransformType& m1)
{
   GroupType* root0 = static_cast<GroupType*>(node0);
   GroupType* root1 = static_cast<GroupType*>(node1);
   assert(root0 != NULL);
   assert(root1 != NULL);

   if (initDoubleFunc.call(root0, m0, parent0.getFrameMatrix(), 
			   root1, m1, parent1.getFrameMatrix())) { 
      m_depth = 0; m_num = 1;
      m_cache0.push(root0); m_cache1.push(root1);
      if (roundFunc() == false) {
	 return false;
      }
      // mixed pairs
      while (!m_cache0.empty() && !m_cache1.empty()) {
	 traverseGeneralGeneral(static_cast<GeneralType*>(m_cache0.front()), 
				static_cast<GeneralType*>(m_cache1.front()));
	 m_cache0.pop(); m_cache1.pop();
	 if (--m_num == 0) {
	    ++m_depth; m_num = m_cache1.depth();
	    if (roundFunc() == false) {
	       return false;
	    }
	 }
      }
      return true;
   } 
   return false;
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseInnerInner 
(GroupType* g0, GroupType* g1)
{
   if (bbFunc.call(g0, g1) == CONTINUE) {
      const typename GroupType::Container& sons0 = g0->getSons();
      const typename GroupType::Container& sons1 = g1->getSons();
      for (typename GroupType::ConstIterator it0 = sons0.begin();
	   it0 != sons0.end(); 
	   ++it0) {
	 for (typename GroupType::ConstIterator it1 = sons1.begin();
	      it1 != sons1.end(); 
	      ++it1) {
	    m_cache0.push(*it0);
	    m_cache1.push(*it1);
	 }
      }
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseInnerLeaf 
(GroupType*   g0, AdapterType* p1)
{
   if (bpFunc.call(g0, p1) == CONTINUE) {
      const typename GroupType::Container& sons0 = g0->getSons();
      for (typename GroupType::ConstIterator it0 = sons0.begin();
	   it0 != sons0.end(); 
	   ++it0) {
 	 m_cache0.push(*it0);
	 m_cache1.push(p1);
      }
   }
}
template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseLeafInner 
(AdapterType* p0, GroupType*   g1)
{
   if (pbFunc.call(p0, g1) == CONTINUE) {
      const typename GroupType::Container& sons1 = g1->getSons();
      for (typename GroupType::ConstIterator it1 = sons1.begin();
	   it1 != sons1.end(); 
	   ++it1) {
 	 m_cache0.push(p0);
	 m_cache1.push(*it1);
      }
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBreadth<BasicTraits,DoubleTraits>::traverseLeafLeaf 
(AdapterType* p0, AdapterType* p1)
{
   ppFunc.call(p0, p1);
}

END_GENVIS_NAMESPACE

#endif



