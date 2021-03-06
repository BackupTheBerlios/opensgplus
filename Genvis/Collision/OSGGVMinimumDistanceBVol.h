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

#ifndef OSGGVMINIMUMDISTANCEBVOL_H
#define OSGGVMINIMUMDISTANCEBVOL_H

#include <assert.h>
#include <vector>
#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVDoubleTraverser.h"
#include "OSGGVTriangleAdapterAligned.h"
#include "OSGGVBVolDistance.h"
#include "OSGGVDoubleTraverserFixed.h"

BEGIN_GENVIS_NAMESPACE

// get node depth in hierarchy by counting
#undef   GV_COUNT_DEPTH

template <class BasicTraits> class DoubleTraverserFixedBase;

/*! \brief Minimum distance computation using the k-DOPs at a fixed level 
    as primitives.
 */
template <class BasicTraits, class BVOL, 
          class Metric = EuclideanMetric>
class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVol 
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
   MinimumDistanceBVol ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */
   inline void setDistance (Real dist);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */
   virtual bool        Init ();
   /*! Init for the single pair in the arguments.                         */
   bool    InitDouble        (GroupType* root0, const TransformType& d0, const TransformType& f0, 
			      GroupType* root1, const TransformType& d1, const TransformType& f1);
   /*! Leave for the single pair in the arguments.                        */
   bool    LeaveDouble       (GroupType* root0, const TransformType& d0, const TransformType& f0, 
			      GroupType* root1, const TransformType& d1, const TransformType& f1);
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
   /*---------------------------------------------------------------------*/
   /*! \name Distance functions.                                          */
   /*! \{                                                                 */
   Real     bvolDistance (const BVOL& dop1,
			  const BVOL& dop2,
			  const Data& data1,
			  const Data& data2,
			  PointClass* store1 = NULL,
			  PointClass* store2 = NULL);
   void     edgePolygonDistance (const PointClass* p1,
				 u32 jmax, const PointClass* p2,
				 const VectorClass& dirZ,
				 Real& result,
				 PointClass* store1,
				 PointClass* store2);
   void     edgeEdgeDistance(const PointClass& s1, 
			     const PointClass& e1, 
			     const PointClass& s2, 
			     const PointClass& e2,
			     Real& result,
			     PointClass* store1,
			     PointClass* store2);
   void     allEdgesDistance (u32 imax, const PointClass* p1,
			      u32 jmax, const PointClass* p2,
			      Real& result,
			      PointClass* store1,
			      PointClass* store2);
   bool     intersectProjection (u32 imax, const PointClass* p1, 
				 u32 jmax, const PointClass* p2,
				 const VectorClass& dirX, 
				 const VectorClass& dirY,
				 std::vector<u32>& enter, 
				 std::vector<u32>& leave);
   bool     intersectProjection (u32 imax, const PointClass* p1,
				 u32 jmax, const PointClass* p2,
				 const VectorClass& dirX,
				 const VectorClass& dirY,
				 const VectorClass& dirZ,
				 Real& result,
				 PointClass* store1,
				 PointClass* store2);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Helper.                                                      */
   /*! \{                                                                 */
   static bool calcIntersectPoint (PointClass& p,
				   Real di, 
				   Real dj, 
				   Real dk,
				   const VectorClass& ni,
				   const VectorClass& nj,
				   const VectorClass& nk);
   static inline void updatePoints (const PointClass& p1, const PointClass& p2,
				    PointClass* store1,   PointClass* store2);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Adapter depth.                                               */
   /*! \{                                                                 */
   inline u32 getCurrentDepth0 (const DoubleTraverserFixedBase<BasicTraits>* trav,
				GroupType* adapter) const;
   inline u32 getCurrentDepth1 (const DoubleTraverserFixedBase<BasicTraits>* trav,
				GroupType* adapter) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   GroupType*              m_root0;
   std::vector<GroupType*> m_root1;
   bool                    m_thisIntersect;

   TransformType      m_M0;
   TransformType      m_M1ToM0;
   TransformType      m_M0ToM1;

   Real               m_scale;
   VectorClass        m_M1Direction[2*BVOL::Size];
   Real               m_metric[BVOL::Size];

   VectorClass        m_proj[BVOL::Size];
   Real               m_M1Offset[BVOL::Size];

   u32                m_perm[2*BVOL::Size];
   u32                m_mask[2*BVOL::Size];
   u32                m_submask[2*BVOL::Size];
};

