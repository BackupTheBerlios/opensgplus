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
//   $Date: 2003/09/11 16:20:29 $
//                                                                            
//=============================================================================

#ifndef OSGGVDOUBLETRAVERSERCOHERENT_H
#define OSGGVDOUBLETRAVERSERCOHERENT_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVGroup.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVDoubleTraverser.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Double traverser for pairwise collision detection on n-ary hierarchies.
    The template argument DoubleTraits is used to define the traversal semantics.
    Traversal is started with the first entry in the generalized front cache.
 */
template <class BasicTraits, class DoubleTraits>
class OSG_GENVISLIB_DLLMAPPING DoubleTraverserCoherent 
: public DoubleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef DoubleTraverserBase<BasicTraits>        Inherited;
   typedef typename DoubleTraits::BVol             BVol;
   typedef typename DoubleTraits::AdapterType      AdapterType;
   typedef typename DoubleTraits::GroupType        GroupType;
   typedef typename DoubleTraits::GeneralType      GeneralType;
   typedef typename DoubleTraits::ObjectT          ObjectT;
   typedef typename DoubleTraits::InitFunctorT       InitFunctorT;
   typedef typename DoubleTraits::InitDoubleFunctorT InitDoubleFunctorT;
   typedef typename DoubleTraits::BVolBVolFunctorT BVolBVolFunctorT;
   typedef typename DoubleTraits::PrimBVolFunctorT PrimBVolFunctorT;
   typedef typename DoubleTraits::BVolPrimFunctorT BVolPrimFunctorT;
   typedef typename DoubleTraits::PrimPrimFunctorT PrimPrimFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline DoubleTraverserCoherent ();
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
   virtual inline bool     apply   (const OSG::NodePtr& node0, 
				    const OSG::NodePtr& node1);
   virtual inline bool     apply   (CacheData& data0, BVolAdapterBase* node0, const TransformType& m0,
				    CacheData& data1, BVolAdapterBase* node1, const TransformType& m1);
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
   ObjectT          m_data;
   InitFunctorT       initFunc;
   InitDoubleFunctorT initDoubleFunc;
   BVolBVolFunctorT bbFunc;
   PrimBVolFunctorT pbFunc;
   BVolPrimFunctorT bpFunc;
   PrimPrimFunctorT ppFunc;
};

