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
//   $Date: 2003/09/11 16:20:30 $
//                                                                            
//=============================================================================

#ifndef OSGGVDYNALIGNCOLLISION_H
#define OSGGVDYNALIGNCOLLISION_H

#include <assert.h>
#include <vector>
#include "OSGMatrix.h"
#include "OSGGVBase.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVKDop.h"
#include "OSGGVDoubleTraverser.h"
#include "OSGGVBVolCollision.h"
#include "OSGGVBVolFunctions.h"

BEGIN_GENVIS_NAMESPACE

/*! \brief Pairwise collision detection using all k-DOP directions for realigning the k-DOPs 
    in the second hierarchy.
 */
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollision 
: public BVolCollisionBase<BasicTraits>
{
public:
   typedef BVolCollisionBase<BasicTraits>                        Inherited;
   typedef typename DoubleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGTriangle2BVol<BasicTraits,BVOL>                 AdapterType;
   typedef BVolGroup<BVOL>                                       GroupType;
   typedef BVolAdapter<BVOL>                                     GeneralType;
   typedef BVOL                                                  BVol;
   typedef unsigned                                              SupportArray[3];

   /*---------------------------------------------------------------------*/
   /*! \name Constructors.                                                */
   /*! \{                                                                 */
   DynamicAlignCollision ();
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/
   /*! \name Traversal functions.                                         */
   /*! \{                                                                 */
   /*! Init for the single pair in the arguments.                         */
   bool    InitDouble        (GroupType* root0, const TransformType& m0, 
			      GroupType* root1, const TransformType& m1);
   /*! Leave for the single pair in the arguments.                        */
   inline bool LeaveDouble (GroupType* root0, const TransformType& m0, 
			    GroupType* root1, const TransformType& m1);

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
   /*! \name Intersection test (with realignment).                        */
   /*! \{                                                                 */
   bool               testIntersect (const BVOL& dop1, const BVOL& dop2);
   /*! \}                                                                 */
   /*---------------------------------------------------------------------*/

   VectorClass        m_M1Direction[2*BVOL::Size];
   Real               m_M1Translate[2*BVOL::Size];
   TransformType      m_M1ToM0;
   TransformType      m_M0ToM1;
   SupportArray       m_support[2*BVOL::Size];
   VectorClass        m_direction[2*BVOL::Size];
   bool               m_realign;
};

template <class BasicTraits, class BVOL>
inline bool DynamicAlignCollision<BasicTraits,BVOL>::LeaveDouble (
GroupType*, const TransformType&, 
GroupType*, const TransformType&)
{
   return false;
}
template <class BasicTraits, class BVOL>
inline void DynamicAlignCollision<BasicTraits,BVOL>::dump (std::ostream& os)
{
   os << "DynamicAlignCollision";
}


/*! \brief Traits class for usage of DynamicAlignCollision with double traverser.
 */
template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING DynamicAlignCollisionTraits
{
public:
   /*---------------------------------------------------------------------*/
   /*! \name Types.                                                       */
   /*! \{                                                                 */
   typedef typename DynamicAlignCollision<BasicTraits,BVOL>::CacheData     CacheData;
   typedef typename DynamicAlignCollision<BasicTraits,BVOL>::ResultT       ResultT;
   typedef DynamicAlignCollision<BasicTraits,BVOL>                         ObjectT;
   typedef typename DynamicAlignCollision<BasicTraits,BVOL>::GeneralType   GeneralType;
   typedef typename BVolCollision<BasicTraits,BVOL>::GroupType             GroupType;
   typedef typename DynamicAlignCollision<BasicTraits,BVOL>::AdapterType   AdapterType;
   typedef typename DynamicAlignCollision<BasicTraits,BVOL>::BVol          BVol;
   typedef typename DynamicAlignCollision<BasicTraits,BVOL>::TransformType TransformType;

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



