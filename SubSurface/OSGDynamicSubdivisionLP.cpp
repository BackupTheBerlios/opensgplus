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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

//#define OSG_COMPILEDYNAMICSUBDIVISIONLPINST

#include "OSGwsdmain.h"
#include "OSGDynamicSubdivisionLP.h"
#include "OSGConfig.h"
#include "OSGRenderAction.h"
#include "OSGViewport.h"
#include "OSGCamera.h"
#include "OSGCameraDecorator.h"
#include "OSGPerspectiveCamera.h"
#include "OSGLog.h"

OSG_USING_NAMESPACE

/*! \class osg::DynamicSubdivisionLP

*/

/*----------------------- constructors & destructors ----------------------*/

template <class MESH>
DynamicSubdivisionLP<MESH>::DynamicSubdivisionLP(void) :
    Inherited()
{
}

template <class MESH>
DynamicSubdivisionLP<MESH>::DynamicSubdivisionLP(const DynamicSubdivisionLP &source) :
    Inherited(source)
{
}

template <class MESH>
DynamicSubdivisionLP<MESH>::~DynamicSubdivisionLP(void)
{
   if (getTesselator() != NULL) {
      delete getTesselator();
   }
}

/*----------------------------- class specific ----------------------------*/

template <class MESH>
void DynamicSubdivisionLP<MESH>::initMethod (void)
{
#if 0
    DrawAction::registerEnterDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> >  , 
            CNodePtr          ,  
            Action           *>(&DynamicSubdivisionLP<MESH>::drawEnter));
#endif  
    RenderAction::registerEnterDefault(
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            FCPtr<GroupPtr, DynamicSubdivisionLP<MESH> >  , 
            CNodePtr          ,  
            Action           *>(&(DynamicSubdivisionLP<MESH>::renderEnter)));
}

template <class MESH>
void DynamicSubdivisionLP<MESH>::changed(BitVector whichField, UInt32 origin)
{
   Inherited::changed(whichField, origin);    
   bool update_parents = false;
  
   // changed MinProjSize:
   // * update in tesselator
   if (whichField & MinProjSizeFieldMask) {
      if (getTesselator() != NULL) {
         getTesselator()->Pmin = getMinProjSize();         
      }
   }   
   // changed MaxProjSize:
   // * update in tesselator
   if (whichField & MaxProjSizeFieldMask) {
      if (getTesselator() != NULL) {
         getTesselator()->Pmax = getMaxProjSize();         
      }
   }
   // changed VertexClassifier:
   // * update in tesselator
   if (whichField & VertexClassifierFieldMask) {
      if (getTesselator() != NULL) {
         getTesselator()->VertexClassifier = getVertexClassifier();         
      }
   }   
   // changed NormalConeAperture:
   // * update in tesselator
   if (whichField & NormalConeApertureFieldMask) {
      if (getTesselator() != NULL) {
         getTesselator()->NormalConeAperture = getNormalConeAperture();         
      }
   }   
   // changed BackfaceCulling:
   // * update in tesselator
   if (whichField & BackfaceCullingFieldMask) {
      if (getTesselator() != NULL) {
         getTesselator()->isSetBFCull = getBackfaceCulling();         
      }
   }      
   // changed MinDepth
   // * update in tesselator
   if (whichField & MinDepthFieldMask) {
      if (getTesselator() != NULL) {
         getTesselator()->setMinDepth(getMinDepth());
      }
   } 
   // changed MeshField: 
   // * create tesselator
   // * do tesselator preprocessing  
   if (whichField & MeshFieldMask
      || whichField & MaxDepthFieldMask ) {    // mesh has been altered or maxdepth
      SINFO << "Mesh changed" << std::endl;
      if (getTesselator() != NULL) {
         delete getTesselator();
      }
      if (getMesh() != NULL) {
         setTesselator(new OpenMeshTesselator(getMesh()));
         // only chance to adjust the maxdepth
         getTesselator()->setMaxDepth(getMaxDepth());
         getTesselator()->setMinDepth(getMinDepth());
         // update fields-data in tesselator
         getTesselator()->Pmin = getMinProjSize();
         getTesselator()->Pmax = getMaxProjSize();
         getTesselator()->VertexClassifier = getVertexClassifier();
         getTesselator()->NormalConeAperture = getNormalConeAperture();
         getTesselator()->isSetBFCull = getBackfaceCulling();
      } else {
         SWARNING << "Mesh is NULL" << std::endl;
      }
   }
   // changed ParentsField: 
   // * create Instance-List
   // * create Geometry-nodes below
   if (update_parents || 
      ((getTesselator() != NULL) &&        
       (whichField & ParentsFieldMask))) {      // parentsfield changed

      getTesselator()->clearInstances();        // delete old instances list
      if (getParents().size()>0 && getTesselator()->patchesready) {        
      UInt32 i;
      for (i=0; i<getParents().size(); ++i) {
         NodePtr parent = getParents()[i];
         getTesselator()->initInstance(i,parent);        // geoknoten erzeugen                  
         parent->invalidateVolume();         
      }      
    }      
  }
}

