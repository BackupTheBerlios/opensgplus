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

#include <OSGConfig.h>
#include <OSGNode.h>
#include <OSGMatrix.h>
#include <OSGMatrixUtility.h>

#include "OSGStereoCamera.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGStereoCamera.cpp,v 1.4 2002/01/10 08:23:05 marcus Exp $";
    static char cvsid_hpp[] = OSGSTEREOCAMERA_HEADER_CVSID;
    static char cvsid_inl[] = OSGSTEREOCAMERA_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::StereoCamera
A stereo perspective camera with a symmetric frustum.
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

StereoCamera::StereoCamera(void) :
    Inherited()
{
}

//! Copy Constructor

StereoCamera::StereoCamera(const StereoCamera &source) :
    Inherited(source)
{
}

//! Destructor

StereoCamera::~StereoCamera(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void StereoCamera::initMethod (void)
{
}

//! react to field changes

void StereoCamera::changed(BitVector, ChangeMode)
{
}

//! output the instance for debug purposes

void StereoCamera::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump StereoCamera NI" << endl;
}

void StereoCamera::getProjection( Matrix& result, UInt32 width, UInt32 height)  
{
    Matrix modelview;
    Real32 fov;

    fov=getFov();
    if(fov> Pi)
        fov = osgdegree2rad( fov );
    MatrixStereoPerspective(result,
                            modelview,
                            fov,
                            (double)width/(double)height,
                            getNear(),
                            getFar(),
                            getZeroparallax(),
                            getEyedistance(),
                            getWhicheye(),
                            1);
}

void StereoCamera::getViewing( Matrix& result, UInt32 , UInt32 )
{
    Matrix m;
    Real32 rEye = getEyedistance() * ( getWhicheye() - .5f );
    getBeacon()->getToWorld( result );  

    m.setIdentity();
    m[3][0] = rEye;

//    result.multLeft(m);
    result.mult(m);
    result.invert();
}




