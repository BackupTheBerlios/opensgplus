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
//   $Date: 2004/12/20 15:57:11 $
//                                                                            
//=============================================================================

#ifndef OSGGVSINGLETRAVERSER_H
#define OSGGVSINGLETRAVERSER_H

#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVSingleTraverserBase.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Single traverser for n-ary hierarchies.
    The template argument SingleTraits is used to define the traversal semantics.
 */
template <class BasicTraits, class SingleTraits>
class SingleTraverser 
: public SingleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef SingleTraverserBase<BasicTraits>     Inherited;
   typedef typename Inherited::Cache            Cache;
   typedef typename Inherited::CacheData        CacheData;
   typedef typename Inherited::GeomObjectType   GeomObjectType;
   typedef typename Inherited::TransformType    TransformType;
   typedef typename Inherited::ResultType       ResultType;

   typedef typename SingleTraits::BVol          BVol;
   typedef typename SingleTraits::AdapterType   AdapterType;
   typedef typename SingleTraits::GroupType     GroupType;
   typedef typename SingleTraits::GeneralType   GeneralType;
   typedef typename SingleTraits::ObjectT       ObjectT;
   typedef typename SingleTraits::InitFunctorT  InitFunctorT;
   typedef typename SingleTraits::InnerFunctorT InnerFunctorT;
   typedef typename SingleTraits::LeafFunctorT  LeafFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline SingleTraverser ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name DataBase for traversal semantics.                            */
   /*! \{                                                                 */
   inline ObjectT&       getData ();
   inline const ObjectT& getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */
   inline bool apply    (const GeomObjectType& node);
   inline bool apply    (CacheData& data, BVolAdapterBase* node, 
			 const TransformType& m=TransformType::identity());
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Dispatching.                                                 */
   /*! \{                                                                 */
   inline void traverse (GeneralType* node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   ObjectT       m_data;
   InitFunctorT  initFunc;
   InnerFunctorT innerFunc;
   LeafFunctorT  leafFunc;
};


template <class BasicTraits, class SingleTraits>
inline SingleTraverser<BasicTraits,SingleTraits>::SingleTraverser ()
  : m_data(),
    initFunc(SingleTraits::createInitFunctor(&m_data)),
    innerFunc(SingleTraits::createInnerFunctor(&m_data)),
    leafFunc(SingleTraits::createLeafFunctor(&m_data))
{
}
template <class BasicTraits, class SingleTraits>
inline typename SingleTraverser<BasicTraits,SingleTraits>::ObjectT& 
SingleTraverser<BasicTraits,SingleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class SingleTraits>
inline const typename SingleTraverser<BasicTraits,SingleTraits>::ObjectT& 
SingleTraverser<BasicTraits,SingleTraits>::getData () const
{
   return m_data;
}

template <class BasicTraits, class SingleTraits>
inline bool SingleTraverser<BasicTraits,SingleTraits>::apply 
(const GeomObjectType& node)
{
   CacheData& data = Cache::the()[node];
   bool result = false;
   for (typename CacheData::AdapterContainer::const_iterator 
        it = data.getAdapter(BVolAdapterBase::getAdapterId()).begin();
	it != data.getAdapter(BVolAdapterBase::getAdapterId()).end();
	++it) {

      BVolAdapterBase* first = static_cast<BVolAdapterBase*>(*it);
      CacheData&       d = getCacheData(data, first);
      TransformType    m = d.getToWorldMatrix();

      result |= apply(d, first, m);
   }
   return result;
}
template <class BasicTraits, class SingleTraits>
inline bool SingleTraverser<BasicTraits,SingleTraits>::apply 
(CacheData& data, BVolAdapterBase* node, const TransformType& m)
{
   GroupType* root = static_cast<GroupType*>(node);
   assert(root != NULL);
   if (initFunc.call(root, m)) { 
      traverse(static_cast<GeneralType*>(root));
      return true;
   } 
   return false;
}

