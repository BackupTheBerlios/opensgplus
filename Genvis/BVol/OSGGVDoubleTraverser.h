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
//   $Date: 2004/12/20 15:57:10 $
//                                                                            
//=============================================================================

#ifndef OSGGVDOUBLETRAVERSER_H
#define OSGGVDOUBLETRAVERSER_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVDoubleTraverserBase.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Depth-first traversal of two n-ary bounding volume hierarchies.
    The template argument DoubleTraits is used to define the traversal semantics.
    DoubleTraits has to specify the following datatypes
    BVol, GeneralType, GroupType, AdapterType, ObjectT
    and functor types
    \begin{description}
    \item[InitFunctorT]       with signature bool initFunc()
    \item[InitDoubleFunctorT] with signature bool initDoubleFunc(GroupType*, TransformType, GroupType*, TransformType)
    \item[BVolBVolFunctorT]   with signature ResultType bbFunc(GroupType*, GroupType*)
    \item[PrimBVolFunctorT]   with signature ResultType pbFunc(AdapterType*, GroupType*)
    \item[BVolPrimFunctorT]   with signature ResultType bpFunc(GroupType*, AdapterType*)
    \item[PrimPrimFunctorT]   with signature ResultType ppFunc(AdapterType*, AdapterType*)
    \end{description}
    For each functor type a function FUNCTORT createFUNCTOR (ObjectT*) is required in DoubleTraits.
 */
template <class BasicTraits, class DoubleTraits>
class DoubleTraverser 
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
   inline DoubleTraverser ();
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
   inline void   traverseInnerInner (GroupType*   n0,    GroupType* n1,
				     CacheData&   data0, CacheData& data1);
   inline void   traverseInnerInner (GroupType* n0,   GroupType* n1);
   inline void   traverseInnerLeaf  (GroupType* n0,   AdapterType* n1);
   inline void   traverseLeafInner  (AdapterType* n0, GroupType* n1);
   inline void   traverseLeafLeaf   (AdapterType* n0, AdapterType* n1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   // generalized front-cache
   typename CacheData::AdapterVector m_cache0;
   typename CacheData::AdapterVector m_cache1;

   ObjectT          m_data;
   InitFunctorT        initFunc;
   InitDoubleFunctorT  initDoubleFunc;
   LeaveDoubleFunctorT leaveDoubleFunc;
   BVolBVolFunctorT bbFunc;
   PrimBVolFunctorT pbFunc;
   BVolPrimFunctorT bpFunc;
   PrimPrimFunctorT ppFunc;
};

template <class BasicTraits, class DoubleTraits>
inline DataBase<BasicTraits>& DoubleTraverser<BasicTraits,DoubleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const DataBase<BasicTraits>& DoubleTraverser<BasicTraits,DoubleTraits>::getData () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline typename DoubleTraverser<BasicTraits,DoubleTraits>::ObjectT&      
DoubleTraverser<BasicTraits,DoubleTraits>::getDataTyped ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const typename DoubleTraverser<BasicTraits,DoubleTraits>::ObjectT& 
DoubleTraverser<BasicTraits,DoubleTraits>::getDataTyped () const
{
   return m_data;
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseGeneralLeaf (GeneralType* b0, AdapterType* p1)
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
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseLeafGeneral  (AdapterType* p0, GeneralType* b1)
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
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseGeneralInner  (GeneralType* b0, GroupType* g1)
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
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseInnerGeneral  (GroupType* g0, GeneralType* b1)
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
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseGeneralGeneral  (GeneralType* b0, GeneralType* b1)
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
inline DoubleTraverser<BasicTraits,DoubleTraits>::DoubleTraverser ()
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
}

template <class BasicTraits, class DoubleTraits>
inline bool DoubleTraverser<BasicTraits,DoubleTraits>::apply 
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
      if (getUseCoherency()) {
	 traverseInnerInner(root0, root1, data0, data1);
      } else {
	 traverseInnerInner(root0, root1);
      }
      leaveDoubleFunc.call(root0, m0, data0.getFrameMatrix(), 
			   root1, m1, data1.getFrameMatrix());

      return true;
   } 
   return false;
}

