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

#include "OSGDVRVolumeTexture.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRVolumeTexture.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static char cvsid_hpp[] = OSGDVRVOLUMETEXTURE_HEADER_CVSID;
    static char cvsid_inl[] = OSGDVRVOLUMETEXTURE_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::DVRVolumeTexture
Attachment for storing and handling Volume-Textures for volume rendering. Here is the place of the voxel values.
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

DVRVolumeTexture::DVRVolumeTexture(void) :
    Inherited()
{
}

//! Copy Constructor

DVRVolumeTexture::DVRVolumeTexture(const DVRVolumeTexture &source) :
    Inherited(source)
{
}

//! Destructor

DVRVolumeTexture::~DVRVolumeTexture(void)
{
#ifdef MW_OSG_1_1
    if (getImage() != NULL) {
        delete getImage();
	setImage(NULL);
    }
#else
    getImage()->subRef();
    setImage(NULL);
#endif
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void DVRVolumeTexture::initMethod (void)
{
}

//! react to field changes

void DVRVolumeTexture::changed(BitVector whichField, UInt32 origin)
{
    DVRVolumeTexturePtr thisP(*this);
  
    FDEBUG(("DVRVolumeTexture::changed - \n"));
  
    if (whichField & ImageFieldMask) {
        FINFO(("DVRVolumeTexture::changed - new Image\n"));
	_mfHistogram.resize(256);
	
        // update histogram
        Real32 * hist = &(_mfHistogram[0]);
	UChar8 * img  = getImage()->getData();
	Real32 maxVal = 0;
	
	for (UInt32 i = 0; i < 256; i++)
	    hist[i] = 0;

	if (img == NULL) return;

	FDEBUG(("Image: %d %d %d\n",
		getImage()->getWidth(), getImage()->getHeight(), getImage()->getDepth()));
	
	for (UInt32 j = 0;
	     j < getImage()->getWidth() * getImage()->getHeight() * getImage()->getDepth();
	     j++)
	{
//	  cerr << j << ": " << (int) img[j] << std::endl ;
	    hist[img[j]]++;
	}

	for (UInt32 k = 0; k < 256; k++)
	    maxVal = (hist[k] > maxVal) ? hist[k] : maxVal;

	_sfMaxVal.setValue(maxVal);


	// update sliceThickness
	const std::string * sTatt = getImage()->findAttachment("SliceThickness");
	if (sTatt) {
	  double sT[3];
	  sscanf(sTatt->c_str(), "%lf %lf %lf", &sT[0], &sT[1], &sT[2]);
	  _sfSliceThickness.setValue(Vec3f(sT[0], sT[1], sT[2]));
	} else {
	  SWARNING << "DVRVolumeTexture::changed - No SliceThickness found" << std::endl;
	}

	// update resolution
	const std::string * resAtt = getImage()->findAttachment("Resolution");
	if (resAtt) {
	  double sT[3];
	  sscanf(resAtt->c_str(), "%lf %lf %lf", &sT[0], &sT[1], &sT[2]);
	  _sfResolution.setValue(Vec3f(sT[0], sT[1], sT[2]));
	} else {
	  SWARNING << "DVRVolumeTexture::changed - No Resolution found" << std::endl;
	}

	//!! FIXME:
	//!! you can either change the image pointer or the file name
	whichField &= ~FileNameFieldMask;
    }

    if (whichField & FileNameFieldMask)
    {
        FINFO(("DVRVolumeTexture::changed - new fileName\n"));

	ImageP datImage = new Image();
#ifndef MW_OSG_1_1
	datImage->addRef();
#endif
	datImage->read(_sfFileName.getValue().c_str());
	datImage->dump();
	beginEditCP(thisP, ImageFieldMask);
	setImage(datImage);
	endEditCP  (thisP, ImageFieldMask);

#ifndef MW_OSG_1_1
	datImage->subRef();
#endif
    }

    Inherited::changed(whichField, origin);
}

//! output the instance for debug purposes

void DVRVolumeTexture::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump DVRVolumeTexture NI" << std::endl;
}

