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
//   $Date: 2003/09/19 21:43:27 $
//                                                                            
//=============================================================================

#ifndef OSGGVMINIMUMDISTANCECALC_H
#define OSGGVMINIMUMDISTANCECALC_H

#include <assert.h>
#include <vector>
#include "OSGMatrix.h"
#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVTriangleAdapterAligned.h"
#include "OSGGVKDop.h"
#include "OSGGVDoubleTraverser.h"
#include "OSGGVBVolDistance.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Minimum distance calculation using a branch-and-bound algorithm
    on the k-DOP hierarchy.
 */
template <class BasicTraits, class BVOL, class Metric=EuclideanMetric>
class OSG_GENVISLIB_DLLMAPPING MinimumDistanceCalc 
: public BVolDistanceBase<BasicTraits,Metric>
{
public:
   typedef BVolDistanceBase<BasicTraits,Metric>                  Inherited;
   typedef BVOL                                                  BVol;
   typedef typename DoubleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGTriangleAligned<BasicTraits,BVOL>               AdapterType;
   typedef BVolGroupAligned<BVOL>                                GroupType;
   typedef BVolAdapter<BVOL>                                     GeneralType;
#ifdef GV_PRECOMPUTEDREALIGN_32BIT
   enum { OccTableHighestBit = 31, OccTableBits = 32 };
   typedef u32                                                   OccTableType;
#else
   enum { OccTableHighestBit = 63, OccTableBits = 64 };
   typedef u64                                                   OccTableType;
#endif
   typedef DataAligned<Real,BVOL::Size>                          Data;

   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   MinimumDistanceCalc ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision functions.                                         */
   /*! \{                                                                 */
   virtual bool   Init ();
   /*! Init for the single pair in the arguments.                         */
   bool        InitDouble        (GroupType* root0, const TransformType& d0, 
				  GroupType* root1, const TransformType& d1);
   /*! Leave for the single pair in the arguments.                        */
   inline bool LeaveDouble (GroupType* root0, const TransformType& m0, 
			    GroupType* root1, const TransformType& m1);

   /*! Operation for inner nodes in the arguments.                        */
   ResultT BVolBVolCollision (GroupType* b0, GroupType* b1);
   /*! Operation for an inner node in the first hierarchy and a leaf node 
       in the second hierarchy.                                           */
   ResultT PrimBVolCollision (AdapterType* b0, GroupType* b1);
   /*! Operation for an inner node in the first hierarchy and a leaf node 
       in the second hierarchy.                                           */
   ResultT BVolPrimCollision (GroupType* b0, AdapterType* b1);
   /*! Operation for leaf nodes in the arguments.                         */
   ResultT PrimPrimCollision (AdapterType* b0, AdapterType* b1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Dump.                                                        */
   /*! \{                                                                 */
   virtual inline void dump (std::ostream& os);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   TransformType      m_M0;
   VectorClass        m_M1Direction[2*BVOL::Size];
   VectorClass        m_proj[BVOL::Size];
   Real               m_M1Offset[BVOL::Size];
   TransformType      m_M1ToM0;
   TransformType      m_M0ToM1;
   unsigned           m_perm[2*BVOL::Size];
   unsigned           m_mask[2*BVOL::Size];
   unsigned           m_submask[2*BVOL::Size];
};

template <class BasicTraits, class BVOL, class Metric>
inline bool MinimumDistanceCalc<BasicTraits,BVOL,Metric>::LeaveDouble (
GroupType*, const TransformType&, 
GroupType*, const TransformType&)
{
   return false;
}
template <class BasicTraits, class BVOL, class Metric>
inline void MinimumDistanceCalc<BasicTraits,BVOL,Metric>::dump (std::ostream& os)
{
   os << "MinimumDistanceCalc";
}


template <class BasicTraits, class BVOL, class Metric=EuclideanMetric>
class OSG_GENVISLIB_DLLMAPPING MinimumDistanceCalcTraits
{
public:
   typedef typename MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>::CacheData   CacheData;
   typedef typename MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>::ResultT     ResultT;
   typedef MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>                       ObjectT;
   typedef typename MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>::GeneralType GeneralType;
   typedef typename MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>::GroupType   GroupType;
   typedef typename MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>::AdapterType AdapterType;
   typedef typename MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>::BVol        BVol;
   typedef typename MinimumDistanceCalc<BasicTraits,BVOL,EuclideanMetric>::TransformType TransformType;

   typedef InitSingleFunctor<bool,ObjectT>                                   InitFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT> InitDoubleFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT> LeaveDoubleFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,GroupType,ObjectT> BVolBVolFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,GroupType,ObjectT> PrimBVolFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,AdapterType,ObjectT> BVolPrimFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,AdapterType,ObjectT> PrimPrimFunctorT;

   static  InitFunctorT        createInitFunctor       (ObjectT* obj);
   static  InitDoubleFunctorT  createInitDoubleFunctor (ObjectT* obj);
   static  LeaveDoubleFunctorT createLeaveDoubleFunctor (ObjectT* obj);
   static  BVolBVolFunctorT createBVolBVolFunctor (ObjectT* obj);
   static  PrimBVolFunctorT createPrimBVolFunctor (ObjectT* obj);
   static  BVolPrimFunctorT createBVolPrimFunctor (ObjectT* obj);
   static  PrimPrimFunctorT createPrimPrimFunctor (ObjectT* obj);
};

END_GENVIS_NAMESPACE

#endif



