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

#ifndef OSGGVORACLEINSTANCE_H
#define OSGGVORACLEINSTANCE_H

#include "OSGGVBase.h"
#include "OSGGVOracle.h"

BEGIN_GENVIS_NAMESPACE

/*! Partitioning adapter vector into two groups
    by evaluating a surface-intersection-cost metric 
    (evaluates surface area of bounding volume). The adapter vector
    is then partitioned according to the minimum.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING MinimumSIC : public MainAxesSorted<BVOL>
{
public:
   MinimumSIC ();

   virtual const char* getDescription () const {
      return "MinimumSurfaceIntersectionCost";
   }

   virtual bool doSplitting (std::vector< std::vector<u32> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&           bvol,
			  u32&                             d, 
			  u32&                             i);

protected:
   virtual Real splitCosts (const std::vector<u32>& nodes,
			    const std::vector<Adapter*>& allNodes,
			    const VectorIndices&  range,
			    u32&             position);

   std::vector<Real> m_leftCosts;
   std::vector<Real> m_rightCosts;
};


/*! Partitioning adapter vector into two groups
     by evaluating the volume of the bounding volume. The adapter vector
    is then partitioned according to the minimum.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING MinimumVolume : public MinimumSIC<BVOL>
{
public:
   MinimumVolume ();

   virtual inline const char* getDescription () const {
      return "MinimumWeightedVolume";
   }

protected:
   virtual Real splitCosts (const std::vector<u32>& nodes,
			    const std::vector<Adapter*>& allNodes,
			    const VectorIndices&  range,
			    u32&             position);
};


/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the median.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMedian : public MainAxesSorted<BVOL>
{
public:
   LongestSideMedian ();

   virtual inline const char* getDescription () const {
      return "LongestSideMedian";
   }

   virtual bool doSplitting (std::vector< std::vector<u32> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  u32&                             d, 
			  u32&                             i);
};



/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the mean.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMean : public LongestSideMedian<BVOL>
{
public:
   LongestSideMean ();

   virtual inline const char* getDescription () const {
      return "LongestSideMean";
   }

   virtual bool doSplitting (std::vector< std::vector<u32> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  u32&                             d, 
			  u32&                             i);
};


/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the median. This version
    of LongestSideMedian uses all k axes instead of only the main axes.
    For k-DOPs only!
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMedian2 : public AllAxesSorted<BVOL>
{
public:
   LongestSideMedian2 ();

   virtual inline const char* getDescription () const {
      return "LongestSideMedian_allAxes";
   }

   virtual bool doSplitting (std::vector< std::vector<u32> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  u32&                             dimension, 
			  u32&                             split);
};



/*! Partitioning adapter vector into two groups
    by subdividing the list of longest extent at the mean. This version
    of LongestSideMean uses all k axes instead of only the main axes.
    For k-DOPs only!
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING LongestSideMean2 : public LongestSideMedian2<BVOL>
{
public:
   LongestSideMean2 ();

   virtual inline const char* getDescription () const {
      return "LongestSideMean_allAxes";
   }

   virtual bool doSplitting (std::vector< std::vector<u32> >& index,
			  const std::vector<Adapter*>&          allNodes,
			  const VectorIndices&                  range,
			  const BoundingVolume<Real>&                 bvol,
			  u32&                             dimension, 
			  u32&                             split);
};


/*! Partitioning adapter vector into two groups
    by iterating over the vector and choosing the one possibility (of two)
    where the bounding volume has smaller surface area approximation.
*/
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING HeuristicGrouping : public AllAxesSorted<BVOL>
{
public:
   HeuristicGrouping ();

   virtual inline const char* getDescription () const {
      return "HeuristicGrouping";
   }
   
   virtual bool doSplitting (std::vector< std::vector<u32> >& index,
			     const std::vector<Adapter*>&          allNodes,
			     const VectorIndices&                  range,
			     const BoundingVolume<Real>&           bvol,
			     u32&                             dimension, 
			     u32&                             split);
};

END_GENVIS_NAMESPACE
#endif