template <class MESH>
void DynamicSubdivisionLP<MESH>::dump (UInt32          uiIndent, 
               const BitVector bvFlags) const
{
    indentLog(uiIndent, PLOG);
    PLOG << getName()
         << std::endl;
}


template <class MESH>
Action::ResultE DynamicSubdivisionLP<MESH>::drawEnter(Action *action)
{    
    return Group::drawEnter(action);
}


template <class MESH>
void DynamicSubdivisionLP<MESH>::prepareFrame (const ViewportPtr& port)
{     
   prepareFrame(port.getCPtr(),NULL);
}

template <class MESH>
void DynamicSubdivisionLP<MESH>::prepareFrame (const Viewport* port, Action* action)
{     
   SINFO << getName() << ": prepareFrame" << std::endl;
   // is getTesselator valid?
   if (getTesselator() != NULL) { 
      

      // get camera object
      Matrix  camMatrix, transMatrix;     
      CameraPtr cam = port->getCamera();
      if (cam == NullFC || cam->getBeacon() == NullFC) {
         return;
      }
      camMatrix = cam->getBeacon()->getToWorld();
      // is view port dependant data set?
      //if (!getTesselator()->isSetViewPort) {
      PerspectiveCameraPtr percam = PerspectiveCameraPtr::dcast(cam);
      if (percam == NullFC) {
         CameraDecoratorPtr dcam = CameraDecoratorPtr::dcast(cam);
         if (dcam == NullFC) {
            return;
         }
         percam = PerspectiveCameraPtr::dcast(dcam->getDecoratee());
         if (percam == NullFC) {
            return;
         }
      }
      Real32 winkel = percam->getFov();
      Int32  yres = port->getPixelHeight();
      Int32  xres = port->getPixelWidth();
      getTesselator()->initViewPortVars(winkel,xres,yres);
      //}         
      // frame setup for parent
      Vec3f eye;
      if (action != NULL) {      
         // get camera position
         transMatrix = action->getActNode()->getToWorld();
         transMatrix.invert();
         transMatrix.mult(camMatrix);
         eye.setValues(transMatrix[3][0], transMatrix[3][1], transMatrix[3][2]); 
         getTesselator()->perFrameSetup(action->getActNode(), eye);
      } else {
         // for manual calls we don't have an action pointer
         for (UInt32 i=0; i<getParents().size(); ++i) {
            // get camera position
            transMatrix = getParents()[i]->getToWorld();
            transMatrix.invert();
            transMatrix.mult(camMatrix);
            eye.setValues(transMatrix[3][0], transMatrix[3][1], transMatrix[3][2]);          
            getTesselator()->perFrameSetup(getParents()[i], eye);         
         }
      }
   }
}