template <class BasicTraits, class BVOL, class Metric>
inline void MinimumDistanceBVol<BasicTraits,BVOL,Metric>::dump (std::ostream& os)
{
   os << "MinimumDistanceBVol";
}
template <class BasicTraits, class BVOL, class Metric>
inline void MinimumDistanceBVol<BasicTraits,BVOL,Metric>::setDistance (Real dist)
{
   Inherited::setDistance(dist);
   CollisionInterface<OpenSGTriangleBase<BasicTraits>,Distance> collisionData(m_contacts[0]);
   collisionData.getData().distance = (isIntersecting() ? -dist : dist);
}
template<class BasicTraits, class BVOL, class Metric>
inline void MinimumDistanceBVol<BasicTraits,BVOL,Metric>::updatePoints (const PointClass& p1,
									const PointClass& p2,
									PointClass* store1,
									PointClass* store2)
{
   if (store1 != NULL) { store1->setValue(p1); }
   if (store2 != NULL) { store2->setValue(p2); } 
}
template<class BasicTraits, class BVOL, class Metric>
inline u32 MinimumDistanceBVol<BasicTraits,BVOL,Metric>::getCurrentDepth0 (const DoubleTraverserFixedBase<BasicTraits>* trav,
									   GroupType* adapter) const
{
#ifdef GV_COUNT_DEPTH
   return adapter->getDepth();
#else
   assert(trav != NULL);
   return trav->getCurrentDepth0();
#endif
}
template<class BasicTraits, class BVOL, class Metric>
inline u32 MinimumDistanceBVol<BasicTraits,BVOL,Metric>::getCurrentDepth1 (const DoubleTraverserFixedBase<BasicTraits>* trav,
									   GroupType* adapter) const
{
#ifdef GV_COUNT_DEPTH
   return adapter->getDepth();
#else
   assert(trav != NULL);
   return trav->getCurrentDepth1();
#endif
}


/*! \brief Traits class for usage of MinimumDistanceBVol with double traverser.
 */
template <class BasicTraits, class BVOL, class Metric=EuclideanMetric>
class OSG_GENVISLIB_DLLMAPPING MinimumDistanceBVolTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename MinimumDistanceBVol<BasicTraits,BVOL,Metric>::CacheData   CacheData;
   typedef typename MinimumDistanceBVol<BasicTraits,BVOL,Metric>::ResultT     ResultT;
   typedef MinimumDistanceBVol<BasicTraits,BVOL,Metric>                       ObjectT;
   typedef typename MinimumDistanceBVol<BasicTraits,BVOL,Metric>::GeneralType GeneralType;
   typedef typename MinimumDistanceBVol<BasicTraits,BVOL,Metric>::GroupType   GroupType;
   typedef typename MinimumDistanceBVol<BasicTraits,BVOL,Metric>::AdapterType AdapterType;
   typedef typename MinimumDistanceBVol<BasicTraits,BVOL,Metric>::BVol        BVol;
   typedef typename MinimumDistanceBVol<BasicTraits,BVOL,Metric>::TransformType TransformType;

   typedef InitSingleFunctor<bool,ObjectT>                                   InitFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT> InitDoubleFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,TransformType,GroupType,TransformType,ObjectT> LeaveDoubleFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,GroupType,ObjectT> BVolBVolFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,GroupType,ObjectT> PrimBVolFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,AdapterType,ObjectT> BVolPrimFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,AdapterType,ObjectT> PrimPrimFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functor creation.                                            */
   /*! \{                                                                 */
   static  InitFunctorT        createInitFunctor        (ObjectT* obj);
   static  InitDoubleFunctorT  createInitDoubleFunctor  (ObjectT* obj);
   static  LeaveDoubleFunctorT createLeaveDoubleFunctor (ObjectT* obj);
   static  BVolBVolFunctorT createBVolBVolFunctor (ObjectT* obj);
   static  PrimBVolFunctorT createPrimBVolFunctor (ObjectT* obj);
   static  BVolPrimFunctorT createBVolPrimFunctor (ObjectT* obj);
   static  PrimPrimFunctorT createPrimPrimFunctor (ObjectT* obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

END_GENVIS_NAMESPACE

#endif



