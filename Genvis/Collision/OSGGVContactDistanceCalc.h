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
//   $Date: 2004/03/12 13:18:34 $
//                                                                            
//=============================================================================

#ifndef OSGGVCONTACTDISTANCECALC_H
#define OSGGVCONTACTDISTANCECALC_H

#include <assert.h>
#include <vector>
#include "OSGMatrix.h"
#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVDoubleTraverser.h"
#include "OSGGVTriangleAdapterAligned.h"
#include "OSGGVBVolDistance.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Contact detection using a branch-and-bound algorithm on the k-DOP hierarchy.
 */
template <class BasicTraits, class BVOL, class Metric=EuclideanMetric>
class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalc 
: public BVolDistanceBase<BasicTraits,Metric>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolDistanceBase<BasicTraits,Metric>                  Inherited;
   typedef typename Inherited::Cache                             Cache;
   typedef typename Inherited::CacheData                         CacheData;
   typedef typename Inherited::DoubleTraverser                   DoubleTraverser;
   typedef typename Inherited::TransformType                     TransformType;
   typedef typename Inherited::CollisionPair                     CollisionPair;
   typedef typename Inherited::CollisionContainer                CollisionContainer;
   typedef typename Inherited::MetricType                        MetricType; 

   typedef typename DoubleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGTriangleAligned<BasicTraits,BVOL>               AdapterType;
   typedef BVolGroupAligned<BVOL>                                GroupType;
   typedef BVolAdapter<BVOL>                                     GeneralType;
   typedef BVOL                                                  BVol;
#ifdef GV_PRECOMPUTEDREALIGN_32BIT
   enum { OccTableHighestBit = 31, OccTableBits = 32 };
   typedef u32                                                   OccTableType;
#else
   enum { OccTableHighestBit = 63, OccTableBits = 64 };
   typedef u64                                                   OccTableType;
#endif
   typedef DataAligned<Real,BVOL::Size>                          Data;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   ContactDistanceCalc ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline Real getTolerance () const;
   inline void setTolerance (Real tolerance);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */
   virtual bool        Init ();
   /*! Init for the single pair in the arguments.                         */
   bool    InitDouble        (GroupType* root0, const TransformType& d0, const TransformType& f0, 
			      GroupType* root1, const TransformType& d1, const TransformType& f1);
   /*! Leave for the single pair in the arguments.                        */
   inline bool LeaveDouble (GroupType* root0, const TransformType& m0, const TransformType& f0, 
			    GroupType* root1, const TransformType& m1, const TransformType& f1);

   /*! Operation for inner nodes in the arguments.                        */
   ResultT BVolBVolCollision (GroupType* b0, GroupType* b1);
   /*! Operation for a leaf node in the first and an inner node 
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
   Real               m_tolerance;
   TransformType      m_M0;
   TransformType      m_M1ToM0;
   TransformType      m_M0ToM1;

   Real               m_scale;

   VectorClass        m_proj[BVOL::Size];
   Real               m_M1Offset[BVOL::Size];

   unsigned           m_perm[2*BVOL::Size];
   unsigned           m_mask[2*BVOL::Size];
   unsigned           m_submask[2*BVOL::Size];
};

template <class BasicTraits, class BVOL, class Metric>
inline void ContactDistanceCalc<BasicTraits,BVOL,Metric>::dump (std::ostream& os)
{
   os << "ContactDistanceCalc";
}

template <class BasicTraits, class BVOL, class Metric>
inline bool ContactDistanceCalc<BasicTraits,BVOL,Metric>::LeaveDouble 
(GroupType*, const TransformType&, const TransformType&, 
 GroupType*, const TransformType&, const TransformType&)
{
   return false;
}

template <class BasicTraits, class BVOL, class Metric>
inline Real ContactDistanceCalc<BasicTraits,BVOL,Metric>::getTolerance () const
{
   return m_tolerance;
}
template <class BasicTraits, class BVOL, class Metric>
inline void ContactDistanceCalc<BasicTraits,BVOL,Metric>::setTolerance (Real tolerance)
{
   m_tolerance = tolerance;
}

/*! \brief Traits class for usage of ContactDistanceCalc with double traverser.
 */
template <class BasicTraits, class BVOL, class Metric=EuclideanMetric>
class OSG_GENVISLIB_DLLMAPPING ContactDistanceCalcTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::CacheData   CacheData;
   typedef typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::ResultT     ResultT;
   typedef ContactDistanceCalc<BasicTraits,BVOL,Metric>                       ObjectT;
   typedef typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::GeneralType GeneralType;
   typedef typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::GroupType           GroupType;
   typedef typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::AdapterType AdapterType;
   typedef typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::BVol        BVol;
   typedef typename ContactDistanceCalc<BasicTraits,BVOL,Metric>::TransformType TransformType;

   typedef InitSingleFunctor<bool,ObjectT>                                    InitFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT>  InitDoubleFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT> LeaveDoubleFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,GroupType,ObjectT> BVolBVolFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,GroupType,ObjectT> PrimBVolFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,AdapterType,ObjectT> BVolPrimFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,AdapterType,ObjectT> PrimPrimFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functor creation.                                            */
   /*! \{                                                                 */
   static  InitFunctorT           createInitFunctor       (ObjectT* obj);
   static  InitDoubleFunctorT     createInitDoubleFunctor (ObjectT* obj);
   static  LeaveDoubleFunctorT    createLeaveDoubleFunctor (ObjectT* obj);
   static  BVolBVolFunctorT createBVolBVolFunctor (ObjectT* obj);
   static  PrimBVolFunctorT createPrimBVolFunctor (ObjectT* obj);
   static  BVolPrimFunctorT createBVolPrimFunctor (ObjectT* obj);
   static  PrimPrimFunctorT createPrimPrimFunctor (ObjectT* obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

END_GENVIS_NAMESPACE

#endif



