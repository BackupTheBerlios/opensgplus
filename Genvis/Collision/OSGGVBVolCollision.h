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
//   $Date: 2004/03/12 13:18:34 $
//                                                                            
//=============================================================================

#ifndef OSGGVBVOLCOLLISION_H
#define OSGGVBVOLCOLLISION_H

#include <assert.h>
#include <vector>
#include "OSGGVBase.h"
#include "OSGGVReal.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVDoubleTraverser.h"
#include "OSGGVGroup.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVCollisionData.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief DataBase for pairwise collision detection, defining the interface for
           reporting collision results and collision statistics.
 */
template <class BasicTraits>
class BVolCollisionBase : public DataBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef DataBase<BasicTraits>               Inherited;
   typedef typename Inherited::Cache           Cache;
   typedef typename Inherited::CacheData       CacheData;
   typedef typename Inherited::DoubleTraverser DoubleTraverser;

   typedef typename BasicTraits::TransformType TransformType;
   typedef OpenSGTriangleBase<BasicTraits>     AdapterType;
   typedef CollisionData<AdapterType>          CollisionPair;
   typedef std::vector<CollisionPair>          CollisionContainer;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructor.                                                 */
   /*! \{                                                                 */
   inline BVolCollisionBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Init.                                                        */
   /*! \{                                                                 */   
   virtual inline bool                      Init (); 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Member.                                                      */
   /*! \{                                                                 */   
   virtual inline bool                      getStopFirst () const;
   virtual inline void                      setStopFirst (bool flag=false);
   inline void                              setLineGeometry (GeoPositionsPtr pos);
   inline GeoPositionsPtr                   getLineGeometry () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision statistics.                                        */
   /*! \{                                                                 */   
   virtual inline u32                  getNumPrimTests () const;
   virtual inline u32                  getNumMixedTests () const;
   virtual inline u32                  getNumBVolTests () const;
   inline u32&                         getPrimPrimTests ();
   inline u32&                         getBVolPrimTests ();
   inline u32&                         getPrimBVolTests ();
   inline u32&                         getBVolBVolTests ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Collision results.                                           */
   /*! \{                                                                 */   
   virtual inline u32                  getNumContacts () const;
   virtual inline CollisionContainer&       getContacts    ();
   virtual inline const CollisionContainer& getContacts    () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   bool               m_stopFirst;
   GeoPositionsPtr    m_pos;
   CollisionContainer m_contacts;
   u32           m_numPrimPrimTests;
   u32           m_numPrimBVolTests;
   u32           m_numBVolPrimTests;
   u32           m_numBVolBVolTests;
};

typedef  BVolCollisionBase<OpenSGTraits>  OSGBVolCollision;

template<class BasicTraits>
inline bool BVolCollisionBase<BasicTraits>::Init ()
{
   // clear statistics
   m_numPrimPrimTests = m_numBVolPrimTests = m_numPrimBVolTests = m_numBVolBVolTests = 0;
   // clear contact container
   m_contacts.clear();
   return true;
}
template <class BasicTraits>
inline bool BVolCollisionBase<BasicTraits>::getStopFirst () const
{
   return m_stopFirst;
}
template <class BasicTraits>
inline void BVolCollisionBase<BasicTraits>::setStopFirst (bool flag)
{
   m_stopFirst = flag;
}
template <class BasicTraits>
inline u32 BVolCollisionBase<BasicTraits>::getNumPrimTests () const
{
   return m_numPrimPrimTests;
}
template <class BasicTraits>
inline u32 BVolCollisionBase<BasicTraits>::getNumMixedTests () const
{
   return m_numPrimBVolTests+m_numBVolPrimTests;
}
template <class BasicTraits>
inline u32 BVolCollisionBase<BasicTraits>::getNumBVolTests () const
{
   return m_numBVolBVolTests;
}
template <class BasicTraits>
inline u32& BVolCollisionBase<BasicTraits>::getPrimPrimTests ()
{
   return m_numPrimPrimTests;
}
template <class BasicTraits>
inline u32& BVolCollisionBase<BasicTraits>::getBVolPrimTests ()
{
   return m_numBVolPrimTests;
}
template <class BasicTraits>
inline u32& BVolCollisionBase<BasicTraits>::getPrimBVolTests ()
{
   return m_numPrimBVolTests;
}
template <class BasicTraits>
inline u32& BVolCollisionBase<BasicTraits>::getBVolBVolTests ()
{
   return m_numBVolBVolTests;
}
template <class BasicTraits>
inline BVolCollisionBase<BasicTraits>::BVolCollisionBase ()
  : Inherited(), m_stopFirst(false)
{
}
template <class BasicTraits>
inline u32 BVolCollisionBase<BasicTraits>::getNumContacts () const
{
   return m_contacts.size();
}
template <class BasicTraits>
inline const typename BVolCollisionBase<BasicTraits>::CollisionContainer& 
BVolCollisionBase<BasicTraits>::getContacts () const
{
   return m_contacts;
}
template <class BasicTraits>
inline typename BVolCollisionBase<BasicTraits>::CollisionContainer& 
BVolCollisionBase<BasicTraits>::getContacts ()
{
   return m_contacts;
}

