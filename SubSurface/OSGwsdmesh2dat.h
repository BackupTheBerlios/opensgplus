/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 * OpenSG - Dynamic Subdivision                                              *
 *                                                                           *
 *					  contact: v.settgast@cg.cs.tu-bs.de * 
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef WSDMESH2DAT_H
#define WSDMESH2DAT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <stdio.h>
#include "OSGwsddat.h"

OSG_BEGIN_NAMESPACE

/*! Mesh to patch conversion.
    The WSDmesh2dat class handles all mesh to patch data conversion plus
    the bounding box calculation (because it is done on the base mesh).
*/
template<class WSDVector, class Mesh, int mtype>
class OSG_SUBSURFACELIB_DLLMAPPING WSDmesh2dat  
{
   enum                                    { MType = mtype };
   typedef WSDVector                         VectorType;
   typedef WSDVector                         VectorCAType[4][wsdmaxvalenz];
   typedef Mesh                              MeshType;
   typedef typename MeshType::HalfedgeHandle HalfedgeHandle;
   typedef typename MeshType::FaceHandle     FaceHandle;
   typedef typename MeshType::Point	     OMPoint;
   typedef typename MeshType::TexCoord	     OMTexCoord;

 /*==========================  PUBLIC  =================================*/
public:

   //! Constructor
   WSDmesh2dat(MeshType *m);
   //! Destructor
   virtual ~WSDmesh2dat();

   //! flag for texture usage
   bool useTexture;

   //! one face into patch
   //! \param f_h face handle to one face
   //! \param pp pointer to the destination patch
   void insertface     (FaceHandle f_h, FaceHandle tpair,WSDdat<VectorType, MType> *pp);
   //! calculate the bounding box of the base mesh
   void getBoundingBox (OSG::Pnt3f& boundingMin, OSG::Pnt3f& boundingMax);

 /*==========================  PRIVATE  =================================*/
private:  			

   //! helper for the corners
   void collectCorner (Int32 i, HalfedgeHandle &fhe_h, HalfedgeHandle &op);
   void helper1       (Int32 i, Int32 a2, Int32 a7, HalfedgeHandle &fhe_h);
   void helper2       (Int32 i, Int32 a2, Int32 a7, HalfedgeHandle &fhe_h);

   //! helper to make the code simpler
   void p_nach(const OMPoint &p, Int32 x, Int32 y);

   HalfedgeHandle getfirsthalfedge   (FaceHandle f_h, FaceHandle tpair);
   Int32          vertexIsRegular    (HalfedgeHandle fhe_h);
   Int32          getCreaseType_Edge (OSG::Int32 crease, Int32 outer, Int32 inner);
     
   MeshType *mesh;                            //!< pointer to the mesh
   WSDdat<VectorType, MType> *ppatch;      //!< pointer to the current patch
};

OSG_END_NAMESPACE

#endif 