template <class BasicTraits, class DoubleTraits>
inline DataBase<BasicTraits>& 
DoubleTraverserCoherent<BasicTraits,DoubleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const DataBase<BasicTraits>& 
DoubleTraverserCoherent<BasicTraits,DoubleTraits>::getData () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline DoubleTraverserCoherent<BasicTraits,DoubleTraits>::ObjectT& 
DoubleTraverserCoherent<BasicTraits,DoubleTraits>::getDataTyped ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const DoubleTraverserCoherent<BasicTraits,DoubleTraits>::ObjectT& 
DoubleTraverserCoherent<BasicTraits,DoubleTraits>::getDataTyped () const
{
   return m_data;
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseGeneralLeaf (GeneralType* b0, AdapterType* p1)
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
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseLeafGeneral (AdapterType* p0, GeneralType* b1)
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
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseGeneralInner  (GeneralType* b0, GroupType* g1)
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
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseInnerGeneral (GroupType* g0, GeneralType* b1)
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
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseGeneralGeneral (GeneralType* b0, GeneralType* b1)
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


#ifdef GV_PROFILED
#include <pgouser.h>
#endif

template <class BasicTraits, class DoubleTraits>
inline DoubleTraverserCoherent<BasicTraits,DoubleTraits>::DoubleTraverserCoherent ()
  : Inherited(),
    m_data(),
    initFunc(DoubleTraits::createInitFunctor(&m_data)),
    initDoubleFunc(DoubleTraits::createInitDoubleFunctor(&m_data)),
    bbFunc(DoubleTraits::createBVolBVolFunctor(&m_data)),
    pbFunc(DoubleTraits::createPrimBVolFunctor(&m_data)),
    bpFunc(DoubleTraits::createBVolPrimFunctor(&m_data)),
    ppFunc(DoubleTraits::createPrimPrimFunctor(&m_data))
{
}

template <class BasicTraits, class DoubleTraits>
inline bool DoubleTraverserCoherent<BasicTraits,DoubleTraits>::apply 
(const NodePtr& node0, const NodePtr& node1)
{
   CacheData& data0 = getCache()[node0];
   CacheData& data1 = getCache()[node1];

   GroupType* root0 = static_cast<GroupType*>(*data0.getAdapter(BVolAdapterBase::getAdapterId()).begin());
   GroupType* root1 = static_cast<GroupType*>(*data1.getAdapter(BVolAdapterBase::getAdapterId()).begin());
   assert(root0 != NULL);
   assert(root1 != NULL);

#ifdef GV_PROFILED
   _PGOPTI_Prof_Reset();
#endif
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
   if (initFunc.call() &&
       initDoubleFunc.call(root0, m0, root1, m1)) { 
      traverseInnerInner(root0, root1, data0, data1);

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

template <class BasicTraits, class DoubleTraits>
inline bool DoubleTraverserCoherent<BasicTraits,DoubleTraits>::apply   
(CacheData& data0, BVolAdapterBase* node0, const TransformType& m0,
 CacheData& data1, BVolAdapterBase* node1, const TransformType& m1)
{
   GroupType* root0 = static_cast<GroupType*>(node0);
   GroupType* root1 = static_cast<GroupType*>(node1);
   assert(root0 != NULL);
   assert(root1 != NULL);

#ifdef GV_PROFILED
   _PGOPTI_Prof_Reset();
#endif
   if (initDoubleFunc.call(root0, m0, root1, m1)) { 
      traverseInnerInner(root0, root1, data0, data1);

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

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseInnerInner 
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
	    traverseGeneralGeneral((GeneralType*)*it0, (GeneralType*)*it1);
	 }
      }
      return;
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseInnerInner 
(GroupType* b0,    GroupType* b1,
 CacheData& data0, CacheData& data1)
{
   // get cached nodes
   typename CacheData::AdapterContainer& last0 = data0.getColCache();
   typename CacheData::AdapterContainer& last1 = data1.getColCache();

#if 0
   if (GV_verbose) {
      // statistics
      std::ostringstream type; type << 2*BVol::Size << "-DOP ";
      float& num = Statistics::the()[std::string(type.str())+="NumCollisionQueries"];
      Statistics::the()[std::string(type.str())+="AvgCachedNodes"]  = 
	(num*Statistics::the()[std::string(type.str())+="AvgCachedNodes"]+last0.size()-1)/(num+1);
      num += 1;
   }
#endif
   if (last0.size() <= 1 || last1.size() <= 1) {
      traverseInnerInner(b0, b1);

      const DoubleTraits::ObjectT::CollisionContainer& result = getData().getContacts();
      if (result.size() > 0) {
	 last0.push_back(static_cast<AdapterType*>(result[0].getFirst()));
	 last1.push_back(static_cast<AdapterType*>(result[0].getSecond()));
      }
   } else {
      // minimum pair of last frame
      traverseLeafLeaf((AdapterType*)(last0[1]), (AdapterType*)(last1[1]));
      // tree traversal   
      traverseInnerInner(b0, b1);
      
      // update OSG cache
      const DoubleTraits::ObjectT::CollisionContainer& result = getData().getContacts();
      if (result.size() > 0) {
         last0[1] = static_cast<AdapterType*>(result[0].getFirst());
	 last1[1] = static_cast<AdapterType*>(result[0].getSecond());
      }
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseInnerLeaf 
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
}
template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseLeafInner 
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
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserCoherent<BasicTraits,DoubleTraits>::traverseLeafLeaf
(AdapterType* p0, AdapterType* p1)
{
   ppFunc.call(p0, p1);
}

END_GENVIS_NAMESPACE
#endif



