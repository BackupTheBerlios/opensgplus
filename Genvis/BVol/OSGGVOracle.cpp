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

#include <algorithm>
#include "OSGGVOracle.h"
#include "OSGGVBVolFunctions.h"
USING_GENVIS_NAMESPACE

// explicit template instantiations
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING MainAxesSorted<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING MinimumSIC<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumSIC<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumSIC<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumSIC<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumSIC<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumSIC<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING MinimumVolume<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumVolume<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumVolume<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumVolume<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumVolume<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING MinimumVolume<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING LongestSideMean<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING LongestSideMean2<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean2<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean2<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean2<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean2<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING LongestSideMean2<K20Dop>;

template class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping<K6Dop>;
template class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping<K14Dop>;
template class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping<K18Dop>;
template class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping<K26Dop>;
template class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping<K12Dop>;
template class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping<K20Dop>;


class ComponentCompare
{
public:
   inline ComponentCompare    (unsigned dimension,
			  const std::vector<Adapter*>& allNodes);

  inline bool operator() (unsigned node1, unsigned node2) const;

protected:
   unsigned m_dimension;
   const std::vector<Adapter*>& m_allNodes;
};
inline ComponentCompare::ComponentCompare (unsigned dimension,
					   const std::vector<Adapter*>& allNodes)
  : m_dimension(dimension), m_allNodes(allNodes)
{
}
inline bool ComponentCompare::operator() (unsigned node1, unsigned node2) const
{
   return static_cast<BVolAdapterBase*>(m_allNodes[node1])->getCenter()[m_dimension]
     < static_cast<BVolAdapterBase*>(m_allNodes[node2])->getCenter()[m_dimension];
}

class ProjCompare
{
public:
   inline ProjCompare     (const VectorClass& direction,
			   const std::vector<Adapter*>& allNodes);

   inline bool operator() (unsigned node1, unsigned node2);

protected:
   VectorClass m_direction;
   const std::vector<Adapter*>& m_allNodes;
};
inline ProjCompare::ProjCompare (const VectorClass& direction,
				 const std::vector<Adapter*>& allNodes)
  : m_direction(direction), m_allNodes(allNodes)
{
}
inline bool ProjCompare::operator() (unsigned node1, unsigned node2)
{
   Real value1 = m_direction.dot(static_cast<BVolAdapterBase*>(m_allNodes[node1])->getCenter());
   Real value2 = m_direction.dot(static_cast<BVolAdapterBase*>(m_allNodes[node2])->getCenter());
   return value1 < value2;
}


template <class BVOL>
bool MainAxesSorted<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
					const std::vector<Adapter*>&          allNodes,
					const VectorIndices&                  range,
					const BoundingVolume<Real>&           bvol,
					unsigned&                             d, 
					unsigned&                             i)
{
   i = 1;
   d = 0;
   return true;
}

template <class BVOL>
void MainAxesSorted<BVOL>::sortIndexList(std::vector<unsigned>& index,
					 const std::vector<Adapter*>& allNodes,
					 unsigned dimension)
{
   std::vector<unsigned>::iterator begin = index.begin();
   std::vector<unsigned>::iterator end   = index.end();
   std::sort(begin, end, ComponentCompare(dimension, allNodes));
}

template <class BVOL>
bool MinimumSIC<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
				    const std::vector<Adapter*>&  allNodes,
				    const VectorIndices& range,
				    const BoundingVolume<Real>& bvol,
				    unsigned& dimension, 
				    unsigned& position)
{
   unsigned numNodes = allNodes.size();
   m_leftCosts.resize(numNodes);
   for (unsigned k=0; k<numNodes; ++k) {
      m_leftCosts[k] = 0.0;
   }
   m_rightCosts = m_leftCosts;

   position  = 0;
   dimension = 0;
   Real minCosts = splitCosts(index[dimension], allNodes, range, position);

   for (unsigned dim=1; dim<dimensions(); ++dim) {
      unsigned pos = 0;
      Real costs = splitCosts(index[dim], allNodes, range, pos);
      if (costs < minCosts) {
         minCosts  = costs;
         position  = pos;
         dimension = dim;
      }
   }

#ifdef GV_VERBOSE
   std::cout << "MinimumSIC<BVOL>::doSplitting(dim " << dimension 
	     << ", pos " << position 
	     << "/" << allNodes.size() 
	     << ", costs " << minCosts
	     << ")" << std::endl;
#endif
   return true;
}

