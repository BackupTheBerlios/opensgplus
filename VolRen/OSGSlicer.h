#ifndef _OSGSLICER_H_
#define _OSGSLICER_H_

#ifdef __sgi
#pragma once
#pragma set woff 3201 // unreferenced variables
#endif


#include <OSGConfig.h>

#include <OSGVector.h>
#include <OSGMatrix.h>
#include <OSGDrawActionBase.h>


OSG_USING_NAMESPACE


// forward references


class Slicer {
  
  public:
    enum SliceDir {
      SD_X_FRONT_TO_BACK,
      SD_X_BACK_TO_FRONT,
      SD_Y_FRONT_TO_BACK,
      SD_Y_BACK_TO_FRONT,
      SD_Z_FRONT_TO_BACK,
      SD_Z_BACK_TO_FRONT
    };

    static void getModelMatrix(DrawActionBase * da, Matrix mat);

    static void getViewMatrix(DrawActionBase * da, Matrix mat);

    static int getAASlicingDirection(DrawActionBase * da, Vec3f *pViewDir);
    
    static int getSlicingDirection(DrawActionBase * da, Vec3f *pViewDir);

    static void rotateToLocal(DrawActionBase * da, Vec3f &in, Vec3f &out);
    
    static bool isBackToFront(int sliceDir);
};

#endif
