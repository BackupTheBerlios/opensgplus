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

#ifndef OSGGVSINGLETRAVERSER_H
#define OSGGVSINGLETRAVERSER_H

#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVGroup.h"
#include "OSGGVBVolAdapterExt.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base single traverser.
 */
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING SingleTraverserBase
{
public:
   typedef typename BasicTraits::Cache         Cache;
   typedef typename BasicTraits::CacheData     CacheData;
   typedef typename BasicTraits::TransformType TransformType;

   /*---------------------------------------------------------------------*/
   /*! \name Result type.                                                 */
   /*! \{                                                                 */
   enum ResultType {
      CONTINUE, // continue traversal
      QUIT      // quit traversal
   };
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline SingleTraverserBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Cache.                                                       */
   /*! \{                                                                 */
   static inline Cache& getCache ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */   
   virtual bool apply    (const NodePtr& node) = 0;
   virtual bool apply    (CacheData& data, BVolAdapterBase* node, const TransformType& m) = 0;
   virtual bool applyAll (const NodePtr& node) = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   static inline CacheData& getCacheData (CacheData& data, BVolAdapterBase* adapter);
};

template class OSG_GENVISLIB_DLLMAPPING SingleTraverserBase<OpenSGTraits>;
typedef  SingleTraverserBase<OpenSGTraits>  OSGBVolTraverser;


template <class BasicTraits>
inline SingleTraverserBase<BasicTraits>::SingleTraverserBase ()
{
}

template <class BasicTraits>
inline SingleTraverserBase<BasicTraits>::Cache& SingleTraverserBase<BasicTraits>::getCache ()
{
   return OSGCache::the();
}

template <class BasicTraits>
inline SingleTraverserBase<BasicTraits>::CacheData& 
SingleTraverserBase<BasicTraits>::getCacheData (CacheData& data, BVolAdapterBase* group)
{
   if (group->getParent() == NULL) {
      return data;
   }
   // triangle adapter
   OpenSGTriangleBase<BasicTraits>*  tri = 
     dynamic_cast<OpenSGTriangleBase<BasicTraits>*>(group->getParent());
   if (tri != NULL) {
      return tri->getObjectAdapter();
   }
   // object adapter
   OpenSGObjectBase<BasicTraits>* parent = 
     dynamic_cast<OpenSGObjectBase<BasicTraits>*>(group->getParent());
   return getCache()[parent->getOriginal()];
}



/*! \brief Single traverser for n-ary hierarchies.
           The template argument SingleTraits is used to define the traversal semantics.
 */
template <class BasicTraits, class SingleTraits>
class OSG_GENVISLIB_DLLMAPPING SingleTraverser 
: public SingleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
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
   /*! \name DataBase for traversal semantics and collision result.       */
   /*! \{                                                                 */
   inline ObjectT&       getData ();
   inline const ObjectT& getData () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Apply.                                                       */
   /*! \{                                                                 */
   inline bool apply    (const NodePtr& node);
   inline bool apply    (CacheData& data, BVolAdapterBase* node, const TransformType& m);
   inline bool applyAll (const NodePtr& node);
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
inline SingleTraverser<BasicTraits,SingleTraits>::ObjectT& SingleTraverser<BasicTraits,SingleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class SingleTraits>
inline const SingleTraverser<BasicTraits,SingleTraits>::ObjectT& SingleTraverser<BasicTraits,SingleTraits>::getData () const
{
   return m_data;
}

template <class BasicTraits, class SingleTraits>
inline bool SingleTraverser<BasicTraits,SingleTraits>::apply (const NodePtr& node)
{
   CacheData& data = getCache()[node];
   GroupType* root = 
     static_cast<GroupType*>(*data.getAdapter(BVolAdapterBase::getAdapterId()).begin());
   assert(root != NULL);
   if (data.getAdapterMatrix(BVolAdapterBase::getAdapterId()) == TransformType::identity()) {
      if (initFunc.call(root, data.getToWorldMatrix())) { 
	 traverse((GeneralType*)root);
	 return true;
      } 
   } else {
      TransformType m; 
      m.invertFrom(data.getAdapterMatrix(BVolAdapterBase::getAdapterId()));
      m.multLeft(data.getToWorldMatrix());
      if (initFunc.call(root, m)) { 
	 traverse((GeneralType*)root);
	 return true;
      } 
   }
   return false;
}
template <class BasicTraits, class SingleTraits>
inline bool SingleTraverser<BasicTraits,SingleTraits>::applyAll (const NodePtr& node)
{
   CacheData& data = getCache()[node];
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
      traverse((GeneralType*)root);
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
	 for (unsigned i=0; i<group->size(); ++i) {
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
class OSG_GENVISLIB_DLLMAPPING SingleEnterLeaveTraverser
: public SingleTraverserBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
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
   inline bool apply    (const NodePtr& node);
   inline bool apply    (CacheData& data, BVolAdapterBase* node, const TransformType& m);
   inline bool applyAll (const NodePtr& node);
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
inline SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::ObjectT& SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class SingleTraits>
inline const SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::ObjectT& SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::getData () const
{
   return m_data;
}

template <class BasicTraits, class SingleTraits>
inline bool SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::apply (const NodePtr& node)
{
   CacheData& data = getCache()[node];
   BVolAdapterBase* root = 
     static_cast<BVolAdapterBase*>(*data.getAdapter(BVolAdapterBase::getAdapterId()).begin());
   assert(root != NULL);
   if (data.getAdapterMatrix(BVolAdapterBase::getAdapterId()) == TransformType::identity()) {
      TransformType m = data.getToWorldMatrix(); 
      return apply(data, root, m);
   } else {
      TransformType m; 
      m.invertFrom(data.getAdapterMatrix(BVolAdapterBase::getAdapterId()));
      m.multLeft(data.getToWorldMatrix());
      return apply(data, root, m);
   }
}
template <class BasicTraits, class SingleTraits>
inline bool SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::applyAll (const NodePtr& node)
{
   CacheData& data = getCache()[node];
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
inline bool SingleEnterLeaveTraverser<BasicTraits,SingleTraits>::apply 
(CacheData& data, BVolAdapterBase* node, const TransformType& m)
{
   GroupType* root = static_cast<GroupType*>(node);
   assert(root != NULL);

   if (initEnter.call(root, m)) { 
      traverse((GeneralType*)node);
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
	 for (unsigned i=0; i<group->size(); ++i) {
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