template <class BVOL>
Real MinimumSIC<BVOL>::splitCosts (const std::vector<unsigned>& index,
				   const std::vector<Adapter*>& allNodes,
				   const VectorIndices&  range,
				   unsigned&             position)
{
   const Real InfCosts = 1e20;
   if (index.size() < 2) {
      position = 0;
      return InfCosts;
   }

   unsigned pos;
   BVOL left, right;
   for (pos=1; pos<range.size(); ++pos) {
      // calculate bvol left of {n_i, n_i+1, .., n_i+pos-1}
      // bvol right of {n_i+size-1, n_i+size-2, .., n_i+size-pos} 
      unsigned pos1 = pos-1;
      unsigned pos2 = range.size()-pos1-1;
      unify(left,  static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos1+range.first]])->getBVol());
      unify(right, static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos2+range.first]])->getBVol());
      m_leftCosts[pos1+range.first]  = pos * left.getSurfaceArea();
      m_rightCosts[pos1+range.first] = pos * right.getSurfaceArea();
   }

   Real minCosts = InfCosts;
   for (pos=1; pos<range.size(); ++pos) {
      unsigned pos1 = pos-1;
      unsigned pos2 = range.size()-pos1-2;
      Real costs = m_leftCosts[pos1+range.first] + m_rightCosts[pos2+range.first];
      if (costs < minCosts) {
         minCosts = costs;
         position = pos;
      }
   }

   return minCosts;
}

template <class BVOL>
Real MinimumVolume<BVOL>::splitCosts (const std::vector<unsigned>& index,
				      const std::vector<Adapter*>& allNodes,
				      const VectorIndices& range,
				      unsigned& position)
{
   unsigned numNodes = allNodes.size();
   m_leftCosts.resize(numNodes);
   for (unsigned k=0; k<numNodes; ++k) {
      m_leftCosts[k] = 0.0;
   }
   m_rightCosts = m_leftCosts;

   const Real InfCosts = 1e20;
   if (index.size() < 2) {
      position = 0;
      return InfCosts;
   }

   unsigned pos;
   BVOL left, right;
   for (pos=1; pos<range.size(); ++pos)
   {
      // calculate bvol left of {n_i, n_i+1, .., n_i+pos-1}
      // bvol right of {n_i+size-1, n_i+size-2, .., n_i+size-pos} 
      unsigned pos1 = pos-1;
      unsigned pos2 = range.size()-pos1-1;
      unify(left,  static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos1+range.first]])->getBVol());
      unify(right, static_cast<BVolAdapter<BVOL>*>(allNodes[index[pos2+range.first]])->getBVol());
      m_leftCosts[pos1+range.first]  = left.getVolume();
      m_rightCosts[pos1+range.first] = right.getVolume();
   }

   Real minCosts = InfCosts;
   for (pos=1; pos<range.size(); ++pos) {
      unsigned pos1 = pos-1;
      unsigned pos2 = range.size()-pos1-2;
      Real costs = m_leftCosts[pos1+range.first] + m_rightCosts[pos2+range.first];
      if (costs < minCosts) {
         minCosts = costs;
         position = pos;
      }
   }

   return minCosts;
}


template <class BVOL>
bool LongestSideMedian<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
					   const std::vector<Adapter*>&          allNodes,
					   const VectorIndices&                  range,
					   const BoundingVolume<Real>&           bvol,
					   unsigned& d, 
					   unsigned& i)
{
   MainAxesSorted<BVOL>::doSplitting (index, allNodes, range, bvol, d, i);
   // find longest axis of bvol
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
bool LongestSideMean<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
					 const std::vector<Adapter*>&  allNodes,
					 const VectorIndices&          range,
					 const BoundingVolume<Real>&   bvol,
					 unsigned& d, 
					 unsigned& i)
{
   LongestSideMedian<BVOL>::doSplitting (index, allNodes, range, bvol, d, i);
   // calculate mean of centroids
   const std::vector<unsigned>& n = index[d];
   Real mean = 0.0;
   for (unsigned pos=range.first; pos<range.last; ++pos) {
      mean += static_cast<BVolAdapterBase*>(allNodes[n[pos]])->getCenter()[d];
   }
   mean *= 1.0/range.size();

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
void LongestSideMedian2<BVOL>::sortIndexList(std::vector<unsigned>& index,
					      const std::vector<Adapter*>& allNodes,
					      unsigned d)
{
   std::vector<unsigned>::iterator begin = index.begin();
   std::vector<unsigned>::iterator end   = index.end();
   std::sort(begin, end, ProjCompare(BVOL::getDirection()[d], allNodes));
}
template <class BVOL>
bool LongestSideMedian2<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
					    const std::vector<Adapter*>&  allNodes,
					    const VectorIndices&                  range,
					    const BoundingVolume<Real>&   bvol,
					    unsigned& d, 
					    unsigned& i)
{
   MainAxesSorted<BVOL>::doSplitting (index, allNodes, range, bvol, d, i);
   const BVOL& dop = static_cast<const BVOL&>(bvol);
   // choose splitting axis
   for (unsigned k=1; k<BVOL::Size; ++k) {
      if (dop.difference(k) > dop.difference(d)) {
	 d = k;
      }
   }
   // split at median
   i = range.size()/2;
   return true;
}


