//=============================================================================
//                                                                            
//                               Genvis                                     
//        Copyright (C) 2001 by Institute of Computer Graphics, TU Braunschweig
//                           graphics.tu-bs.de                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2004/12/22 18:45:01 $
//                                                                            
//=============================================================================

#include <algorithm>
#include "OSGGVOracleInstance.h"
#include "OSGGVSingleton.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumSIC<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumSIC<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumSIC<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumSIC<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumSIC<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumSIC<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumVolume<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumVolume<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumVolume<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumVolume<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumVolume<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<MinimumVolume<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian2<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian2<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian2<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian2<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian2<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMedian2<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean2<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean2<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean2<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean2<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean2<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<LongestSideMean2<K20Dop> >;

template class OSG_GENVISLIB_DLLMAPPING Singleton<HeuristicGrouping<K6Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<HeuristicGrouping<K14Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<HeuristicGrouping<K18Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<HeuristicGrouping<K26Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<HeuristicGrouping<K12Dop> >;
template class OSG_GENVISLIB_DLLMAPPING Singleton<HeuristicGrouping<K20Dop> >;

// explicit template instantiations (repetition from OSGGVOracle.cpp)
template class OSG_GENVISLIB_DLLMAPPING Oracle<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING Oracle<K20Dop>;

template <class BVOL>
Oracle<BVOL>* Oracle<BVOL>::getOracle (u32  i,
				       bool nullFail)
{
   if (i < getOraclesInternal().size()) {
      return getOraclesInternal()[i];
   } 
   if (!nullFail) {
      return &Singleton<LongestSideMedian<BVOL> >::the();
   }
   return NULL;
}
template <class BVOL>
Oracle<BVOL>* Oracle<BVOL>::getOracle (const char* description,
				       bool  nullFail)
{
   for (u32 i=0; i<getOraclesInternal().size(); ++i) {
     if (strcmp(getOraclesInternal()[i]->getDescription(), description) == 0) {
        return getOraclesInternal()[i];
     }
   }
   if (!nullFail) {
      return &Singleton<LongestSideMedian<BVOL> >::the();
   }
   return NULL;
}


template <class BVOL>
MinimumSIC<BVOL>::MinimumSIC ()
  : MainAxesSorted<BVOL>()
{
   registerOracle(this);
}

template <class BVOL>
bool MinimumSIC<BVOL>::doSplitting (std::vector< std::vector<u32> >& index,
				    const std::vector<Adapter*>&  allNodes,
				    const VectorIndices& range,
				    const BoundingVolume<Real>& bvol,
				    u32& dimension, 
				    u32& position)
{
   u32 numNodes = allNodes.size();
   m_leftCosts.resize(numNodes);
   for (u32 k=0; k<numNodes; ++k) {
      m_leftCosts[k] = 0.0;
   }
   m_rightCosts = m_leftCosts;

   position  = 0;
   dimension = 0;
   Real minCosts = splitCosts(index[dimension], allNodes, range, position);

   for (u32 dim=1; dim<getDimension(); ++dim) {
      u32 pos = 0;
      Real costs = splitCosts(index[dim], allNodes, range, pos);
      if (costs < minCosts) {
         minCosts  = costs;
         position  = pos;
         dimension = dim;
      }
   }

#ifdef GV_VERBOSE
   if (GV_verbose) {
      GV_stream << "MinimumSIC<BVOL>::doSplitting(dim " << dimension 
		<< ", pos " << position 
		<< "/" << allNodes.size() 
		<< ", costs " << minCosts
		<< ")" 
		<< std::endl;
   }
#endif
   return true;
}

template <class BVOL>
Real MinimumSIC<BVOL>::splitCosts (const std::vector<u32>& index,
				   const std::vector<Adapter*>& allNodes,
				   const VectorIndices&  range,
				   u32&             position)
{
   const Real InfCosts = 1e20;
   if (index.size() < 2) {
      position = 0;
      return InfCosts;
   }

   u32 pos;
   BVOL left, right;
   for (pos=1; pos<range.size(); ++pos) {
      // calculate bvol left of {n_i, n_i+1, .., n_i+pos-1}
      // bvol right of {n_i+size-1, n_i+size-2, .., n_i+size-pos} 
      u32 pos1 = pos-1;
      u32 pos2 = range.size()-pos1-1;
      unify(left,  static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos1+range.first]])->getBVol());
      unify(right, static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos2+range.first]])->getBVol());
      m_leftCosts[pos1+range.first]  = pos * left.getSurfaceArea();
      m_rightCosts[pos1+range.first] = pos * right.getSurfaceArea();
   }

   Real minCosts = InfCosts;
   for (pos=1; pos<range.size(); ++pos) {
      u32 pos1 = pos-1;
      u32 pos2 = range.size()-pos1-2;
      Real costs = m_leftCosts[pos1+range.first] + m_rightCosts[pos2+range.first];
      if (costs < minCosts) {
         minCosts = costs;
         position = pos;
      }
   }

   return minCosts;
}