template <class BasicTraits, class SingleTraits>
inline void SingleTraverser<BasicTraits,SingleTraits>::traverse (GeneralType* node)
{
#ifdef GV_DISPATCHBYCAST
   GroupType* group = dynamic_cast<GroupType*>(node);
   if (group != NULL) {
#else
   if (node->isInner()) {
      GroupType* group = dynamic_cast<GroupType*>(node);
#endif
      if (innerFunc.call(group) == CONTINUE) {
	 for (u32 i=0; i<group->size(); ++i) {
	    traverse(static_cast<GeneralType*>(group->getSon(i)));   
	 }
      }
   } else { // leaf node
      AdapterType* leaf = dynamic_cast<AdapterType*>(node);
      leafFunc.call(leaf);
   }
}


/*! \brief Single traverser for n-ary hierarchies.
    The template argument SingleTraits is used to define the traversal semantics.
 */
template <class BasicTraits, class SingleTraits>
class SingleEnterLeaveTraverser
: public SingleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef SingleTraverserBase<BasicTraits>     Inherited;
   typedef typename Inherited::Cache            Cache;
   typedef typename Inherited::CacheData        CacheData;
   typedef typename Inherited::GeomObjectType   GeomObjectType;
   typedef typename Inherited::TransformType    TransformType;
   typedef typename Inherited::ResultType       ResultType;

   typedef typename SingleTraits::BVol          BVol;
   typedef typename SingleTraits::AdapterType   AdapterType;
   typedef typename SingleTraits::GroupType     GroupType;
   typedef typename SingleTraits::GeneralType   GeneralType;
   typedef typename SingleTraits::ObjectT       ObjectT;
   typedef typename SingleTraits::InitFunctorT  InitFunctorT;
   typedef typename SingleTraits::InnerFunctorT InnerFunctorT;
   typedef typename SingleTraits::LeafFunctorT  LeafFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline SingleEnterLeaveTraverser ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name DataBase for traversal semantics and collision result.       */
   /*! \{                                                                 */
   inline ObjectT&       getData ();
   inline const ObjectT& getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */
   inline bool apply    (const GeomObjectType& node);
   inline bool apply    (CacheData& data, BVolAdapterBase* node, 
			 const TransformType& m=TransformType::identity());
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   /*---------------------------------------------------------------------*/
   /*! \name Dispatching.                                                 */
   /*! \{                                                                 */
   inline void traverse (GeneralType* node);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

private:
   ObjectT       m_data;
   InitFunctorT  initEnter;
   InitFunctorT  initLeave;
   InnerFunctorT innerEnter;
   InnerFunctorT innerLeave;
   LeafFunctorT  leafEnter;
};

template <class BasicTraits, class SingleTraits>
inline SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::SingleEnterLeaveTraverser ()
  : m_data(),
    initEnter (SingleTraits::createInitFunctor(&m_data)),
    initLeave (SingleTraits::createInitLeaveFunctor(&m_data)),
    innerEnter(SingleTraits::createInnerFunctor(&m_data)),
    innerLeave(SingleTraits::createInnerLeaveFunctor(&m_data)),
    leafEnter (SingleTraits::createLeafFunctor(&m_data))
{
}
template <class BasicTraits, class SingleTraits>
inline typename SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::ObjectT& 
SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class SingleTraits>
inline const typename SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::ObjectT& 
SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::getData () const
{
   return m_data;
}

template <class BasicTraits, class SingleTraits>
inline bool SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::apply 
(const GeomObjectType& node)
{
   CacheData& data = Cache::the()[node];
   bool result = false;
   u32 i = 0;
   for (typename CacheData::AdapterContainer::const_iterator 
        it = data.getAdapter(BVolAdapterBase::getAdapterId()).begin();
	it != data.getAdapter(BVolAdapterBase::getAdapterId()).end();
	++it, ++i) {
      BVolAdapterBase* first = static_cast<BVolAdapterBase*>(*it);
      CacheData&       d = getCacheData(data, first);
      TransformType    m = d.getToWorldMatrix(); 

      result |= apply(d, first, m);
   }
   return result;
}
template <class BasicTraits, class SingleTraits>
inline bool SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::apply 
(CacheData& data, BVolAdapterBase* node, const TransformType& m)
{
   GroupType* root = static_cast<GroupType*>(node);
   assert(root != NULL);

   if (initEnter.call(root, m)) { 
      traverse(static_cast<GeneralType*>(node));
      initLeave.call(root, m);
      return true;
   } 
   initLeave.call(root, m);
   return false;
}

template <class BasicTraits, class SingleTraits>
inline void SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::traverse (GeneralType* node)
{
#if 1
   GroupType* group = dynamic_cast<GroupType*>(node);
   if (group != NULL) {
#else
   if (node->isInner()) {
      GroupType* group = dynamic_cast<GroupType*>(node);
#endif
      if (innerEnter.call(group) == CONTINUE) {
	 for (u32 i=0; i<group->size(); ++i) {
	    traverse(static_cast<GeneralType*>(group->getSon(i)));   
	 }
      }
      innerLeave.call(group);
   } else { // leaf node
      AdapterType* leaf = dynamic_cast<AdapterType*>(node);
      leafEnter.call(leaf);
   }
}

END_GENVIS_NAMESPACE
#endif