template <class BVOL>
bool LongestSideMean2<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
					  const std::vector<Adapter*>&  allNodes,
					  const VectorIndices&                        range,
					  const BoundingVolume<Real>&   bvol,
					  unsigned& d, 
					  unsigned& i)
{
   LongestSideMedian2<BVOL>::doSplitting (index, allNodes, range, bvol, d, i);
   // calculate mean of centroids
   const std::vector<unsigned>& n = index[d];
   Real mean = 0.0;
   for (unsigned pos=range.first; pos<range.last; ++pos) {
      mean += static_cast<BVolAdapterBase*>(allNodes[n[pos]])->getCenter()[d];
   }
   mean *= 1.0/range.size();

   // find split position
   for (i=1; i<range.size()-1; ++i) {
      if (BVOL::getDirection()[d].dot(static_cast<BVolAdapterBase*>(allNodes[n[i+range.first]])->getCenter()) > mean) {
	 break;
      }
   }
   return true;
}


template <class BVOL>
void HeuristicGrouping<BVOL>::sortIndexList (std::vector<unsigned>& index,
					     const std::vector<Adapter*>& allNodes,
					     unsigned d)
{
   std::vector<unsigned>::iterator begin = index.begin();
   std::vector<unsigned>::iterator end   = index.end();
   std::sort(begin, end, ProjCompare(BVOL::getDirection()[d], allNodes));
}

template <class REAL, int SIZE>
static REAL approximateVolume (const KDop<REAL,SIZE>& dop)
{
   REAL value = 0;
   for (unsigned k=0; k<SIZE; ++k) {
      value += dop.difference(k);
   }
   return value;
}

template <class BVOL>
bool HeuristicGrouping<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
					   const std::vector<Adapter*>&          allNodes,
					   const VectorIndices&                  range,
					   const BoundingVolume<Real>& bvol,
					   unsigned& dim, 
					   unsigned& split)
{
   const BVOL& dop = static_cast<const BVOL&>(bvol);
   // find longest axis of bvol
   unsigned k;
   for (k=1; k<BVOL::Size; ++k) {
      if (dop.difference(k) > dop.difference(dim)) {
	 dim = k;
      }
   }

   std::vector<unsigned> leftObj;  leftObj.reserve(allNodes.size());
   std::vector<unsigned> rightObj; rightObj.reserve(allNodes.size());

   BVOL left(static_cast<BVolAdapter<BVOL>*>(allNodes[index[dim][range.first]])->getBVol());
   BVOL right(static_cast<BVolAdapter<BVOL>*>(allNodes[index[dim][range.last-1]])->getBVol());

   leftObj.push_back(index[dim][range.first]);
   unsigned i;
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
   std::cout << "HeuristicGrouping<BVOL>::doSplitting"
	     << "(numLeft=" << leftObj.size() 
	     << ", numRight=" << rightObj.size() << ")" 
	     << std::endl;
   return true;
}

#if 0
#include "OSGGVTriangleAdapterAligned.h"
template <class BVOL>
bool LongestSideMedianAligned<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
						  const std::vector<Adapter*>&  allNodes,
						  const VectorIndices&                  range,
						  const BoundingVolume<Real>&   bvol,
						  unsigned& dimension, 
						  unsigned& split)
{
   MainAxesSorted<BVOL>::doSplitting (index, allNodes, range, bvol, dimension, split);
   // find longest axis of bvol
   const BVOL& dop = static_cast<const BVOL&>(bvol);
   unsigned k;
   for (unsigned pos=range.first; pos<range.last; ++pos) {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* adapter = 
	static_cast<OpenSGTriangleAligned<OpenSGTraits,BVOL>*>(allNodes[index[0][pos]]);

      // translate dop to correct center
      BVOL thisDop(adapter->getBVol());
      for (k=0; k<BVOL::Size; ++k) {
	 thisDop.minVector()[k] += adapter->getRotationCenter()[k];
	 thisDop.maxVector()[k] += adapter->getRotationCenter()[k];
      }
      thisDop.update();

      unify(dop, thisDop);
   }
   // choose splitting axis
   for (k=1; k<BVOL::Size; ++k) {
      if (dop.difference(k) > dop.difference(dimension)) {
	 dimension = k;
      }
   }
   // split at median
   split = range.size()/2;
   return true;
}


