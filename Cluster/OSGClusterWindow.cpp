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
#include "OSGRemoteAspect.h"
#include "OSGConnectionFactory.h"

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

/*! \defgroup ClusterLib Cluster library

The cluster library includes all objects concerning with clustering
and remote rendering

*/

/*! \class osg::ClusterWindow
Base class for cluster configurations
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

ClusterWindow::ClusterWindow(void) :
    Inherited(),
    _connection(NULL),
    _remoteAspect(NULL)
{
}

//! Copy Constructor

ClusterWindow::ClusterWindow(const ClusterWindow &source) :
    Inherited(source),
    _connection(NULL),
    _remoteAspect(NULL)
{
}

//! Destructor

ClusterWindow::~ClusterWindow(void)
{
    if(_connection)
    {
        delete _connection;
    }
    if(_remoteAspect)
    {
        delete _remoteAspect;
    }
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
    MFString::iterator s;

    if(_connection)
    {
        SWARNING << "init called twice" << endl;
        return;
    }
    // create connection
    if(getConnectionType().empty())
    {
        setConnectionType("StreamSock");
    }
    _connection = ConnectionFactory::the().create(getConnectionType());
    if(_connection == NULL)
    {
        SFATAL << "Unknown connection type " << getConnectionType() << endl;
    }
    // create remote aspect
    _remoteAspect = new RemoteAspect();
    // connect to all servers
    for(s =getServers().begin();
        s!=getServers().end();
        s++)
    {
        DgramSocket      serviceSock;
        BinSocketMessage msg;
        string           respServer;
        string           respAddress;
        Bool             retry=true;

        SINFO << "Connect to " << (*s) << endl;
        serviceSock.open();
        while(retry)
        {
            try
            {
                // find server
                do
                {
                    msg.clear();
                    msg.putString(*s);
                    serviceSock.sendTo(msg,BroadcastAddress(getServicePort()));
                }
                while(serviceSock.waitReadable(2)==false);
                serviceSock.recv(msg);
                msg.getString(respServer);
                msg.getString(respAddress);
                if(respServer == *s)
                {
                    SINFO << "Found at address " << respAddress << endl;
                    // connect to server
                    _connection->connect(respAddress);
                    retry=false;
                }
            }
            catch(exception &e)
            {
                SINFO << e.what() << endl;
            }
        }
        serviceSock.close();
    }
    // init client window
    clientInit();
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
    if(_connection && _remoteAspect)
    {
        clientSwap();
    }
}

void ClusterWindow::renderAllViewports( RenderAction *action )
{
    if(_connection && _remoteAspect)
    {
        clientRender(action);
    }
}

void ClusterWindow::frameInit(void)
{
    if(_remoteAspect && _connection)
    {
        clientPreSync();
        _remoteAspect->sendSync(*_connection);
    }
}

void ClusterWindow::frameExit(void)
{
}

/*----------------------------- client methods ----------------------------*/

/*! init client window
 *  
 */

void ClusterWindow::clientInit( void )
{
}

/*! client frame before sync
 *  
 *  default action activate client if client window is given
 */

void ClusterWindow::clientPreSync( void )
{
    if(getClientWindow() != NullFC)
    {
        getClientWindow()->activate();
        getClientWindow()->frameInit();
    }
}

/*! initiate client rendering
 *  
 *  default is to render all viewports with the given action
 */

void ClusterWindow::clientRender( RenderAction *action )
{
    if(getClientWindow() != NullFC)
    {
        getClientWindow()->renderAllViewports( action );
    }
}

/*! swap client window
 *  
 *  default is to swap the given window
 */

void ClusterWindow::clientSwap( void )
{
    if(getClientWindow() != NullFC)
    {
        getClientWindow()->swap( );
        getClientWindow()->frameExit();
    }
}

/*----------------------------- server methods ----------------------------*/

/*! initialise the cluster window on the server side
 *  
 *  !param window     server render window
 *  !param id         server id
 */

void ClusterWindow::serverInit( WindowPtr ,
                                UInt32 )
{
}

/*! render server window
 *  
 *  default action is to render all viewports with the given action
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param action     action
 */

void ClusterWindow::serverRender( WindowPtr window,
                                  UInt32 ,
                                  RenderAction *action )
{
    window->activate();
    window->frameInit();
    window->renderAllViewports( action );
}

/*! swap server window
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param connection connection to client
 */

void ClusterWindow::serverSwap        ( WindowPtr window,
                                        UInt32 )
{
    window->swap();
    window->frameExit();
}

