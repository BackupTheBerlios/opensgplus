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

#ifndef OSGGVORACLE_H
#define OSGGVORACLE_H

#include "OSGGVBase.h"
#include "OSGGVIndices.h"
#include "OSGGVBVolAdapterBase.h"
#include "OSGGVBVolFunctions.h"

BEGIN_GENVIS_NAMESPACE

/*! Interface for partitioning adapter vectors into two groups.
*/
class OSG_GENVISLIB_DLLMAPPING Oracle
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline Oracle ();
   virtual inline ~Oracle ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Partitioning.                                                */
   /*! \{                                                                 */
   /*! Number of index lists, the optimizer creates for the adapter list. */
   virtual unsigned dimensions () = 0;
   /*! Sort index list for dimension d. 
       @param index(out)   Index list for dimension d.
       @param allNodes(in) Adapter list from cache.
       @param d(in)        Dimension to be processed. */
   virtual void sortIndexList (std::vector<unsigned>&       index,
			       const std::vector<Adapter*>& allNodes,
			       unsigned                     d) = 0;
   /*! Choose splitting of a subrange of the adapter list.
       @param index(in)    Index lists.
       @param allNodes(in) Adapter list from cache. 
       @param range(in)    Subrange of the index lists. 
       @param bvol(in)     Bounding volume for the subrange of the adapter list.
       @param d(out)       Dimension to be splitted.
       @param i(out)       Splitting index.  
       @return true iff splitting should be performed. */  
   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			     const std::vector<Adapter*>&          allNodes,
			     const VectorIndices&                  range,
			     const BoundingVolume<Real>&           bvol,
			     unsigned&                             d, 
			     unsigned&                             i) = 0;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   static void  calcBoundingVolume (std::vector< std::vector<unsigned> >& index,
				    const std::vector<Adapter*>&          allNodes,
				    const VectorIndices&  range,
				    BoundingVolume<Real>& bvol);
   virtual void calcBVOL (std::vector< std::vector<unsigned> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&  range,
			  BoundingVolume<Real>& bvol);

   virtual void     setMinimum (std::vector< std::vector<unsigned> >& index,
				const std::vector<Adapter*>&          allNodes,
				unsigned,
				const VectorIndices&  range,
				BoundingVolume<Real>& bvol);
   virtual void     setMaximum (std::vector< std::vector<unsigned> >& index,
				const std::vector<Adapter*>&          allNodes,
				unsigned,
				const VectorIndices&  range,
				BoundingVolume<Real>& bvol);
};

inline Oracle::Oracle ()
{
}
inline Oracle::~Oracle ()
{
}
inline void Oracle::calcBoundingVolume (std::vector< std::vector<unsigned> >& index,
					const std::vector<Adapter*>&          allNodes,
					const VectorIndices&  range,
					BoundingVolume<Real>& bvol)
{
   bvol.setEmpty();
   for (unsigned j=range.first; j<range.last; ++j) {
      bvol.unify(static_cast<BVolAdapterBase*>(allNodes[index[0][j]])->getBoundingVolume());
   }
}
inline void Oracle::calcBVOL (std::vector< std::vector<unsigned> >& index,
		      const std::vector<Adapter*>&          allNodes,
		      const VectorIndices&  range,
		      BoundingVolume<Real>& bvol)
{
   return calcBoundingVolume(index, allNodes, range, bvol);
}

inline void Oracle::setMinimum (std::vector< std::vector<unsigned> >& index,
				const std::vector<Adapter*>&          allNodes,
				unsigned,
				const VectorIndices&  range,
				BoundingVolume<Real>& bvol)
{
   calcBVOL (index, allNodes, range, bvol);
}
inline void Oracle::setMaximum (std::vector< std::vector<unsigned> >& index,
				const std::vector<Adapter*>&          allNodes,
				unsigned,
				const VectorIndices&  range,
				BoundingVolume<Real>& bvol)
{
   calcBVOL (index, allNodes, range, bvol);
}


