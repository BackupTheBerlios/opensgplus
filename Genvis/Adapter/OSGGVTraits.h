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

#ifndef OSGGVTRAITS_H
#define OSGGVTRAITS_H

#include "OSGGVBase.h"
#include "OSGGVCache.h"

#include "OSGMatrix.h"
#include "OSGGeometry.h"
#include "OSGTriangleIterator.h"
#include "OSGFaceIterator.h"
#include "OSGPrimitiveIterator.h"

BEGIN_GENVIS_NAMESPACE

class OSG_GENVISLIB_DLLMAPPING OpenSGTraits
{
public:
   typedef OSG::Matrix            TransformType;
   typedef OSG::NodePtr           GeomObjectType;
   typedef OSG::FaceIterator      GeomFaceType;
   typedef OSG::PrimitiveIterator GeomPrimitiveType;
   typedef OSG::TriangleIterator  GeomTriangleType;
   typedef OSGCache               Cache;
   typedef OSGCache::CacheData    CacheData;
};

END_GENVIS_NAMESPACE
#endif