template <class REAL, int SIZE>
static void calcKDOP (const std::vector<unsigned>&         index,
		      const std::vector<Adapter*>& nodes,
		      KDop<REAL,SIZE>&                     bvol)
{  
   typedef KDop<REAL,SIZE> BVOL;

   unsigned i, k;
   const unsigned num = 20;
   VectorClass d[SIZE*num];
   for (i=0; i<SIZE; ++i) {
      calcRegularPolytope<REAL,SIZE>(i, num, d+i*num);
   }

   std::vector<unsigned>::const_iterator iter; 
   std::vector<unsigned>::const_iterator end = index.end(); 
   // calc center
   PointClass center;
   for (iter = index.begin(); 
	iter != end;
	++iter) {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	((OpenSGTriangleAligned<OpenSGTraits,BVOL>*)nodes[*iter]);
      center += VectorClass(leaf->getPosition(0));
      center += VectorClass(leaf->getPosition(1));
      center += VectorClass(leaf->getPosition(2));
   }
   center /= 3*Real(index.size());


   PointClass p;
   VectorClass local;
   iter = index.begin(); 
   {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	((OpenSGTriangleAligned<OpenSGTraits,BVOL>*)nodes[*iter]);
      p = leaf->getPosition(0); 
      local = p - center;
      for (k=0; k<SIZE; ++k) {
	 if (testIntersectSector<REAL,SIZE>(k, num, d, local)) {
	    bvol.maxVector()[k] = local.length();
	    bvol.minVector()[k] = GV_dot(d[(num*k)], local);
	    for (i=1; i<num; ++i) {
	       bvol.minVector()[k] = stdMin(bvol.minVector()[k], GV_dot(d[(num*k)+i], local));
	    }
	 } else if (testIntersectSector<REAL,SIZE>(k, num, d, -local)) {
	    bvol.maxVector()[k] = GV_dot(d[(num*k)], local);
	    for (i=1; i<num; ++i) {
	       bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], GV_dot(d[(num*k)+i], local));
	    }
	    bvol.minVector()[k] = -(local.length());
	 } else {
	    bvol.maxVector()[k] = bvol.minVector()[k] = GV_dot(d[num*k], local);
	    for (i=1; i<num; ++i) {
	       Real s = GV_dot(d[(num*k)+i], local);
	       bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], 
					      s);
	       bvol.minVector()[k] = stdMin(bvol.minVector()[k], 
					      s);
	    }
	 }
      }
      p = leaf->getPosition(1); 
      local = p - center;
      for (k=0; k<SIZE; ++k) {
	 if (testIntersectSector<REAL,SIZE>(k, num, d, local)) {
	    bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], 
					   local.length());
	 } else if (testIntersectSector<REAL,SIZE>(k, num, d, -local)) {
	    bvol.minVector()[k] = stdMin(bvol.minVector()[k], 
					   -(local.length()));
	 } else {
	    for (i=0; i<num; ++i) {
	       Real s = GV_dot(d[(num*k)+i], local);
	       bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], 
					      s);
	       bvol.minVector()[k] = stdMin(bvol.minVector()[k], 
					      s);
	    }
	 }
      }
      p = leaf->getPosition(2); 
      local = p - center;
      for (k=0; k<SIZE; ++k) {
	 if (testIntersectSector<REAL,SIZE>(k, num, d, local)) {
	    bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], 
					   local.length());
	 } else if (testIntersectSector<REAL,SIZE>(k, num, d, -local)) {
	    bvol.minVector()[k] = stdMin(bvol.minVector()[k], 
					   -(local.length()));
	 } else {
	    for (i=0; i<num; ++i) {
	       Real s = GV_dot(d[(num*k)+i], local);
	       bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], s);
	       bvol.minVector()[k] = stdMin(bvol.minVector()[k], s);
	    }
	 }
      }
   }

   for (++iter; 
	iter != end; 
	++iter) {
      OpenSGTriangleAligned<OpenSGTraits,BVOL>* leaf = 
	((OpenSGTriangleAligned<OpenSGTraits,BVOL>*)nodes[*iter]);
      p = leaf->getPosition(0); 
      local = p - center;
      for (k=0; k<SIZE; ++k) {
	 if (testIntersectSector<REAL,SIZE>(k, num, d, local)) {
	    bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], local.length());
	 } else if (testIntersectSector<REAL,SIZE>(k, num, d, -local)) {
	    bvol.minVector()[k] = stdMin(bvol.minVector()[k], -(local.length()));
	 } else {
	    for (i=0; i<num; ++i) {
	       Real s = GV_dot(d[(num*k)+i], local);
	       bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], s);
	       bvol.minVector()[k] = stdMin(bvol.minVector()[k], s);
	    }
	 }
      }
      p = leaf->getPosition(1); 
      local = p - center;
      for (k=0; k<SIZE; ++k) {
	 if (testIntersectSector<REAL,SIZE>(k, num, d, local)) {
	    bvol.maxVector()[k] = stdMax(bvol.maxVector()[k],
					   local.length());
	 } else if (testIntersectSector<REAL,SIZE>(k, num, d, -local)) {
	    bvol.minVector()[k] = stdMin(bvol.minVector()[k],
					   -(local.length()));
	 } else {
	    for (i=0; i<num; ++i) {
	       Real s = GV_dot(d[(num*k)+i], local);
	       bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], 
					      s);
	       bvol.minVector()[k] = stdMin(bvol.minVector()[k], 
					      s);
	    }
	 }
      }
      p = leaf->getPosition(2); 
      local = p - center;
      for (k=0; k<SIZE; ++k) {
	 if (testIntersectSector<REAL,SIZE>(k, num, d, local)) {
	    bvol.maxVector()[k] = stdMax(bvol.maxVector()[k],
					   local.length());
	 } else if (testIntersectSector<REAL,SIZE>(k, num, d, -local)) {
	    bvol.minVector()[k] = stdMin(bvol.minVector()[k],
					   -(local.length()));
	 } else {
	    for (i=0; i<num; ++i) {
	       Real s = GV_dot(d[(num*k)+i], local);
	       bvol.maxVector()[k] = stdMax(bvol.maxVector()[k], 
					      s);
	       bvol.minVector()[k] = stdMin(bvol.minVector()[k], 
					      s);
	    }
	 }
      }
   }

   bvol.setEmpty(false);
   bvol.update();
}


