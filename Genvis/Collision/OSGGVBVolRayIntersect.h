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

#ifndef OSGGVBVOLRAYINTERSECT_H
#define OSGGVBVOLRAYINTERSECT_H

#include "OSGVector.h"
#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVSingleTraverser.h"
#include "OSGGVGroup.h"
#include "OSGGVRay.h"
#include "OSGGVIntersection.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base for ray intersection with bounding volume hierarchy.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING BVolRayIntersectBase
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BasicTraits::GeomObjectType      GeomObjectType;
   typedef typename BasicTraits::GeomFaceType        GeomFaceType;
   typedef typename BasicTraits::TransformType       TransformType;
   typedef BVolAdapterBase                           GeneralType; 
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline BVolRayIntersectBase ();
   virtual inline ~BVolRayIntersectBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */   
   virtual inline GeneralType* getHit () const;
   inline Real                 getHitDist () const;
   inline PointClass           getHitPoint () const;
   inline void                 setIntersect (Intersection& in);
   inline Intersection&        getIntersect () const;
   inline const Ray&           getRay () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   Intersection*    m_intersect;
};

template <class BasicTraits>
inline BVolRayIntersectBase<BasicTraits>::BVolRayIntersectBase ()
  : m_intersect(NULL)
{
}
template <class BasicTraits>
inline BVolRayIntersectBase<BasicTraits>::~BVolRayIntersectBase ()
{
}

template <class BasicTraits>
inline typename BVolRayIntersectBase<BasicTraits>::GeneralType*    
BVolRayIntersectBase<BasicTraits>::getHit () const
{
   assert(m_intersect != NULL);
   return m_intersect->getTo();
}
template <class BasicTraits>
inline Real BVolRayIntersectBase<BasicTraits>::getHitDist () const
{
   assert(m_intersect != NULL);
   return m_intersect->getDist();
}

template <class BasicTraits>
inline void     BVolRayIntersectBase<BasicTraits>::setIntersect (Intersection& in)
{
   m_intersect = &in;
}
template <class BasicTraits>
inline Intersection& BVolRayIntersectBase<BasicTraits>::getIntersect () const
{
   assert(m_intersect != NULL);
   return *m_intersect;
}
template <class BasicTraits>
inline const Ray& BVolRayIntersectBase<BasicTraits>::getRay () const
{
   assert(m_intersect != NULL);
   return m_intersect->getRay();
}

template <class BasicTraits>
inline PointClass BVolRayIntersectBase<BasicTraits>::getHitPoint () const
{
   return getRay().getOrigin() + getHitDist()*getRay().getDirection();
}


/*! \brief Ray intersection with bounding volume hierarchy.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING BVolRayIntersect 
: public BVolRayIntersectBase<BasicTraits>
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef BVolRayIntersectBase<BasicTraits>                     Inherited;
   typedef typename Inherited::GeomObjectType                    GeomObjectType;
   typedef typename Inherited::GeomFaceType                      GeomFaceType;
   typedef typename Inherited::TransformType                     TransformType;
   typedef typename Inherited::GeneralType                       GeneralType; 

   typedef BoundingVolume<Real>                                  BVol;
   typedef typename SingleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef BVolAdapterBase                                       AdapterType;
   typedef BVolGroupInterface                                    GroupType;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline BVolRayIntersect ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */   
   inline AdapterType* getHitTyped () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */
   bool     InitRayIntersect   (GeneralType* a, const TransformType& m);
   bool     LeaveRayIntersect  (GeneralType* a, const TransformType& m);
   ResultT  InnerRayIntersect (GroupType*   b0);
   inline ResultT  InnerLeaveRayIntersect (GroupType*   b0);
   ResultT  LeafRayIntersect  (AdapterType* p0);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   Intersection m_local;
};

template <class BasicTraits>
inline BVolRayIntersect<BasicTraits>::BVolRayIntersect ()
  : BVolRayIntersectBase<BasicTraits>()
{
}
template <class BasicTraits>
inline typename BVolRayIntersect<BasicTraits>::AdapterType* 
BVolRayIntersect<BasicTraits>::getHitTyped () const
{
   return static_cast<AdapterType*>(getHit());
}
template <class BasicTraits>
inline typename BVolRayIntersect<BasicTraits>::ResultT  
BVolRayIntersect<BasicTraits>::InnerLeaveRayIntersect (GroupType*)
{
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}


/*! \brief Traits class for BVolRayIntersect.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING BVolRayIntersectTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename BVolRayIntersect<BasicTraits>::BVol          BVol;
   typedef typename BVolRayIntersect<BasicTraits>::ResultT       ResultT;
   typedef BVolRayIntersect<BasicTraits>                         ObjectT;
   typedef typename BVolRayIntersect<BasicTraits>::GeneralType   GeneralType;
   typedef typename BVolRayIntersect<BasicTraits>::GroupType     GroupType;
   typedef typename BVolRayIntersect<BasicTraits>::AdapterType   AdapterType;
   typedef typename BVolRayIntersect<BasicTraits>::TransformType TransformType;

   typedef InitSingleFunctor2<bool,GeneralType,TransformType,ObjectT> InitFunctorT;
   typedef SingleFunctor<ResultT,GroupType,ObjectT>                   InnerFunctorT;
   typedef SingleFunctor<ResultT,AdapterType,ObjectT>                 LeafFunctorT;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Functor creation.                                            */
   /*! \{                                                                 */
   static  InitFunctorT  createInitFunctor      (ObjectT* obj);
   static  InitFunctorT  createInitLeaveFunctor (ObjectT* obj);
   static  InnerFunctorT createInnerFunctor     (ObjectT* obj);
   static  InnerFunctorT createInnerLeaveFunctor(ObjectT* obj);
   static  LeafFunctorT  createLeafFunctor      (ObjectT* obj);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
};

END_GENVIS_NAMESPACE
#endif


