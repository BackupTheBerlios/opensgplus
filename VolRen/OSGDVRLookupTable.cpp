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

#include "OSGDVRLookupTable.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRLookupTable.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static char cvsid_hpp[] = OSGDVRLOOKUPTABLE_HEADER_CVSID;
    static char cvsid_inl[] = OSGDVRLOOKUPTABLE_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::DVRLookupTable
Attachment for storing and handling Lookup-Tables for volume rendering.
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

DVRLookupTable::DVRLookupTable(void) :
    Inherited()
{
    commonConstructor();
}

//! Copy Constructor

DVRLookupTable::DVRLookupTable(const DVRLookupTable &source) :
    Inherited(source)
{
    commonConstructor();
}

//! Destructor

DVRLookupTable::~DVRLookupTable(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void DVRLookupTable::initMethod (void)
{
}

//! react to field changes

void DVRLookupTable::changed(BitVector whichField, UInt32)
{
    SINFO << "DVRLookupTable::changed" << std::endl;

    if (whichField & DataFieldMask) {
      // copy data from interleaved field to single fields
      UInt32 iter = 0;
      for (UInt32 i = 0; i < _sfDimension.getValue(); i++) {
	for (UInt32 j = 0; j < _mfSize.getValue(i); j++) {
	  for (UInt32 k = 0; k < _sfChannel.getValue(); k++) {

	    //!! setValue ( <value>, index )
	    if (k == 0)
	      _mfDataR.setValue((Real32) _mfData.getValue(iter++) / (Real32) TypeConstants<UInt8>::getMax(), j);
	    if (k == 1)
	      _mfDataG.setValue((Real32) _mfData.getValue(iter++) / (Real32) TypeConstants<UInt8>::getMax(), j);
	    if (k == 2)
	      _mfDataB.setValue((Real32) _mfData.getValue(iter++) / (Real32) TypeConstants<UInt8>::getMax(), j);
	    if (k == 3)
	      _mfDataA.setValue((Real32) _mfData.getValue(iter++) / (Real32) TypeConstants<UInt8>::getMax(), j);
	  }
	}
      }
    }


    if (whichField & DataRFieldMask) {
        // copy data from R field to channel 0 of interleaved field
        UInt32 iter = 0;
	UInt32 numChannels = _sfChannel.getValue();
	for (UInt32 i = 0; i < _sfDimension.getValue(); i++) {
	  for (UInt32 j = 0; j < _mfSize.getValue(i); j++) {
	    //!! setValue ( <value>, index )
	    _mfData.setValue((Real32) _mfDataR.getValue(j) * (Real32) TypeConstants<UInt8>::getMax(), iter);
	    iter+=numChannels;
	  }
	}
    }

    
    if (whichField & DataGFieldMask) {
        // copy data from G field to channel 1 of interleaved field
	UInt32 iter = 1;
        UInt32 numChannels = _sfChannel.getValue();
	for (UInt32 i = 0; i < _sfDimension.getValue(); i++) {
	  for (UInt32 j = 0; j < _mfSize.getValue(i); j++) {
	    //!! setValue ( <value>, index )
	    _mfData.setValue((Real32) _mfDataG.getValue(j) * (Real32) TypeConstants<UInt8>::getMax(), iter);
	    iter+=numChannels;
	  }
	}
    }

    if (whichField & DataBFieldMask) {
        // copy data from B field to channel 2 of interleaved field
	UInt32 iter = 2;
        UInt32 numChannels = _sfChannel.getValue();
	for (UInt32 i = 0; i < _sfDimension.getValue(); i++) {
	  for (UInt32 j = 0; j < _mfSize.getValue(i); j++) {
	    //!! setValue ( <value>, index )
	    _mfData.setValue((Real32) _mfDataB.getValue(j) * (Real32) TypeConstants<UInt8>::getMax(), iter);
	    iter+=numChannels;
	  }
	}
    }

    
    if (whichField & DataAFieldMask) {
        // copy data from A field to to channel 3 interleaved field
	UInt32 iter = 3;
        UInt32 numChannels = _sfChannel.getValue();
	for (UInt32 i = 0; i < _sfDimension.getValue(); i++) {
	  for (UInt32 j = 0; j < _mfSize.getValue(i); j++) {
	    //!! setValue ( <value>, index )
	    _mfData.setValue((Real32) _mfDataA.getValue(j) * (Real32) TypeConstants<UInt8>::getMax(), iter);
	    iter+=numChannels;
	  }
	}
    }

    
    if (whichField & (DimensionFieldMask | SizeFieldMask | ChannelFieldMask)) {
        SINFO << "DVRLookupTable::changed - DimensionFieldMask | SizeFieldMask | ChannelFieldMask " << std::endl;
        commonConstructor();
    }


    // mark table as being changed
    setTouched( true );
    

    // notify parent if appearance 
    MFFieldContainerPtr * par = getMFParents();
    for (UInt32 i = 0; i < par->size(); i++) {
    }
}


//! output the instance for debug purposes

void DVRLookupTable::dump(      UInt32    , 
                         const BitVector ) const
{
    SINFO << "Dump DVRLookupTable NI" << std::endl;

    UInt32 i = 0;

    for (i = 0; i < _mfDataR.size() / 2; i++)
      SLOG << _mfDataR.getValue(i) << _mfDataR.getValue(i + 1) << std::endl;
    for (i = 0; i < _mfDataG.size() / 2; i++)
      SLOG << _mfDataG.getValue(i) << _mfDataG.getValue(i + 1) << std::endl;
    for (i = 0; i < _mfDataB.size() / 2; i++)
      SLOG << _mfDataB.getValue(i) << _mfDataB.getValue(i + 1) << std::endl;
    for (i = 0; i < _mfDataA.size() / 2; i++)
      SLOG << _mfDataA.getValue(i) << _mfDataA.getValue(i + 1) << std::endl;
}


//! used to initialize member variables - called by every constructor
void DVRLookupTable::commonConstructor( void )
{
  UInt32 size = _sfDimension.getValue();
  
  // set default size for every dimension
  _mfSize.resize(_sfDimension.getValue());
  for (UInt32 j = 0; j < _sfDimension.getValue(); j++) {
    _mfSize.setValue(256, j);
    size *= _mfSize.getValue(j);
  }

  // allocate data field
  _mfData.resize(size * _sfChannel.getValue());
  _mfDataR.resize(size);
  _mfDataG.resize(size);
  _mfDataB.resize(size);
  _mfDataA.resize(size);
  
  // store default ramp
  UInt32 iter = 0;
  UInt32 singleIter = 0;
  for (UInt32 i = 0; i < _sfDimension.getValue(); i++) {
    for (UInt32 j = 0; j < _mfSize.getValue(i); j++) {
      
      //!! setValue ( <value>, index )
      _mfDataR.setValue((Real32) j / (Real32) TypeConstants<UInt8>::getMax(), singleIter);
      _mfDataG.setValue((Real32) j / (Real32) TypeConstants<UInt8>::getMax(), singleIter);
      _mfDataB.setValue((Real32) j / (Real32) TypeConstants<UInt8>::getMax(), singleIter);
      _mfDataA.setValue((Real32) j / (Real32) TypeConstants<UInt8>::getMax(), singleIter);

      singleIter++;
      
      for (UInt32 k = 0; k < _sfChannel.getValue(); k++) {
	_mfData.setValue(j, iter++);
      }
    }
  }
}