template <class BasicTraits, class DoubleTraits>
inline bool DoubleTraverser<BasicTraits,DoubleTraits>::apply   
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
      if (getUseCoherency()) {
	 traverseInnerInner(root0, root1, data0, data1);
      } else {
	 traverseInnerInner(root0, root1);
      }
      leaveDoubleFunc.call(root0, m0, parent0.getFrameMatrix(), 
			   root1, m1, parent1.getFrameMatrix());

      return true;
   } 
   return false;
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseInnerInner 
(GroupType* g0, GroupType* g1)
{
   if (bbFunc.call(g0, g1) == CONTINUE) {
      const typename GroupType::Container& sons0 = g0->getSons();
      const typename GroupType::Container& sons1 = g1->getSons();
      if (rand() < RAND_MAX/2) {
	 for (typename GroupType::ConstIterator it0 = sons0.begin();
	      it0 != sons0.end(); 
	      ++it0) {
	    for (typename GroupType::ConstIterator it1 = sons1.begin();
		 it1 != sons1.end(); 
		 ++it1) {
	      traverseGeneralGeneral((GeneralType*)*it0, (GeneralType*)*it1);
	    }
	 }
      } else {
	 for (typename GroupType::ConstIterator it1 = sons1.begin();
	      it1 != sons1.end(); 
	      ++it1) {
	    for (typename GroupType::ConstIterator it0 = sons0.begin();
		 it0 != sons0.end(); 
		 ++it0) {
	      traverseGeneralGeneral((GeneralType*)*it0, (GeneralType*)*it1);
	    }
	 }
      }
      return;
   }
   if (getUseCoherency()) {
       Adapter* parent = NULL;
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == g1 
	   && (parent=((GeneralType*)last0)->getParent()) == g0->getParent()
	   && parent != NULL) {
	  last0 = parent;
       } else if (last0 == g0 
		  && (parent=((GeneralType*)last1)->getParent()) == g1->getParent()
		  && parent != NULL) {
	  last1 = parent;
       } else {
	  m_cache0.push_back(g0);
	  m_cache1.push_back(g1);
       }
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseInnerInner 
(GroupType* b0,    GroupType* b1,
 CacheData& data0, CacheData& data1)
{
   // get cached nodes
   typename CacheData::AdapterContainer& last0 = data0.getColCache(data1);
   typename CacheData::AdapterContainer& last1 = data1.getColCache(data0);

#if 0
   if (GV_verbose) {
      // statistics
      std::ostringstream type; type << 2*BVol::Size << "-DOP ";
      Real& num = Statistics::the()[std::string(type.str())+="NumCollisionQueries"];
      Statistics::the()[std::string(type.str())+="AvgCachedNodes"]  = 
	(num*Statistics::the()[std::string(type.str())+="AvgCachedNodes"]+last0.size()-1)/(num+1);
      num += 1;
   }
#endif
   if (last0.empty() || last1.empty()) {
      traverseInnerInner(b0, b1);

      // fill collision caches
      last0.clear();
      last0.swap(m_cache0);
      last1.clear();
      last1.swap(m_cache1);
      return;
   }

   typename CacheData::AdapterContainer::iterator i0, i1;
   // mixed pairs
   for (i0=last0.begin()+1, i1=last1.begin()+1;
	i0 != last0.end(); 
	++i0, ++i1) {
      traverseGeneralGeneral((GeneralType*)*i0, (GeneralType*)*i1);
   }

   // update OSG cache
   last0.clear();
   last0.swap(m_cache0);
   last1.clear();
   last1.swap(m_cache1);
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseInnerLeaf 
(GroupType*   g0, AdapterType* p1)
{
   if (bpFunc.call(g0, p1) == CONTINUE) {
      const typename GroupType::Container& sons0 = g0->getSons();

      for (typename GroupType::ConstIterator it0 = sons0.begin();
	   it0 != sons0.end(); 
	   ++it0) {
	 traverseGeneralLeaf((GeneralType*)*it0, p1);
      }
      return;
   }
   if (getUseCoherency()) {
       Adapter* parent = NULL;
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == p1 
	   && (parent=((GeneralType*)last0)->getParent()) == g0->getParent()
	   && parent != NULL) {
	  last0 = parent;
       } else if (last0 == g0 
		  && (parent=((GeneralType*)last1)->getParent()) == p1->getParent()
		  && parent != NULL) {
	  last1 = parent;
       } else {
	  m_cache0.push_back(g0);
	  m_cache1.push_back(p1);
       }
   }
}
template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseLeafInner 
(AdapterType* p0, GroupType*   g1)
{
   if (pbFunc.call(p0, g1) == CONTINUE) {
      const typename GroupType::Container& sons1 = g1->getSons();

      for (typename GroupType::ConstIterator it1 = sons1.begin();
	   it1 != sons1.end(); 
	   ++it1) {
	 traverseLeafGeneral(p0, (GeneralType*)*it1);
      }
      return;
   }
   if (getUseCoherency()) {
       Adapter* parent = NULL;
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == g1 
	   && (parent=((GeneralType*)last0)->getParent()) == p0->getParent()
	   && parent != NULL) {
	  last0 = parent;
       } else if (last0 == p0 
		  && (parent=((GeneralType*)last1)->getParent()) == g1->getParent()
		  && parent != NULL) {
	  last1 = parent;
       } else {
	  m_cache0.push_back(p0);
	  m_cache1.push_back(g1);
       }
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverser<BasicTraits,DoubleTraits>::traverseLeafLeaf 
(AdapterType* p0, AdapterType* p1)
{
   ppFunc.call(p0, p1);
   if (getUseCoherency()) {
       Adapter* parent = NULL;
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == p1 
	   && (((GeneralType*)last0)->getParent() == (parent=p0->getParent())
	       || last0 == parent)) {
	  last0 = parent;
       } else if (last0 == p0 
		  && (((GeneralType*)last1)->getParent() == (parent=p1->getParent())
		      || last1 == parent)) {
	  last1 = parent;
       } else {
	  m_cache0.push_back(p0);
	  m_cache1.push_back(p1);
       }
   }
}

END_GENVIS_NAMESPACE

#endif



