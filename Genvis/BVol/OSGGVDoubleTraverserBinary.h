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

#ifndef OSGGVDOUBLETRAVERSERBINARY_H
#define OSGGVDOUBLETRAVERSERBINARY_H

#include <assert.h>
#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVGroup.h"
#include "OSGGVDoubleTraverser.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Double traverser for pairwise collision detection on binary hierarchies.
    The template argument DoubleTraits is used to define the traversal semantics.
*/
template <class BasicTraits, class DoubleTraits>
class OSG_GENVISLIB_DLLMAPPING DoubleTraverserBinary 
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
   inline DoubleTraverserBinary ();
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
   virtual inline bool apply   (const OSG::NodePtr& node0, const OSG::NodePtr& node1);
   virtual inline bool apply   (CacheData& data0, BVolAdapterBase* node0, const TransformType& m0,
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
   // generalized front-cache
   typename CacheData::AdapterVector m_cache0;
   typename CacheData::AdapterVector m_cache1;

   ObjectT             m_data;
   InitFunctorT        initFunc;
   InitDoubleFunctorT  initDoubleFunc;
   LeaveDoubleFunctorT leaveDoubleFunc;
   BVolBVolFunctorT    bbFunc;
   PrimBVolFunctorT    pbFunc;
   BVolPrimFunctorT    bpFunc;
   PrimPrimFunctorT    ppFunc;
};

