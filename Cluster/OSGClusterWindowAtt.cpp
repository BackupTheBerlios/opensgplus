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

#include "OSGClusterWindowAtt.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::ClusterWindowAtt

The base class for all windows.

*/

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char ClusterWindowAtt::cvsid[] = "@(#)$Id: $";

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

/** \brief initialize the static features of the class, e.g. action callbacks
 */

void ClusterWindowAtt::initMethod (void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/


/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 */

ClusterWindowAtt::ClusterWindowAtt(void) :
    Inherited()
{
}

/** \brief Copy Constructor
 */

ClusterWindowAtt::ClusterWindowAtt(const ClusterWindowAtt &source) :
    Inherited(source)
{
}

/** \brief Destructor
 */

ClusterWindowAtt::~ClusterWindowAtt(void)
{
}


/** \brief react to field changes
 */

void ClusterWindowAtt::changed(BitVector, ChangeMode)
{
}

/*------------------------------- dump ----------------------------------*/

/** \brief output the instance for debug purposes
 */

void ClusterWindowAtt::dump(      UInt32     uiIndent, 
                         const BitVector &bvFlags) const
{
	SLOG << "Dump ClusterWindowAtt NI" << endl;
}

    

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

