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

#ifndef OSGGVEMPTYCOLLISION_H
#define OSGGVEMPTYCOLLISION_H

#include <assert.h>
#include <vector>
#include "OSGGVBase.h"
//#include "OSGGVVector3.h"
#include "OSGGVReal.h"
#include "OSGMatrix.h"
#include "OSGGVFunctors.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVDoubleTraverser.h"
#include "OSGGVBVolCollision.h"

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING EmptyCollision : public BVolCollisionBase<BasicTraits>
{
public:
   typedef typename DoubleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGTriangle2BVol<BasicTraits,BVOL>                 AdapterType;
   typedef BVolGroup<BVOL>                                       GroupType;
   typedef BVolAdapter<BVOL>                                     GeneralType;
   typedef BVOL                                                  BVol;

   inline EmptyCollision ();

   // collision functions
   bool    InitDouble        (GroupType* root0, const Matrix& d0, 
			      GroupType* root1, const Matrix& d1);
   inline bool LeaveDouble (GroupType* root0, const Matrix& m0, 
			    GroupType* root1, const Matrix& m1);
   ResultT BVolBVolCollision (GroupType* b0, 
			      GroupType* b1);

   ResultT PrimBVolCollision (AdapterType* b0,
			      GroupType* b1);

   ResultT BVolPrimCollision (GroupType* b0, 
			      AdapterType* b1);

   ResultT PrimPrimCollision (AdapterType* b0, 
			      AdapterType* b1);

   virtual inline void dump (std::ostream& os);
};

template <class BasicTraits, class BVOL>
inline EmptyCollision<BasicTraits,BVOL>::EmptyCollision ()
  : BVolCollisionBase<BasicTraits>()
{
}
template <class BasicTraits, class BVOL>
inline bool EmptyCollision<BasicTraits,BVOL>::LeaveDouble (
GroupType*, const Matrix&, 
GroupType*, const Matrix&)
{
   return false;
}
template <class BasicTraits, class BVOL>
inline void EmptyCollision<BasicTraits,BVOL>::dump (std::ostream& os)
{
   os << "EmptyCollision";
}


template <class BasicTraits, class BVOL>
class OSG_GENVISLIB_DLLMAPPING EmptyCollisionTraits
{
public:
   typedef typename EmptyCollision<BasicTraits,BVOL>::CacheData   CacheData;
   typedef typename EmptyCollision<BasicTraits,BVOL>::ResultT     ResultT;
   typedef EmptyCollision<BasicTraits,BVOL>                       ObjectT;
   typedef typename EmptyCollision<BasicTraits,BVOL>::GeneralType GeneralType;
   typedef typename EmptyCollision<BasicTraits,BVOL>::GroupType   GroupType;
   typedef typename EmptyCollision<BasicTraits,BVOL>::AdapterType AdapterType;
   typedef typename EmptyCollision<BasicTraits,BVOL>::BVol        BVol;

   typedef InitSingleFunctor<bool,ObjectT>                                   InitFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,Matrix,GroupType,Matrix,ObjectT> InitDoubleFunctorT;
   typedef InitDoubleFunctor<bool,GroupType,Matrix,GroupType,Matrix,ObjectT> LeaveDoubleFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,GroupType,ObjectT> BVolBVolFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,GroupType,ObjectT> PrimBVolFunctorT;
   typedef DispatchFunctor<ResultT,GroupType,AdapterType,ObjectT> BVolPrimFunctorT;
   typedef DispatchFunctor<ResultT,AdapterType,AdapterType,ObjectT> PrimPrimFunctorT;

   static  InitFunctorT        createInitFunctor        (ObjectT* obj);
   static  InitDoubleFunctorT  createInitDoubleFunctor  (ObjectT* obj);
   static  LeaveDoubleFunctorT createLeaveDoubleFunctor (ObjectT* obj);
   static  BVolBVolFunctorT createBVolBVolFunctor (ObjectT* obj);
   static  PrimBVolFunctorT createPrimBVolFunctor (ObjectT* obj);
   static  BVolPrimFunctorT createBVolPrimFunctor (ObjectT* obj);
   static  PrimPrimFunctorT createPrimPrimFunctor (ObjectT* obj);
};

END_GENVIS_NAMESPACE

#endif