template <class BVOL>
bool HeuristicGroupingAligned<BVOL>::doSplitting (std::vector< std::vector<unsigned> >& index,
						  const std::vector<Adapter*>&  allNodes,
						  const VectorIndices&                  range,
						  const BoundingVolume<Real>&   bvol,
						  unsigned& dim, 
						  unsigned& split)
{
   MainAxesSorted<BVOL>::doSplitting (index, allNodes, range, bvol, dim, split);
   // find longest axis of bvol
   const BVOL& dop = static_cast<const BVOL&>(bvol);
   unsigned k;
   for (k=1; k<BVOL::Size; ++k) {
      if (bvol.difference(k) > bvol.difference(dim)) {
	 dim = k;
      }
   }

   std::vector<unsigned> leftObj;  leftObj.reserve(allNodes.size());
   leftObj.push_back(index[dim][range.first]);
   std::vector<unsigned> rightObj; rightObj.reserve(allNodes.size());
   rightObj.push_back(index[dim][range.last-1]);

   unsigned i;
   for (i=range.first+1; i<range.last-1; ++i) {
      // add allNodes[index[dim][i]] to left and right
      leftObj.push_back(index[dim][i]);
      BVOL left;
      calcKDOP(leftObj, allNodes, left);
      rightObj.insert(rightObj.begin(), index[dim][i]);
      BVOL right;
      calcKDOP(rightObj, allNodes, right);

      // choose left or right according to which one is smaller
      Real leftCost  = ::approximateVolume(left);
      Real rightCost = ::approximateVolume(right);
      if (leftCost < rightCost) { // left
	 rightObj.erase(rightObj.begin());
      } else if (leftCost > rightCost) { // right
	 leftObj.pop_back();
      } else { // if leftCost == rightCost, then decide on numbers
	 if (leftObj.size() <= rightObj.size()) { // left
	    rightObj.erase(rightObj.begin());
	 } else { // right
	    leftObj.pop_back();
	 }
      }
   }

   // rearrange array index[dim]
   for (i=1; i<leftObj.size(); ++i) {
      index[dim][i+range.first] = leftObj[i];
   }
   split = leftObj.size();
   for (i=0; i<rightObj.size(); ++i) {
      index[dim][i+range.first+split] = rightObj[i];
   }
   std::cout << "HeuristicGroupingAligned<BVOL>::doSplitting"
	     << "(numLeft=" << leftObj.size() 
	     << ", numRight=" << rightObj.size() << ")" 
	     << std::endl;
   return true;
}
#endif
