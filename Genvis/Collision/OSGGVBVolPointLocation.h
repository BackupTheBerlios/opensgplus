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

#ifndef OSGGVBVOLPOINTLOCATION_H
#define OSGGVBVOLPOINTLOCATION_H

#include "OSGGVBase.h"
#include "OSGVector.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVInput.h"
#include "OSGGVSingleTraverser.h"
#include "OSGGVGroup.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Base for point location on bounding volume hierarchies.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING BVolPointLocationBase
{
public:
   typedef typename BasicTraits::GeomObjectType      GeomObjectType;
   typedef typename BasicTraits::GeomFaceType        GeomFaceType;
   typedef typename BasicTraits::TransformType       TransformType;
   typedef BVolAdapterBase                           GeneralType; 

   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline BVolPointLocationBase ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */   
   virtual inline GeneralType* getHit () const;
   inline void       setPoint (const PointClass& p);
   inline PointClass getPoint ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   GeneralType* m_hit;
   PointClass   m_point;
};

template <class BasicTraits>
inline BVolPointLocationBase<BasicTraits>::BVolPointLocationBase ()
  : m_hit(NULL), m_point()
{
}

template <class BasicTraits>
inline BVolPointLocationBase<BasicTraits>::GeneralType*    
BVolPointLocationBase<BasicTraits>::getHit       () const
{
   return m_hit;
}

template <class BasicTraits>
inline void       BVolPointLocationBase<BasicTraits>::setPoint (const PointClass&   p)
{
   m_point = p;
}
template <class BasicTraits>
inline PointClass BVolPointLocationBase<BasicTraits>::getPoint ()
{
   return m_point;
}

/*! \brief Point location on bounding volume hierarchies. Reports the last leaf node, which
    contains the query point.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING BVolPointLocation 
: public BVolPointLocationBase<BasicTraits>
{
public:
   typedef BoundingVolume<Real>                         BVol;
   typedef SingleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef BVolAdapterBase                              AdapterType;
   typedef BVolGroupInterface                           GroupType;

   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   inline BVolPointLocation ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Members.                                                     */
   /*! \{                                                                 */   
   inline AdapterType* getHitTyped () const;
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */   
   bool     InitPointLocation  (GeneralType* a, const TransformType& m);
   bool     LeavePointLocation (GeneralType* a, const TransformType& m);
   ResultT  InnerPointLocation (GroupType*   b0);
   inline ResultT  InnerLeavePointLocation (GroupType*   b0);
   ResultT  LeafPointLocation  (AdapterType* p0);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

protected:
   PointClass m_local;
};

template <class BasicTraits>
inline BVolPointLocation<BasicTraits>::BVolPointLocation ()
  : BVolPointLocationBase<BasicTraits>()
{
}
template <class BasicTraits>
inline BVolPointLocation<BasicTraits>::AdapterType* 
BVolPointLocation<BasicTraits>::getHitTyped () const
{
   return static_cast<AdapterType*>(m_hit);
}
template <class BasicTraits>
inline BVolPointLocation<BasicTraits>::ResultT  
BVolPointLocation<BasicTraits>::InnerLeavePointLocation 
(GroupType*)
{
   return SingleTraverserBase<BasicTraits>::CONTINUE;
}


/*! \brief Traits class for point location.
*/
template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING BVolPointLocationTraits
{
public:
   typedef typename BVolPointLocation<BasicTraits>::BVol          BVol;
   typedef typename BVolPointLocation<BasicTraits>::ResultT       ResultT;
   typedef BVolPointLocation<BasicTraits>                         ObjectT;
   typedef typename BVolPointLocation<BasicTraits>::GeneralType   GeneralType;
   typedef typename BVolPointLocation<BasicTraits>::GroupType     GroupType;
   typedef typename BVolPointLocation<BasicTraits>::AdapterType   AdapterType;
   typedef typename BVolPointLocation<BasicTraits>::TransformType TransformType;

   typedef InitSingleFunctor2<bool,GeneralType,TransformType,ObjectT> InitFunctorT;
   typedef SingleFunctor<ResultT,GroupType,ObjectT>                   InnerFunctorT;
   typedef SingleFunctor<ResultT,AdapterType,ObjectT>                 LeafFunctorT;

   static  InitFunctorT  createInitFunctor      (ObjectT* obj);
   static  InitFunctorT  createInitLeaveFunctor (ObjectT* obj);
   static  InnerFunctorT createInnerFunctor     (ObjectT* obj);
   static  InnerFunctorT createInnerLeaveFunctor(ObjectT* obj);
   static  LeafFunctorT  createLeafFunctor      (ObjectT* obj);
};

END_GENVIS_NAMESPACE
#endif
