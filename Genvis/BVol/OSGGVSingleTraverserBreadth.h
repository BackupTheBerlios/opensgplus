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
//   $Revision: 1.2 $
//   $Date: 2004/12/20 15:57:11 $
//                                                                            
//=============================================================================

#ifndef OSGGVSINGLETRAVERSERBREADTH_H
#define OSGGVSINGLETRAVERSERBREADTH_H

#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVGroup.h"
#include "OSGGVBVolAdapterExt.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Breadth-first traverser for n-ary bounding volume hierarchies.
    The template argument SingleTraits is used to define the traversal semantics.
    SingleTraits has to specify the following datatypes
    BVol, GeneralType, GroupType, AdapterType, ObjectT
    and functor types
    \begin{description}
    \item[InitFunctorT]  with signature bool    initFunc()
    \item[RoundFunctorT] with signature bool    roundFunc()
    \item[InnerFunctorT] with signature ResultT innerFunc(GroupType*)
    \item[LeafFunctorT]  with signature ResultT leafFunc(AdapterType*)
    \end{description}
    For each functor type a creation function FUNCTORT createFUNCTOR (ObjectT*) 
    is required in SingleTraits.
 */
template <class BasicTraits, class SingleTraits>
class SingleTraverserBreadth
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
   typedef typename SingleTraits::RoundFunctorT RoundFunctorT;
   typedef typename SingleTraits::InnerFunctorT InnerFunctorT;
   typedef typename SingleTraits::LeafFunctorT  LeafFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline SingleTraverserBreadth ();
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
   typedef std::queue<Adapter*> AdapterQueue;
   AdapterQueue  m_cache;

   u32           m_depth;
   u32           m_num;
   ObjectT       m_data;
   InitFunctorT  initFunc;
   RoundFunctorT roundFunc;
   InnerFunctorT innerFunc;
   LeafFunctorT  leafFunc;
};


template <class BasicTraits, class SingleTraits>
inline SingleTraverserBreadth<BasicTraits,SingleTraits>::SingleTraverserBreadth ()
  : m_data(),
    initFunc(SingleTraits::createInitFunctor(&m_data)),
    roundFunc(SingleTraits::createRoundFunctor(&m_data)),
    innerFunc(SingleTraits::createInnerFunctor(&m_data)),
    leafFunc(SingleTraits::createLeafFunctor(&m_data))
{
}
template <class BasicTraits, class SingleTraits>
inline typename SingleTraverserBreadth<BasicTraits,SingleTraits>::ObjectT& 
SingleTraverserBreadth<BasicTraits,SingleTraits>::getData ()
{
   return m_data;
}
template <class BasicTraits, class SingleTraits>
inline const typename SingleTraverserBreadth<BasicTraits,SingleTraits>::ObjectT& 
SingleTraverserBreadth<BasicTraits,SingleTraits>::getData () const
{
   return m_data;
}

template <class BasicTraits, class SingleTraits>
inline u32 SingleTraverserBreadth<BasicTraits,SingleTraits>::getDepth () const
{
   return m_depth;
}
template <class BasicTraits, class SingleTraits>
inline u32 SingleTraverserBreadth<BasicTraits,SingleTraits>::getNumRemaining () const
{
   return m_num;
}

template <class BasicTraits, class SingleTraits>
inline bool SingleTraverserBreadth<BasicTraits,SingleTraits>::apply 
(const GeomObjectType& node)
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
inline bool SingleTraverserBreadth<BasicTraits,SingleTraits>::apply 
(CacheData& data, BVolAdapterBase* node, const TransformType& m)
{
   GroupType* root = static_cast<GroupType*>(node);
   assert(root != NULL);
   if (initFunc.call(root, m)) { 
      m_depth = 0; m_num = 1;
      m_cache.push(root);
      if (roundFunc() == false) {
	 return false;
      }
      while (!m_cache.empty()) {
	 traverse(static_cast<GeneralType*>(m_cache.front()));
	 m_cache.pop();
	 if (--m_num == 0) {
 	    ++m_depth; m_num = m_cache.size();
	    if (roundFunc() == false) {
	       return false;
	    }
	 }
      }
      return true;
   } 
   return false;
}

template <class BasicTraits, class SingleTraits>
inline void SingleTraverserBreadth<BasicTraits,SingleTraits>::traverse (GeneralType* node)
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
	    m_cache.push(group->getSon(i));   
	 }
      }
   } else { // leaf node
      AdapterType* leaf = dynamic_cast<AdapterType*>(node);
      leafFunc.call(leaf);
   }
}

END_GENVIS_NAMESPACE
#endif