template <class BVOL>
inline MinimumVolume<BVOL>::MinimumVolume ()
  : MinimumSIC<BVOL>()
{
   registerOracle(this);
}

template <class BVOL>
Real MinimumVolume<BVOL>::splitCosts (const std::vector<u32>& index,
				      const std::vector<Adapter*>& allNodes,
				      const VectorIndices& range,
				      u32& position)
{
   u32 numNodes = allNodes.size();
   m_leftCosts.resize(numNodes);
   for (u32 k=0; k<numNodes; ++k) {
      m_leftCosts[k] = 0.0;
   }
   m_rightCosts = m_leftCosts;

   const Real InfCosts = 1e20;
   if (index.size() < 2) {
      position = 0;
      return InfCosts;
   }

   u32 pos;
   BVOL left, right;
   for (pos=1; pos<range.size(); ++pos)
   {
      // calculate bvol left of {n_i, n_i+1, .., n_i+pos-1}
      // bvol right of {n_i+size-1, n_i+size-2, .., n_i+size-pos} 
      u32 pos1 = pos-1;
      u32 pos2 = range.size()-pos1-1;
      unify(left,  static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos1+range.first]])->getBVol());
      unify(right, static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos2+range.first]])->getBVol());
      m_leftCosts[pos1+range.first]  = left.getVolume();
      m_rightCosts[pos1+range.first] = right.getVolume();
   }

   Real minCosts = InfCosts;
   for (pos=1; pos<range.size(); ++pos) {
      u32 pos1 = pos-1;
      u32 pos2 = range.size()-pos1-2;
      Real costs = m_leftCosts[pos1+range.first] + m_rightCosts[pos2+range.first];
      if (costs < minCosts) {
         minCosts = costs;
         position = pos;
      }
   }

   return minCosts;
}



template <class BVOL>
inline LongestSideMedian<BVOL>::LongestSideMedian ()
{
   registerOracle(this);
}

template <class BVOL>
bool LongestSideMedian<BVOL>::doSplitting (std::vector< std::vector<u32> >& index,
					   const std::vector<Adapter*>&          allNodes,
					   const VectorIndices&                  range,
					   const BoundingVolume<Real>&           bvol,
					   u32& d, 
					   u32& i)
{
   // find longest axis of bvol
   d = 0;
   if (bvol.difference(1) > bvol.difference(d)) {
      d = 1;
   }
   if (bvol.difference(2) > bvol.difference(d)) {
      d = 2;
   }
   i = range.size()/2;
   return true;
}


template <class BVOL>
inline LongestSideMean<BVOL>::LongestSideMean ()
{
   registerOracle(this);
}

template <class BVOL>
bool LongestSideMean<BVOL>::doSplitting (std::vector< std::vector<u32> >& index,
					 const std::vector<Adapter*>&  allNodes,
					 const VectorIndices&          range,
					 const BoundingVolume<Real>&   bvol,
					 u32& d, 
					 u32& i)
{
   // find longest axis of bvol
   d = 0;
   if (bvol.difference(1) > bvol.difference(d)) {
      d = 1;
   }
   if (bvol.difference(2) > bvol.difference(d)) {
      d = 2;
   }

   // calculate mean of centroids
   const std::vector<u32>& n = index[d];
   Real mean = 0.0;
   for (u32 pos=range.first; pos<range.last; ++pos) {
      mean += static_cast<BVolAdapterBase*>(allNodes[n[pos]])->getCenter()[d];
   }
   mean /= Real(range.size());
   // find split position
   for (i=1; i<range.size()-1; ++i) {
      if (static_cast<BVolAdapterBase*>(allNodes[n[i+range.first]])->getCenter()[d] > mean) {
	 break;
      }
   }
   return true;
}


// For k-DOPs only!
template <class BVOL>
inline LongestSideMedian2<BVOL>::LongestSideMedian2 ()
{
}

template <class BVOL>
bool LongestSideMedian2<BVOL>::doSplitting (std::vector< std::vector<u32> >& index,
					    const std::vector<Adapter*>&  allNodes,
					    const VectorIndices&                  range,
					    const BoundingVolume<Real>&   bvol,
					    u32& d, 
					    u32& i)
{
   const BVOL& dop = static_cast<const BVOL&>(bvol);
   // choose splitting axis
   d = 0;
   for (u32 k=1; k<BVOL::Size; ++k) {
      if (dop.difference(k) > dop.difference(d)) {
	 d = k;
      }
   }
   // split at median
   i = range.size()/2;
   return true;
}