template <class BasicTraits, class DoubleTraits>
inline DataBase<BasicTraits>& 
DoubleTraverserBinary<BasicTraits,DoubleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const DataBase<BasicTraits>& 
DoubleTraverserBinary<BasicTraits,DoubleTraits>::getData () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline const DoubleTraverserBinary<BasicTraits,DoubleTraits>::ObjectT& 
DoubleTraverserBinary<BasicTraits,DoubleTraits>::getDataTyped () const
{
   return m_data;
}
template <class BasicTraits, class DoubleTraits>
inline DoubleTraverserBinary<BasicTraits,DoubleTraits>::ObjectT& 
DoubleTraverserBinary<BasicTraits,DoubleTraits>::getDataTyped ()
{
   return m_data;
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseGeneralLeaf (GeneralType* b0, AdapterType* p1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafLeaf ((AdapterType*)b0, p1);	
   } else {
      traverseInnerLeaf((GroupType*)b0,   p1);	
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseLeafGeneral (AdapterType* p0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverseLeafLeaf (p0, (AdapterType*)b1);	
   } else {
      traverseLeafInner(p0, (GroupType*)b1);	
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseGeneralInner  (GeneralType* b0, GroupType* g1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafInner ((AdapterType*)b0, g1);	
   } else {
      traverseInnerInner((GroupType*)b0,   g1); 	
   }
}
template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseInnerGeneral  (GroupType* g0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b1) != NULL) {
#else
   if (b1->isLeaf()) {
#endif
      traverseInnerLeaf (g0, (AdapterType*)b1);	
   } else {
      traverseInnerInner(g0, (GroupType*)b1);	
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseGeneralGeneral (GeneralType* b0, GeneralType* b1)
{
#ifdef GV_DISPATCHBYCAST
   if (dynamic_cast<AdapterType*>(b0) != NULL) {
#else
   if (b0->isLeaf()) {
#endif
      traverseLeafGeneral ((AdapterType*)b0, b1);	
   } else {
      traverseInnerGeneral((GroupType*)b0,   b1);	
   }
}


#ifdef GV_PROFILED
#include <pgouser.h>
#endif

template <class BasicTraits, class DoubleTraits>
inline DoubleTraverserBinary<BasicTraits,DoubleTraits>::DoubleTraverserBinary ()
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
inline bool DoubleTraverserBinary<BasicTraits,DoubleTraits>::apply
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
      if (getUseCoherency()) {
	 traverseInnerInner  (root0, root1, data0, data1);
      } else {
	 traverseInnerInner(root0, root1);
      }
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

template <class BasicTraits, class DoubleTraits>
inline bool DoubleTraverserBinary<BasicTraits,DoubleTraits>::apply   
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
      if (getUseCoherency()) {
	 traverseInnerInner(root0, root1, data0, data1);
      } else {
	 traverseInnerInner(root0, root1);
      }
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

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseInnerInner 
(GroupType* g0, GroupType* g1)
{
   if (bbFunc.call(g0, g1) == CONTINUE) {

      assert(g0->size() >= 2);
      GeneralType* son00 = (GeneralType*)g0->getSon(0);
      GeneralType* son01 = (GeneralType*)g0->getSon(1);
      assert(g1->size() >= 2);
      GeneralType* son10 = (GeneralType*)g1->getSon(0);
      GeneralType* son11 = (GeneralType*)g1->getSon(1);
      traverseGeneralGeneral(son00, son10);
      traverseGeneralGeneral(son00, son11);
      traverseGeneralGeneral(son01, son10);
      traverseGeneralGeneral(son01, son11);

      return;
   }
   if (getUseCoherency()) {
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == g1 && ((GeneralType*)last0)->isSibling(g0)) {
	  last0 = ((GeneralType*)last0)->getParent();
       } else if (last0 == g0 && ((GeneralType*)last1)->isSibling(g1)) {
	  last1 = ((GeneralType*)last1)->getParent();
       } else {
	  m_cache0.push_back(g0);
	  m_cache1.push_back(g1);
       }
   }
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseInnerInner 
(GroupType* b0,
 GroupType* b1,
 CacheData&   data0, 
 CacheData&   data1)
{
   // get cached nodes
   typename CacheData::AdapterContainer& last0 = data0.getColCache();
   typename CacheData::AdapterContainer& last1 = data1.getColCache();

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
   if (last0.size() <= 1 || last1.size() <= 1) {
      traverseInnerInner(b0, b1);

      last0.insert(last0.end(), m_cache0.begin(),    m_cache0.end());
      m_cache0.clear();
      last1.insert(last1.end(), m_cache1.begin(),    m_cache1.end());
      m_cache1.clear();
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
   last0.erase(last0.begin()+1, last0.end());
   last0.insert(last0.end(), m_cache0.begin(),    m_cache0.end());
   m_cache0.clear();
   last1.erase(last1.begin()+1, last1.end());
   last1.insert(last1.end(), m_cache1.begin(),    m_cache1.end());
   m_cache1.clear();
}

template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseInnerLeaf 
(GroupType*   g0, AdapterType* p1)
{
   if (bpFunc.call(g0, p1) == CONTINUE) {

      assert(g0->size() >= 2);
      traverseGeneralLeaf((GeneralType*)g0->getSon(0), p1);
      traverseGeneralLeaf((GeneralType*)g0->getSon(1), p1);

      return;
   }
   if (getUseCoherency()) {
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == p1 && ((GeneralType*)last0)->isSibling(g0)) {
	  last0 = ((GeneralType*)last0)->getParent();
       } else if (last0 == g0 && ((GeneralType*)last1)->isSibling(p1)) {
	  last1 = ((GeneralType*)last1)->getParent();
       } else {
	  m_cache0.push_back(g0);
	  m_cache1.push_back(p1);
       }
   }
}
template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseLeafInner 
(AdapterType* p0, GroupType*   g1)
{
   if (pbFunc.call(p0, g1) == CONTINUE) {

      assert(g1->size() >= 2);
      traverseLeafGeneral(p0, (GeneralType*)g1->getSon(0));
      traverseLeafGeneral(p0, (GeneralType*)g1->getSon(1));

      return;
   }
   if (getUseCoherency()) {
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == g1 && ((GeneralType*)last0)->isSibling(p0)) {
	  last0 = ((GeneralType*)last0)->getParent();
       } else if (last0 == p0 && ((GeneralType*)last1)->isSibling(g1)) {
	  last1 = ((GeneralType*)last1)->getParent();
       } else {
	  m_cache0.push_back(p0);
	  m_cache1.push_back(g1);
       }
   }
}
template <class BasicTraits, class DoubleTraits>
inline void DoubleTraverserBinary<BasicTraits,DoubleTraits>::traverseLeafLeaf
(AdapterType* p0, AdapterType* p1)
{
   ppFunc.call(p0, p1);
   if (getUseCoherency()) {
       Adapter*& last0 = (m_cache0.empty() ? s_dummy : *(m_cache0.end()-1));
       Adapter*& last1 = (m_cache1.empty() ? s_dummy : *(m_cache1.end()-1));
       if (last1 == p1 && ((GeneralType*)last0)->isSibling(p0)) {
	  last0 = ((GeneralType*)last0)->getParent();
       } else if (last0 == p0 && ((GeneralType*)last1)->isSibling(p1)) {
	  last1 = ((GeneralType*)last1)->getParent();
       } else {
	  m_cache0.push_back(p0);
	  m_cache1.push_back(p1);
       }
   }
}

END_GENVIS_NAMESPACE
#endif



