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
//   $Date: 2004/12/20 15:59:52 $
//                                                                            
//=============================================================================

#ifndef OSGGVSPATIALIZE_H
#define OSGGVSPATIALIZE_H

#include "OSGGVBase.h"
#include "OSGGVInput.h"
#include "OSGGVSingleTraverser.h"
#include "OSGGVGroup.h"

#include "OSGGroup.h"
#include "OSGGeometry.h"

BEGIN_GENVIS_NAMESPACE

template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING Spatialize
{
public:
   typedef BoundingVolume<Real>                                  BVol;
   typedef typename SingleTraverserBase<BasicTraits>::ResultType ResultT;
   typedef OpenSGObjectBase<BasicTraits>                         AdapterType;
   typedef BVolGroupBase                                         GroupType; 
   typedef BVolAdapterBase                                       GeneralType; 
   typedef typename BasicTraits::TransformType                   TransformType; 

   Spatialize ();
   ~Spatialize ();

   OSG::NodePtr getRoot () const;
   void         setDepth (u32 level);
   u32          getDepth () const;

   bool     InitEnter  (GeneralType* a, const TransformType& m);
   bool     InitLeave  (GeneralType* a, const TransformType& m);
   ResultT  InnerEnter (GroupType*   g0);
   ResultT  InnerLeave (GroupType*   g0);
   ResultT  LeafEnter  (AdapterType* p0);

   static OSG::GroupPtr getGroupCore ();
   static void          setGroupCore (const OSG::GroupPtr& group);

private:
   static OSG::GroupPtr s_group;
   OSG::NodePtr m_root;
   OSG::NodePtr m_rootLevel;
   std::vector<OSG::NodePtr> m_rootStack;
   unsigned     m_depth;
   unsigned     m_current;
};


template <class BasicTraits>
class OSG_GENVISLIB_DLLMAPPING SpatializeTraits
{
public:
   typedef typename Spatialize<BasicTraits>::BVol        BVol;
   typedef typename Spatialize<BasicTraits>::ResultT     ResultT;
   typedef Spatialize<BasicTraits>                       ObjectT;
   typedef typename Spatialize<BasicTraits>::GeneralType GeneralType;
   typedef typename Spatialize<BasicTraits>::AdapterType AdapterType;
   typedef typename Spatialize<BasicTraits>::GroupType   GroupType;
   typedef typename Spatialize<BasicTraits>::TransformType TransformType; 

   typedef InitSingleFunctor2<bool,GeneralType,TransformType,ObjectT> InitFunctorT;
   typedef SingleFunctor<ResultT,GroupType,ObjectT>   InnerFunctorT;
   typedef SingleFunctor<ResultT,AdapterType,ObjectT> LeafFunctorT;

   static  InitFunctorT  createInitFunctor      (ObjectT* obj);
   static  InitFunctorT  createInitLeaveFunctor (ObjectT* obj);
   static  InnerFunctorT createInnerFunctor     (ObjectT* obj);
   static  InnerFunctorT createInnerLeaveFunctor(ObjectT* obj);
   static  LeafFunctorT  createLeafFunctor      (ObjectT* obj);
};

END_GENVIS_NAMESPACE
#endif