/*! Base for partitioning adapter vector into two groups
    using sorted lists of projections to the main axes.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING MainAxesSorted : public Oracle
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline MainAxesSorted ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Partitioning.                                                */
   /*! \{                                                                 */
   /*! Sort index list for dimension d. 
       @param index(out)   Index list for dimension d.
       @param allNodes(in) Adapter list from cache.
       @param d(in)        Dimension to be processed. */
   virtual void sortIndexList (std::vector<unsigned>&       index,
				const std::vector<Adapter*>& allNodes,
				unsigned                     d);
   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			     const std::vector<Adapter*>&          allNodes,
			     const VectorIndices&                  range,
			     const BoundingVolume<Real>&           bvol,
			     unsigned&                             d, 
			     unsigned&                             i);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
#if 0
   virtual void     setMinimum (std::vector< std::vector<unsigned> >& index,
				const std::vector<Adapter*>&          allNodes,
				unsigned d,
				const VectorIndices&  range,
				BoundingVolume<Real>& bvol);
   virtual void     setMaximum (std::vector< std::vector<unsigned> >& index,
				const std::vector<Adapter*>&          allNodes,
				unsigned d,
				const VectorIndices&  range,
				BoundingVolume<Real>& bvol);
#endif
   virtual void calcBVOL (std::vector< std::vector<unsigned> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&  range,
			  BoundingVolume<Real>& bvol);
};

template <class BVOL>
inline MainAxesSorted<BVOL>::MainAxesSorted ()
{
}
#if 0
template <class BVOL>
inline void MainAxesSorted<BVOL>::setMinimum (std::vector< std::vector<unsigned> >& index,
			const std::vector<Adapter*>&          allNodes,
			unsigned d,
			const VectorIndices&  range,
			BoundingVolume<Real>& bvol)
{
  const BVOL& bv0 = static_cast<BVolAdapter<BVOL>*>(allNodes[index[d][range.first]])->getBVol();
  const BVOL& bv1 = static_cast<BVolAdapter<BVOL>*>(allNodes[index[d][range.last-1]])->getBVol();
  bvol.minVector()[d] = bv0.minVector()[d];
  bvol.maxVector()[d] = bv1.maxVector()[d];
}
template <class BVOL>
inline void MainAxesSorted<BVOL>::setMaximum (std::vector< std::vector<unsigned> >& index,
		 const std::vector<Adapter*>&          allNodes,
		 unsigned d,
		 const VectorIndices&  range,
		 BoundingVolume<Real>& bvol)
{
  const BVOL& bv0 = static_cast<BVolAdapter<BVOL>*>(allNodes[index[d][range.first]])->getBVol();
  const BVOL& bv1 = static_cast<BVolAdapter<BVOL>*>(allNodes[index[d][range.last-1]])->getBVol();
  bvol.minVector()[d] = bv0.minVector()[d];
  bvol.maxVector()[d] = bv1.maxVector()[d];
}
#endif
template <class BVOL>
inline void MainAxesSorted<BVOL>::calcBVOL (std::vector< std::vector<unsigned> >& index,
		      const std::vector<Adapter*>&          allNodes,
		      const VectorIndices&  range,
		      BoundingVolume<Real>& bvol)
{
   bvol.setEmpty();
   BVOL& dop = static_cast<BVOL&>(bvol);
   for (unsigned j=range.first; j<range.last; ++j) {
      unify(dop, static_cast<BVolAdapter<BVOL>*>(allNodes[index[0][j]])->getBVol());
   }
}


