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
#include <GL/gl.h>
#include <OSGViewport.h>
#include <OSGTileCameraDecorator.h>
#include "OSGMultiDisplayWindow.h"
#include "OSGConnection.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: $";
    static char cvsid_hpp[] = OSGMULTIDISPLAYCONFIG_HEADER_CVSID;
    static char cvsid_inl[] = OSGMULTIDISPLAYCONFIG_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::MultiDisplayWindow
Cluster rendering configuration for multible display
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

MultiDisplayWindow::MultiDisplayWindow(void) :
    Inherited()
{
}

//! Copy Constructor

MultiDisplayWindow::MultiDisplayWindow(const MultiDisplayWindow &source) :
    Inherited(source)
{
}

//! Destructor

MultiDisplayWindow::~MultiDisplayWindow(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void MultiDisplayWindow::initMethod (void)
{
}

//! react to field changes

void MultiDisplayWindow::changed(BitVector, ChangeMode)
{
}

//! output the instance for debug purposes

void MultiDisplayWindow::dump(      UInt32    , 
                              const BitVector ) const
{
    SLOG << "hServers:" << getHServers() << " "
         << "vServers:" << getVServers() << endl;
}

/*----------------------------- server methods ----------------------------*/

/*! initialize server window
 */

void MultiDisplayWindow::serverInit( WindowPtr window,
                                     UInt32 id )
{
    ViewportPtr            svp,cvp;
    TileCameraDecoratorPtr deco;
    UInt32                 row,column;

    if(window == NullFC)
    {
        SWARNING << "No Server window" << endl;
        return;
    }
    if(getPort().size()==0)
    {
        SWARNING << "Cluster window has no viewport" << endl;
        return;
    }
    if(id >= (getHServers()*getVServers()))
    {
        SWARNING << "Server outside of the server grid " 
                 << getHServers() << " x " 
                 << getVServers()
                 << endl;
        return;
    }

    // position of this server
    row   =id/getHServers();
    column=id%getHServers();
    
    // get cluster viewport. Currently there is only one viewport
    // fore each window allowed!
    cvp=getPort()[0];

    // create camera decorator
    deco = TileCameraDecorator::create();
    beginEditCP(deco);
    deco->setCamera( cvp->getCamera() );
    deco->setSize( 1.0/getHServers() * column,
                   1.0/getVServers() * row,
                   1.0/getHServers() * (column+1),
                   1.0/getVServers() * (row   +1) );
    deco->setFullWidth ( window->getWidth() * getHServers() );
    deco->setFullHeight( window->getHeight() * getVServers() );
    endEditCP(deco);

    // create new server viewport
    svp = Viewport::create();
    beginEditCP(svp);
    svp->setCamera    ( deco );
    svp->setBackground( cvp->getBackground() );
    svp->setRoot      ( cvp->getRoot() );
    svp->setSize      ( 0,0, 1,1 );
    beginEditCP(window);
    window->addPort(svp);
    endEditCP(window);
    endEditCP(svp);
}

/*! render server window
 *  
 *  default action is to render all viewports with the given action
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param action     action
 */

void MultiDisplayWindow::serverRender( WindowPtr window,
                                       UInt32 id,
                                       RenderAction *action )
{
    ViewportPtr            svp;
    TileCameraDecoratorPtr deco;
    
    // adjust window size
    if(getPort().size()>0)
    {
        ViewportPtr vp=window->getPort()[0];
        TileCameraDecoratorPtr deco=
            TileCameraDecoratorPtr::dcast(vp->getCamera());
        if(deco != NullFC)
        {
            beginEditCP(deco);
            deco->setFullWidth ( window->getWidth() * getHServers() );
            deco->setFullHeight( window->getHeight() * getVServers() );
            endEditCP(deco);
        }
    }
    Inherited::serverRender(window,id,action);
}

/*! swap server window
 */

void MultiDisplayWindow::serverSwap( WindowPtr window,UInt32 id )
{
    _connection->wait();
    Inherited::serverSwap(window,id);
}

/*----------------------------- client methods ----------------------------*/

/*! init client window
 */

void MultiDisplayWindow::clientInit( void )
{
    ViewportPtr cvp;
    if(getClientWindow() == NullFC ||
       getPort().size()==0)
        return;
    cvp=getPort()[0];

    // duplucate viewport for client window
    ViewportPtr vp;
    vp = Viewport::create();
    beginEditCP(vp);
    vp->setCamera( cvp->getCamera() );
    vp->setBackground( cvp->getBackground() );
    vp->setRoot( cvp->getRoot() );
    vp->setSize( 0,0, 1,1 );
    beginEditCP(getClientWindow());
    getClientWindow()->addPort(vp);
    endEditCP(getClientWindow());
    endEditCP(vp);
}

/*! render client window
 */

void MultiDisplayWindow::clientSwap( void )
{
    // sync all servers
    _connection->signal();
    // show client window 
    Inherited::clientSwap();
}