template <class BasicTraits>
inline void BVolCollisionBase<BasicTraits>::setLineGeometry (GeoPositionsPtr pos)
{
   m_pos = pos;
}
template <class BasicTraits>
inline GeoPositionsPtr BVolCollisionBase<BasicTraits>::getLineGeometry () const
{
   return m_pos;
}


template <class BasicTraits> class DoubleTraverserFixedBase;

/*! \brief Pairwise collision detection using only the major axis directions 0, 1, 2
    for realigning the k-DOPs in the second hierarchy.
 */
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING BVolCollision : public BVolCollisionBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolCollision<BasicTraits,BVOL>                       Self;
   typedef BVolCollisionBase<BasicTraits>                        Inherited;
   typedef typename Inherited::Cache                             Cache;
   typedef typename Inherited::CacheData                         CacheData;
   typedef typename Inherited::DoubleTraverser                   DoubleTraverser;
   typedef typename Inherited::TransformType                     TransformType;
   typedef typename Inherited::CollisionPair                     CollisionPair;
   typedef typename Inherited::CollisionContainer                CollisionContainer;

   typedef typename DoubleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGTriangle2BVol<BasicTraits,BVOL>                 AdapterType;
   typedef BVolGroup<BVOL>                                       GroupType;
   typedef BVolAdapter<BVOL>                                     GeneralType;
   typedef BVOL                                                  BVol;
   typedef Real (Self::*PrimIntersectTest) (AdapterType*, AdapterType*);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline BVolCollision ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */
   /*! Init for the single pair in the arguments.                         */
   bool        InitDouble  (GroupType* root0, const TransformType& m0, const TransformType& f0, 
			    GroupType* root1, const TransformType& m1, const TransformType& f1);
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

private:
   /*---------------------------------------------------------------------*/
   /*! \name Adapter depth.                                               */
   /*! \{                                                                 */
   u32 getCurrentDepth0 (const DoubleTraverserFixedBase<BasicTraits>* trav,
			 GroupType* adapter) const;
   u32 getCurrentDepth1 (const DoubleTraverserFixedBase<BasicTraits>* trav,
			 GroupType* adapter) const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Intersection tests.                                          */
   /*! \{                                                                 */
   /*! Intersection test of two bounding volumes. This includes realignment
       of model 1 in model space 0. */
   Real bvolIntersect (const BVOL& dop0, const BVOL& dop1);
   /*! Intersection tests of two primitives (here triangles). Returns collision
       time of primitives; -1 if no collision occurs. */
   Real primIntersectStatic (AdapterType*  p0, AdapterType*  p1);
   /*! Intersection tests of two primitives (here triangles). Returns collision
       time of primitives; -1 if no collision occurs. */
   Real primIntersectDynamic  (AdapterType*  p0, AdapterType*  p1);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   TransformType     m_M1ToM0;
   TransformType     m_M0ToM1;
   VectorClass       m_M0Direction[BVOL::Size];
   Real              m_proj00[BVOL::Size];
   Real              m_proj01[BVOL::Size];
   Real              m_proj02[BVOL::Size];
   Real              m_M0Offset[BVOL::Size];
   VectorClass       m_M1Direction[BVOL::Size];
   Real              m_proj10[BVOL::Size];
   Real              m_proj11[BVOL::Size];
   Real              m_proj12[BVOL::Size];
   Real              m_M1Offset[BVOL::Size];
   Real              m_proj0[2*BVOL::Size];
   Real              m_proj1[2*BVOL::Size];
   VectorClass       m_t0;
   VectorClass       m_t1;
   PrimIntersectTest f_primIntersect; 
};

template <class BasicTraits, class BVOL>
inline BVolCollision<BasicTraits,BVOL>::BVolCollision ()
  : BVolCollisionBase<BasicTraits>()
{
}
template <class BasicTraits, class BVOL>
inline bool BVolCollision<BasicTraits,BVOL>::LeaveDouble (
GroupType*, const TransformType&, const TransformType&,  
GroupType*, const TransformType&, const TransformType&)
{
   return false;
}
template <class BasicTraits, class BVOL>
inline void BVolCollision<BasicTraits,BVOL>::dump (std::ostream& os)
{
   os << "BVolCollision";
}


/*! \brief Traits class for usage of BVolCollision with double traverser.
 */
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING BVolCollisionTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BVolCollision<BasicTraits,BVOL>::CacheData     CacheData;
   typedef typename BVolCollision<BasicTraits,BVOL>::ResultT       ResultT;
   typedef BVolCollision<BasicTraits,BVOL>                         ObjectT;
   typedef typename BVolCollision<BasicTraits,BVOL>::GeneralType   GeneralType;
   typedef typename BVolCollision<BasicTraits,BVOL>::GroupType     GroupType;
   typedef typename BVolCollision<BasicTraits,BVOL>::AdapterType   AdapterType;
   typedef typename BVolCollision<BasicTraits,BVOL>::BVol          BVol;
   typedef typename BVolCollision<BasicTraits,BVOL>::TransformType TransformType;

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