/*! Partitioning adapter vector into two groups
    by evaluating a surface-intersection-cost metric 
    (evaluates surface area of bounding volume). The adapter vector
    is then partitioned according to the minimum.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING MinimumSIC : public MainAxesSorted<BVOL>
{
public:
   inline MinimumSIC ();

   virtual unsigned dimensions () { return 3; }

   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&           bvol,
			  unsigned&                             d, 
			  unsigned&                             i);

protected:
   virtual Real splitCosts (const std::vector<unsigned>& nodes,
			    const std::vector<Adapter*>& allNodes,
			    const VectorIndices&  range,
			    unsigned&             position);

   std::vector<Real> m_leftCosts;
   std::vector<Real> m_rightCosts;
};

template <class BVOL>
inline MinimumSIC<BVOL>::MinimumSIC ()
  : MainAxesSorted<BVOL>()
{
}


/*! Partitioning adapter vector into two groups
     by evaluating the volume of the bounding volume. The adapter vector
    is then partitioned according to the minimum.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING MinimumVolume : public MinimumSIC<BVOL>
{
public:
   inline MinimumVolume ();

   virtual unsigned dimensions () { return 3; }

protected:
   virtual Real splitCosts (const std::vector<unsigned>& nodes,
			    const std::vector<Adapter*>& allNodes,
			    const VectorIndices&  range,
			    unsigned&             position);
};

template <class BVOL>
inline MinimumVolume<BVOL>::MinimumVolume ()
  : MinimumSIC<BVOL>()
{
}


/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the median.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMedian : public MainAxesSorted<BVOL>
{
public:
   inline LongestSideMedian ();

   virtual unsigned dimensions () { return 3; }

   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  unsigned&                             d, 
			  unsigned&                             i);
};

template <class BVOL>
inline LongestSideMedian<BVOL>::LongestSideMedian ()
{
}


/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the mean.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMean : public LongestSideMedian<BVOL>
{
public:
   inline LongestSideMean ();

   virtual unsigned dimensions () { return 3; }

   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  unsigned&                             d, 
			  unsigned&                             i);
};

template <class BVOL>
inline LongestSideMean<BVOL>::LongestSideMean ()
{
}

/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the median. This version
    of LongestSideMedian uses all k axes instead of only the main axes.
    For k-DOPs only!
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2 : public LongestSideMedian<BVOL>
{
public:
   inline LongestSideMedian2 ();

   virtual inline unsigned dimensions () { return BVOL::Size; }
   
   virtual void sortIndexList (std::vector<unsigned>& index,
				const std::vector<Adapter*>&  allNodes,
				unsigned dimension);
   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  unsigned&                             dimension, 
			  unsigned&                             split);
};

template <class BVOL>
inline LongestSideMedian2<BVOL>::LongestSideMedian2 ()
{
}


/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the mean. This version
    of LongestSideMean uses all k axes instead of only the main axes.
    For k-DOPs only!
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMean2 : public LongestSideMedian2<BVOL>
{
public:
   inline LongestSideMean2 ();

   virtual inline unsigned dimensions () { return BVOL::Size; }

   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  unsigned&                             dimension, 
			  unsigned&                             split);
};

template <class BVOL>
inline LongestSideMean2<BVOL>::LongestSideMean2 ()
{
}

/*! Partitioning adapter vector into two groups
    by iterating over the vector and choosing the one possibility (of two)
    where the bounding volume has smaller surface area approximation.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping : public Oracle
{
public:
   inline HeuristicGrouping ();

   virtual inline unsigned dimensions () { return BVOL::Size; }
   
   virtual void sortIndexList (std::vector<unsigned>& index,
				const std::vector<Adapter*>&  allNodes,
				unsigned dimension);
   virtual bool doSplitting (std::vector< std::vector<unsigned> >& index,
			     const std::vector<Adapter*>&          allNodes,
			     const VectorIndices&                  range,
			     const BoundingVolume<Real>&           bvol,
			     unsigned&                             dimension, 
			     unsigned&                             split);
};

template <class BVOL>
inline HeuristicGrouping<BVOL>::HeuristicGrouping ()
{
}

END_GENVIS_NAMESPACE
#endif






