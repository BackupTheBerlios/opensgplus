//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.1 $
//   $Date: 2003/09/11 16:20:29 $
//                                                                            
//=============================================================================

#include "OSGGVOptimizer.h"
#include "OSGGVBVolAdapter.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroup< K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroup<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroup<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroup<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroup<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Optimizer<BVolGroup<K20Dop> >;


template <class GROUP>
Optimizer<GROUP>::~Optimizer ()
{
   // m_oracle is not deleted by this object!
   // This is the responsibility of the user!
}

template <class GROUP>
Optimizer<GROUP>::Optimizer (Oracle*                oracle,
			     const std::vector<Adapter*>& leafNodes,
			     unsigned maxDepth,
			     unsigned minObjects)
  : m_allNodes(&leafNodes)
{
   m_store = NULL;
   m_currentLevel = 0;
   m_maxLevel   = maxDepth;
   m_minObjects = minObjects;
   m_oracle = oracle;
}

template <class GROUP>
GROUP* Optimizer<GROUP>::optimize ()
{
   if (getLeafNodes().size() == 0) {
      GroupType* base = FactoryType::the().newObject();
      getDataStore().push_back(base);
      return base;
   }
   if (getLeafNodes().size() == 1) {
      GroupType* base = FactoryType::the().newObject();
      getDataStore().push_back(base);
      base->getSons().push_back(static_cast<BVolAdapterBase*>(getLeafNodes()[0]));
      return base;
   }

   VectorIndices fullRange;
   fullRange.first = 0;
   fullRange.last  = getLeafNodes().size();

   // create m_oracle->dimensions() index arrays m_nodes
#if 0
   unsigned dim;
   m_nodes.resize(m_oracle->dimensions());
   for (dim=0; dim<m_oracle->dimensions(); ++dim) {
      m_nodes[dim].resize(getLeafNodes().size());
      for (unsigned s=0; s<getLeafNodes().size(); ++s) {
         m_nodes[dim][s] = s;
      }
   }
#else
   unsigned d;
   m_nodes.resize(m_oracle->dimensions());
   m_nodes[0].reserve(getLeafNodes().size());
   for (unsigned s=0; s<getLeafNodes().size(); ++s) {
      m_nodes[0].push_back(s);
   }
   for (d=1; d<m_oracle->dimensions(); ++d) {
      m_nodes[d].resize(getLeafNodes().size());
      std::copy(m_nodes[0].begin(), m_nodes[0].end(), m_nodes[d].begin());
   }
#endif

   for (d = 0; d<m_oracle->dimensions(); ++d) {
      m_oracle->sortIndexList(m_nodes[d], getLeafNodes(), d);
   }
   BVol bvol; 
   m_oracle->calcBVOL(m_nodes, getLeafNodes(), fullRange, bvol);

   m_currentLevel = 0;
   return static_cast<GroupType*>(optimize(fullRange, bvol, 0));
}

template <class GROUP>
BVolAdapterBase* Optimizer<GROUP>::optimize (const VectorIndices& range,
					     BVol&                bvol,
					     unsigned             level)
{
   if (level > m_currentLevel) {
      m_currentLevel = level;
   }
   if (   level < m_maxLevel 
       && range.size() > m_minObjects) {
      // create group node
      GroupType* base = FactoryType::the().newObject();
      getDataStore().push_back(base);
      base->init(range.first, range.last, m_nodes, getLeafNodes());
      //base->getBoundingVolumeInternal() = bvol;
      //base->setValid();
      // find split position
      unsigned pos, dim;
      if (!m_oracle->doSplitting(m_nodes, getLeafNodes(), range, bvol, dim, pos)) {
	 std::vector<BVolAdapterBase*>& sons = base->getSons(); sons.reserve(range.size());
	 for (unsigned i=range.first; i<range.last; ++i) {
	    sons.push_back(static_cast<BVolAdapterBase*>(getLeafNodes()[m_nodes[0][i]]));
	    static_cast<BVolAdapterBase*>(getLeafNodes()[m_nodes[0][i]])->setParent(base);
	 }
	 return base;
      }

      // update sorted lists und subdivide into sublists
      VectorIndices range1, range2;
      subdivSorted (range, range1, range2, pos, dim);

      // continue on sublists
      BVol& bvolLeft(bvol);
      BVol  bvolRight(bvol);
      m_oracle->setMinimum(m_nodes, getLeafNodes(), dim, range1, bvolLeft);
      BVolAdapterBase* left  = optimize(range1, bvolLeft,  level+1);
      m_oracle->setMaximum(m_nodes, getLeafNodes(), dim, range2, bvolRight);
      BVolAdapterBase* right = optimize(range2, bvolRight, level+1);

      left ->setParent(base);
      right->setParent(base);
      std::vector<BVolAdapterBase*>& sons = base->getSons(); sons.reserve(2);
      sons.push_back(left);
      sons.push_back(right);
      return base;
   }

   if (range.size() < 2) {
      return static_cast<BVolAdapterBase*>(getLeafNodes()[m_nodes[0][range.first]]);
   }

   // create group
   GroupType* base = FactoryType::the().newObject();
   getDataStore().push_back(base);
   base->init(range.first, range.last, m_nodes, getLeafNodes());
   //base->getBoundingVolumeInternal() = bvol;
   //base->setValid();
   std::vector<BVolAdapterBase*>& sons = base->getSons(); sons.reserve(range.size());
   for (unsigned i=range.first; i<range.last; ++i) {
      sons.push_back(static_cast<BVolAdapterBase*>(getLeafNodes()[m_nodes[0][i]]));
      static_cast<BVolAdapterBase*>(getLeafNodes()[m_nodes[0][i]])->setParent(base);
   }
   return base;
}

template <class GROUP>
void Optimizer<GROUP>::subdivSorted (const VectorIndices& range,
				     VectorIndices& range1,
				     VectorIndices& range2,
				     unsigned       pos, 
				     unsigned       dim)
{
   std::vector<bool> rightTag; rightTag.resize(getLeafNodes().size());
   std::vector<unsigned>::const_iterator end = m_nodes[dim].begin() + range.last;
   unsigned count;
   std::vector<unsigned>::const_iterator iter;
   for (iter=m_nodes[dim].begin() + range.first, count=0;
	iter != end;
	++iter, ++count) {
      rightTag[*iter] = (count < pos) ? false : true;
   }

   std::vector<unsigned> temp; temp.resize(getLeafNodes().size());
   for (unsigned d=0; d<m_oracle->dimensions(); ++d) {
      if (d == dim) {
	 continue;
      }

      std::vector<unsigned>::iterator wleft  = temp.begin() + range.first;
      std::vector<unsigned>::iterator wright = wleft + pos;
      end = m_nodes[d].begin() + range.last;
      for (iter = m_nodes[d].begin() + range.first; 
	   iter != end;
	   ++iter) {
	 if (rightTag[*iter]) {
            *wright++ = *iter;
         } else {
            *wleft++  = *iter;
         }
      }
      std::copy (temp.begin() + range.first, temp.begin() + range.last, m_nodes[d].begin() + range.first);
   }

   range1.first = range.first;
   range1.last  = range2.first = range.first + pos;
   range2.last  = range.last;
}
