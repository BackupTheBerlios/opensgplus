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

#ifndef OSGGVINPUT_H
#define OSGGVINPUT_H

#include "OSGGVBase.h"
#include "OSGGVTraits.h"
#include "OSGGVBVolAdapterExt.h"
#include "OSGGVBVolAdapter.h"
#include "OSGGVTriangleAdapterAligned.h"
#include "OSGGVGroup.h"

BEGIN_GENVIS_NAMESPACE

#if 0
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGInput
{
public:
   typedef BVOL                                     BVol;
   typedef OpenSGObjectBase<OpenSGTraits>           ObjectBase;
   typedef OpenSGObject2BVol<OpenSGTraits,BVOL>     Object2BVolAdapter;
   typedef OpenSGFaceBase<OpenSGTraits>             FaceBase;
   typedef OpenSGFace2BVol<OpenSGTraits,BVOL>       Face2BVolAdapter;
   typedef BVolGroup<BVOL>                          Group;
   typedef OpenSGTriangleBase<OpenSGTraits>         TriangleBase;
   typedef OpenSGTriangle2BVol<OpenSGTraits,BVOL>   Triangle2BVolAdapter;
   typedef BVolGroup<BVOL>                          TriangleGroup;
};
#endif

template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGInput
{
public:
   typedef BVOL                                     BVol;
};
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGObjectInput
{
public:
   typedef OpenSGObjectBase<OpenSGTraits>           BaseType;
   typedef OpenSGObject2BVol<OpenSGTraits,BVOL>     AdapterType;
   typedef BVolGroup<BVOL>                          GroupType;
   typedef typename AdapterType::BVol               BVol;
};
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGFaceInput
{
public:
   typedef OpenSGFaceBase<OpenSGTraits>             BaseType;
   typedef OpenSGFace2BVol<OpenSGTraits,BVOL>       AdapterType;
   typedef BVolGroup<BVOL>                          GroupType;
   typedef typename AdapterType::BVol               BVol;
};
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGPrimitiveInput
{
public:
   typedef OpenSGPrimitiveBase<OpenSGTraits>        BaseType;
   typedef OpenSGPrimitive2BVol<OpenSGTraits,BVOL>  AdapterType;
   typedef BVolGroup<BVOL>                          GroupType;
   typedef typename AdapterType::BVol               BVol;
};
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGTriangleInput
{
public:
   typedef OpenSGTriangleBase<OpenSGTraits>         BaseType;
   typedef OpenSGTriangle2BVol<OpenSGTraits,BVOL>   AdapterType;
   typedef BVolGroup<BVOL>                          GroupType;
   typedef typename AdapterType::BVol               BVol;
};
template <class BVOL>
class OSG_GENVISLIB_DLLMAPPING OpenSGTriangleAlignedInput
{
public:
   typedef OpenSGTriangleBase<OpenSGTraits>         BaseType;
   typedef OpenSGTriangleAligned<OpenSGTraits,BVOL> AdapterType;
   typedef BVolGroupAligned<BVOL>                   GroupType;
   typedef typename AdapterType::BVol               BVol;
};

END_GENVIS_NAMESPACE
#endif
