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

#include "OSGDVRShader.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGDVRShader.cpp,v 1.1 2002/10/10 11:11:26 weiler Exp $";
    static char cvsid_hpp[] = OSGDVRSHADER_HEADER_CVSID;
    static char cvsid_inl[] = OSGDVRSHADER_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::DVRShader
Abstract shader class - not to be instantiated. Is intended to be stateless - NO FIELDS!!
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

DVRShader::DVRShader(void) :
    Inherited()
{
}

//! Copy Constructor

DVRShader::DVRShader(const DVRShader &source) :
    Inherited(source)
{
}

//! Destructor

DVRShader::~DVRShader(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void DVRShader::initMethod (void)
{
}

//! react to field changes

void DVRShader::changed(BitVector, UInt32)
{
}

//! output the instance for debug purposes

void DVRShader::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump DVRShader NI" << std::endl;
}