template <class BVOL>
inline LongestSideMean2<BVOL>::LongestSideMean2 ()
{
   registerOracle(this);
}

template <class BVOL>
bool LongestSideMean2<BVOL>::doSplitting (std::vector< std::vector<u32> >& index,
					  const std::vector<Adapter*>&  allNodes,
					  const VectorIndices&                        range,
					  const BoundingVolume<Real>&   bvol,
					  u32& d, 
					  u32& i)
{
   const BVOL& dop = static_cast<const BVOL&>(bvol);
   // choose splitting axis
   d = 0;
   for (u32 k=1; k<BVOL::Size; ++k) {
      if (dop.difference(k) > dop.difference(d)) {
	 d = k;
      }
   }

   // calculate mean of centroids
   const std::vector<u32>& n = index[d];
   Real mean = 0.0;
   for (u32 pos=range.first; pos<range.last; ++pos) {
      mean += static_cast<BVolAdapterBase*>(allNodes[n[pos]])->getCenter()[d];
   }
   mean /= Real(range.size());
   // find split position
   for (i=1; i<range.size()-1; ++i) {
      if (BVOL::getDirection()[d].dot(static_cast<BVolAdapterBase*>(allNodes[n[i+range.first]])->getCenter()) > mean) {
	 break;
      }
   }
   return true;
}




template <class BVOL>
inline HeuristicGrouping<BVOL>::HeuristicGrouping ()
{
   registerOracle(this);
}

template <class REAL, int SIZE>
static REAL approximateVolume (const KDop<REAL,SIZE>& dop)
{
   REAL value = -1;
   for (u32 k=0; k<SIZE; ++k) {
      value = stdMax(value, dop.difference(k));
   }
   return value;
}

template <class BVOL>
bool HeuristicGrouping<BVOL>::doSplitting (std::vector< std::vector<u32> >& index,
					   const std::vector<Adapter*>&          allNodes,
					   const VectorIndices&                  range,
					   const BoundingVolume<Real>& bvol,
					   u32& dim, 
					   u32& split)
{
   const BVOL& dop = static_cast<const BVOL&>(bvol);
   // find longest axis of bvol
   dim = 0;
   u32 k;
   for (k=1; k<BVOL::Size; ++k) {
      if (dop.difference(k) > dop.difference(dim)) {
	 dim = k;
      }
   }

   std::vector<u32> leftObj;  leftObj.reserve(allNodes.size());
   std::vector<u32> rightObj; rightObj.reserve(allNodes.size());

   BVOL left(static_cast<BVolAdapter<BVOL>*>(allNodes[index[dim][range.first]])->getBVol());
   BVOL right(static_cast<BVolAdapter<BVOL>*>(allNodes[index[dim][range.last-1]])->getBVol());

   leftObj.push_back(index[dim][range.first]);
   u32 i;
   for (i=range.first+1; i<range.last-1; ++i) {
      // add allNodes[index[dim][i]] to left and right
      BVOL leftCopy(left);
      unify(leftCopy,  static_cast<BVolAdapter<BVOL>*>(allNodes[index[dim][i]])->getBVol());
      BVOL rightCopy(right);
      unify(rightCopy, static_cast<BVolAdapter<BVOL>*>(allNodes[index[dim][i]])->getBVol());

      // choose left or right according to which one is smaller
      Real leftCost  = ::approximateVolume(leftCopy);
      Real rightCost = ::approximateVolume(rightCopy);
      if (leftCost < rightCost) { // left
	 left = leftCopy;
	 leftObj.push_back(index[dim][i]);
      } else if (leftCost > rightCost) { // right
	 right = rightCopy;
	 rightObj.push_back(index[dim][i]);
      } else {
	 // decide on numbers
	 if (leftObj.size() <= rightObj.size()) { // left
	    left = leftCopy;
	    leftObj.push_back(index[dim][i]);
	 } else { // right
	    right = rightCopy;
	    rightObj.push_back(index[dim][i]);
	 }
      }
   }
   rightObj.push_back(index[dim][range.last-1]);

   // rearrange array index[dim]
   for (i=1; i<leftObj.size(); ++i) {
      index[dim][i+range.first] = leftObj[i];
   }
   split = leftObj.size();
   for (i=0; i<rightObj.size(); ++i) {
      index[dim][i+range.first+split] = rightObj[i];
   }
#ifdef GV_VERBOSE
   if (GV_verbose) {
      GV_stream << "HeuristicGrouping<BVOL>::doSplitting"
		<< "(numLeft=" << leftObj.size() 
		<< ", numRight=" << rightObj.size() << ")" 
		<< std::endl;
   }
#endif
   return true;
}





