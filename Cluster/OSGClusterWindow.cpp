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

#define OSG_COMPILECLUSTERLIB

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>
#include <OSGClusterDef.h>
#include "OSGClusterWindow.h"
#include "OSGDgramSocket.h"
#include "OSGStreamSocket.h"
#include "OSGRemoteAspect.h"
#include "OSGConnection.h"
#include "OSGMulticastConnection.h"
#include "OSGStreamSockConnection.h"
#include "OSGBinSocketMessage.h"
#include "OSGRemoteAspect.h"
#include "OSGClusterClient.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: $";
    static char cvsid_hpp[] = OSGCLUSTERWINDOW_HEADER_CVSID;
    static char cvsid_inl[] = OSGCLUSTERWINDOW_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::ClusterWindow
Base class for cluster configurations
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

ClusterWindow::ClusterWindow(void) :
    Inherited()
{
}

//! Copy Constructor

ClusterWindow::ClusterWindow(const ClusterWindow &source) :
    Inherited(source)
{
}

//! Destructor

ClusterWindow::~ClusterWindow(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void ClusterWindow::initMethod (void)
{
}

//! react to field changes

void ClusterWindow::changed(BitVector, ChangeMode)
{
}

//! output the instance for debug purposes

void ClusterWindow::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump ClusterWindow NI" << endl;
}

void (*ClusterWindow::getFunctionByName ( const Char8 * ))()
{
    return NULL;
}

/*! init cluster window. connect to all servers
 */

void ClusterWindow::init( void )
{
}

void ClusterWindow::render( RenderAction *action )
{
    activate();
    frameInit();
    renderAllViewports( action );
    swap();
    frameExit();
}

void ClusterWindow::activate( void )
{
}

void ClusterWindow::deactivate( void )
{
}

void ClusterWindow::swap( void )
{
    ClusterWindowPtr ptr(*this);
    ClusterClient *client = ClusterClient::find(ptr);
    if(client)
    {
        clientSwap(client->getClientWindow(),
                   client->getConnection());
    }
}

void ClusterWindow::renderAllViewports( RenderAction *action )
{
    ClusterWindowPtr ptr(*this);
    ClusterClient *client = ClusterClient::find(ptr);
    if(client)
    {
        clientRenderAllViewports(client->getClientWindow(),
                                 client->getConnection(),
                                 action);
    }
}

void ClusterWindow::frameInit(void)
{
    ClusterWindowPtr ptr(*this);
    ClusterClient *client = ClusterClient::find(ptr);
    if(client)
    {
        clientFrameInit(client->getClientWindow(),
                        client->getConnection(),
                        client->getRemoteAspect());
    }
}

void ClusterWindow::frameExit(void)
{
    ClusterWindowPtr ptr(*this);
    ClusterClient *client = ClusterClient::find(ptr);
    if(client)
    {
        clientFrameExit(client->getClientWindow(),
                        client->getConnection());
    }
}

/*----------------------------- client methods ----------------------------*/

/*! init client window
 *  
 */

void ClusterWindow::clientInit        ( WindowPtr ,
                                        Connection * )
{
}

/*! init client window at begin of frame
 *  
 *  This default function sends the modifications to all servers,
 *  activates the client window and init frame of client window
 */

void ClusterWindow::clientFrameInit   ( WindowPtr window,
                                        Connection *connection,
                                        RemoteAspect *aspect               )
{
    aspect->sendSync(*connection);
    Thread::getCurrentChangeList()->clearAll();
    if(window!=NullFC)
    {
        window->activate();
        window->frameInit();
    }
}

/*! initiate client rendering
 *  
 *  default is to render all viewports with the given action
 */

void ClusterWindow::clientRenderAllViewports( WindowPtr window,
                                              Connection *,
                                              RenderAction *action )
{
    if(window!=NullFC)
    {
        window->renderAllViewports( action );
    }
}

/*! swap client window
 *  
 *  default is to swap the given window
 */

void ClusterWindow::clientSwap( WindowPtr window,
                                Connection * )
{
    if(window!=NullFC)
    {
        window->swap();
    }
}

/*! end of client rendering
 *  
 *  frame exit for client window
 */

void ClusterWindow::clientFrameExit( WindowPtr window,
                                     Connection * )
{
    if(window!=NullFC)
    {
        window->frameExit();
    }
}

/*----------------------------- server methods ----------------------------*/

/*! initialise the cluster window on the server side
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param connection connection to client
 */

void ClusterWindow::serverInit( WindowPtr ,
                                UInt32 ,
                                Connection *)
{
}

/*! initialise the server rendering frame
 *  
 *  default action is to receive sync and activate server window
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param connection connection to client
 *  !param aspect     remote aspect
 */

void ClusterWindow::serverFrameInit   ( WindowPtr window,
                                        UInt32 ,
                                        Connection *connection,
                                        RemoteAspect *aspect )
{
    // recive sync
    aspect->receiveSync(*connection);
    window->activate();
    window->frameInit();
}

/*! render server window
 *  
 *  default action is to render all viewports with the given action
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param connection connection to client
 *  !param action     action
 */

void ClusterWindow::serverRenderAllViewports( WindowPtr window,
                                              UInt32 ,
                                              Connection *,
                                              RenderAction *action )
{
    window->renderAllViewports( action );
}

/*! swap server window
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param connection connection to client
 */

void ClusterWindow::serverSwap        ( WindowPtr window,
                                        UInt32 ,
                                        Connection * )
{
    window->swap();
}

/*! end of server frame rendering actions
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param connection connection to client
 */

void ClusterWindow::serverFrameExit   ( WindowPtr window,
                                        UInt32 ,
                                        Connection *)
{
    window->frameExit();
}




