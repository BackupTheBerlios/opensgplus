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
 *            contact: v.settgast@cg.cs.tu-bs.de * 
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

#ifndef WSDMAIN_H
#define WSDMAIN_H

//#pragma warning( disable : 4661) 

#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <vector>
#include <OpenMesh/Core/Utils/Property.hh>
#include "OSGwsddat.h"
#include "OSGwsdmesh2dat.h"
#include "OSGwsdsubdiv.h"
#include "OSGwsdpairing.h"
#include "OSGConfig.h"
#include "OSGSubSurfaceDef.h"

OSG_BEGIN_NAMESPACE

//! datastruct for vertices
template<class WSDVector, class Mesh> 
struct OSG_SUBSURFACELIB_DLLMAPPING SVertexData
{
   typedef Mesh                            MeshType;
   typedef WSDVector                       VectorType;
   typedef typename MeshType::VertexHandle VertexHandle;

   VertexHandle             vh;
   FacingType              ft;  // back, siluette or front
   VectorType               limPo;
   VectorType               limNo;
   Int32                    maxdepth;
};
//! struct for quick subdivision while approx. the curvature 
template <class Mesh> 
struct OSG_SUBSURFACELIB_DLLMAPPING workArray
{
   typedef Mesh                          MeshType;
   typedef typename MeshType::FaceHandle FaceHandle;
   typedef typename MeshType::Point   OMPoint;

   OMPoint  p;
   FaceHandle  fh;
   Int32  isCrease;
};

/*! Tesselator class.
*/
template<class WSDVector, class Mesh, int mtype>
class OSG_SUBSURFACELIB_DLLMAPPING WSDmain  
{
   enum                                    { MType = mtype };
   typedef Mesh                              MeshType;
   typedef WSDVector                         VectorType;
   typedef workArray<MeshType>               WorkType;
   typedef WorkType                          WorkArrayType[wsdmaxvalenz*2];
   typedef typename MeshType::FaceHandle     FaceHandle;
   typedef typename MeshType::HalfedgeHandle HalfedgeHandle;
   typedef typename MeshType::VertexHandle   VertexHandle;
   typedef typename MeshType::Point          OMPoint;

   typedef std::vector<perInstanceData>   perInstanceDataContainer;
   typedef WSDdat<VectorType, MType>      PatchData;
   typedef std::vector<PatchData>         PatchDataContainer;
   typedef std::vector<OSG::UInt16>       PatchIndexContainer;

   /*==========================  PUBLIC  =================================*/
public:
   /*! Constructor                                */
   WSDmain(MeshType *m);
   
   /*! Destructors                                */   
   virtual ~WSDmain();

   //! method to set the maximum tesselation depth
   void setMaxDepth(UInt16 setdepth);

   void setMinDepth(UInt16 setdepth);

   sharedFields            mySharedFields;    //!< Limitpoints, -normals are being shared
   perInstanceDataContainer  myInstances;       //!< "per parent one Instance"-data

   void initOSGStuff(Int32 fsize);                 //!< init OpenSG geodata and shared data
   void initPatches();                      //!< init patches (preprocessing)
   void initInstance(Int32 n, OSG::NodePtr& parent);  //!< init instances
   void clearInstances(void);                  //!< completely remove instances

   //! output method where the geometry is set up
   void perFrameSetup(OSG::NodePtr& parent, OSG::Vec3f eyepoint);

   //! call through for WSDmesh2dat::getBoundingBox   
   void OpenMeshBoundingBox(OSG::Pnt3f &boundingMin, OSG::Pnt3f &boundingMax);
 
   //! returns index into patches for face handle
   Int32 getFaceIndex (FaceHandle fh);  

   //! Vertexlist
   std::vector<SVertexData<VectorType, MeshType> > VertexListe;

#ifdef DEFINE_SHOWROOM
   UInt32 numtris;
   UInt32 numquads;
   UInt32 depthsInUse[30];          // never use a maxdepth higher than 30!
   UInt16 uniDepth;                 //!< if curvature is deactivated, use this depth
   UInt32 numPatches;               //!< number of patches

   UInt32 maxindissize;
#endif

   //! Flags
   bool patchesready;      //!< flag: are the patches ready?
   bool isSetViewPort;      //!< flag: are the viewport parameters set?
   bool isSetBFCull;          //!< flag: perform backface culling?

   bool useCreases;
   //! property within openmesh
   OpenMesh::EPropHandleT<Int32> isCrease;


#ifdef DEFINE_SHOWROOM
   bool useCurvature;
   bool useProjSize;
   bool useSilhouette;
#endif

   //! \name for the projected patch size
   //! \{

   //! setup the viewport parameters
   //! \param fovy field of view in y direction
   //! \param resX viewport width in pixel
   //! \param resY viewport height in pixel
   void initViewPortVars(Real32 fovy, Int32 resX, Int32 resY);
   //! \}

   Real32 gamma_winkel;      //!< angle for one pixel

   //! \name projected size parameters
   //! \{

   Real32 hmin,hmax,hminSil;  
   Real32 Pmin,Pmax,PminSil;   
   Real32 VertexClassifier;        //!< silhouette epsilon
   Real32 NormalConeAperture;          //!< curvature epsilon
   //! \}

   //! \name for the curvature approximation:
   //! \{
   
   //! \param pmesh pointer to base mesh
   //! \param v_h vertex for calculation
   //! for a given vertex return the optimal depth
   void getOptiDepth(MeshType *pmesh, VertexHandle v_h);
   //! \}

 /*==========================  PRIVATE  =================================*/
 private:
   MeshType *pmesh;             //!< the mesh
   PatchDataContainer patches;         //!< the patches
   PatchIndexContainer singles;        //!< Loop only: indices to single triangles

   UInt16 wsdmaxdepth;         //!< maximum tesselation depth (default is 4)
   UInt16 wsdmindepth;         //!< minimum tesselation depth (default is 0)

   //! flag for texture usage
   bool useTexture;

#ifdef DEFINE_SHOWROOM
   UInt32 _depthsInUse[30];          // never use a maxdepth higher than 30!
#endif

   //! class for reading the mesh data
   WSDmesh2dat<VectorType, MeshType, MType> mesh2wsd;  
   //! class containing the subdivision algorithms
   WSDsubdiv<VectorType, MType>             subdivwsd;

   //! \name for the curvature calculation
   //! \{
   OMPoint NormInner          (WorkType* wara_, Int32 v);
   OMPoint NormCrease         (OMPoint alpha_, WorkType* wara_, Int32 v, Int32 g, Int32 h);
   void    simpleSubdiv       (WorkType* wara_, WorkType* waB_, OMPoint &a, Int32 i);
   void    simpleCreaseSubdiv (WorkType* wara_, WorkType* waB_, OMPoint &a,
                               Int32 g, Int32 h, Int32 i);
   Int32   getLimOffset       (WSDdat<VectorType, MType> *pp, OMPoint a);
   Real32  getGreatestAngle   (WorkType* wara_, OMPoint &a, OMPoint &n, 
                               Int32 s, Int32 num, Int32 size);
   //! \}

   //! \name some helpers
   //! \{

   void collectNeighbors(FaceHandle f_h,WSDdat<VectorType, MType> *pp);   

   void initOneGeo(perInstanceData *instance);        //!< OpenSG - Geometry init (per Instance)
   bool isQuad(FaceHandle f_h);                       //!< returns true if f_h is a quad
   Int32 getIndex(OSG::NodePtr &p);

   //! property within openmesh (patch index)
   OpenMesh::FPropHandleT<Int32> patchIndex;
   //! \}
};

OSG_END_NAMESPACE

#endif 