template <class MESH>
Action::ResultE DynamicSubdivisionLP<MESH>::renderEnter (Action* action)
{  
   SINFO << getName() << ": renderEnter" << std::endl;
   RenderAction *da = dynamic_cast<RenderAction *>(action);
   if (da != NULL) {
      // is getTesselator valid?
      if (getTesselator() != NULL) { 
         // first time call: patches not initialized
         if ((!getTesselator()->patchesready) && (!getTesselator()->errorcase)) {

            OSG::GeometryPtr geop = NullFC;

            if (getParents().size()>0) {
               SINFO << "searching in parent 0 for children!" << std::endl;
               NodePtr parent = getParents()[0];
               if (parent->getNChildren()>0) {
                  SINFO << "searching " << parent->getNChildren() << " children!" << std::endl;
                  for (UInt32 i=0; i<parent->getNChildren(); i++) {                     
                     NodePtr child = parent->getChild(i);
                     char* child1001=(char*)OSG::getName(child);
                     if (strcmp(child1001,"DynSubdivNode") == 0) {
                        SINFO << "Geometry found for recycling!" << std::endl;
                        geop = GeometryPtr::dcast(child->getCore());
                     }
                     // TODO: release memory?
                  }
               }
            }

            getTesselator()->initPatches(geop);
            // now we need to init the instances:
         
           
            getTesselator()->clearInstances();        // delete old instances list
            if (getParents().size()>0 && getTesselator()->patchesready) {        
               UInt32 i;
               for (i=0; i<getParents().size(); ++i) {
                  NodePtr parent = getParents()[i];
                  getTesselator()->initInstance(i,parent);        // geoknoten erzeugen                  
                  parent->invalidateVolume();         
               }    
            }

            // ok, we do one uniformSetup now
            getTesselator()->uniformSetup(); 
            
         }

         // if AutoUpdate is true, prepareFrame is called every frame
         if (getAutoUpdate()) {
            this->prepareFrame(da->getViewport(),action);
         }


      }
   }


#if 0
   RenderAction *da = dynamic_cast<RenderAction *>(action);
   if (da != NULL) {
      // is getTesselator valid?
      if (getTesselator() != NULL) { 
         // get camera object
         CameraP cam = da->getCamera();
         Matrix  camMatrix, transMatrix;     
         camMatrix = da->getCameraToWorld();
         // is view port dependant data set?
         if (!getTesselator()->isSetViewPort) {
            PerspectiveCameraP percam = dynamic_cast<PerspectiveCameraP>(cam);
            Real32    winkel = percam->getFov();
            ViewportP vp = da->getViewport();
            Int32     yres = vp->getPixelHeight();
            Int32     xres = vp->getPixelWidth();           
            getTesselator()->initViewPortVars(winkel,xres,yres);
         }
         // get camera position
         NodePtr parent = da->getActNode();
         transMatrix = parent->getToWorld();              
         transMatrix.invert();
         transMatrix.mult(camMatrix);
         Vec3f eye(transMatrix[3][0], transMatrix[3][1], transMatrix[3][2]);    
         // frame setup for parent
         getTesselator()->perFrameSetup(parent, eye);
      }
   }
#endif
   return Group::renderEnter(action);
}



template <class MESH>
void DynamicSubdivisionLP<MESH>::adjustVolume    (Volume &volume)
{
   if (getTesselator() == NULL) {
      return;
   }   
   Pnt3f boundingMin, boundingMax;
   getTesselator()->OpenMeshBoundingBox(boundingMin, boundingMax);   
   volume.setValid(false);
   volume.setEmpty();
   volume.extendBy(boundingMin);
   volume.extendBy(boundingMax);
   volume.setValid();
}

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

#if 0
namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDynamicSubdivisionLP.cpp,v 1.7 2004/07/09 08:57:03 fuenfzig Exp $";
    static char cvsid_hpp[] = OSGDYNAMICSUBDIVISIONLP_HEADER_CVSID;
    static char cvsid_inl[] = OSGDYNAMICSUBDIVISIONLP_INLINE_CVSID;
}
#endif

#ifdef __sgi
#pragma reset woff 1174
#endif

