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

#include "OSGConfig.h"

#include "OSGClusterClient.h"
#include "OSGDgramSocket.h"
#include "OSGStreamSocket.h"
#include "OSGRemoteAspect.h"
#include "OSGBinSocketMessage.h"
#include "OSGConnection.h"
#include "OSGConnectionFactory.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id:$";
    static Char8 cvsid_hpp[] = OSG_CLUSTERCLIENTHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::ClusterClient
    ClusterClient documentation,
 */

ClusterClient::ClusterClientMap ClusterClient::_client;

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor documentation
 */
ClusterClient::ClusterClient(ClusterWindowPtr clusterWindow,
                             WindowPtr        clientWindow,
                             const string    &connectionType,
                             UInt32           servicePort):
    _clusterWindow(clusterWindow),
    _clientWindow(clientWindow),
    _servicePort(servicePort),
    _connection(NULL)
{
    // create connection
    _connection = ConnectionFactory::the().create(connectionType);
    if(_connection == NULL)
    {
        SFATAL << "Unknown connection type " << connectionType << endl;
    }

    // insert into client map
    _client[_clusterWindow.getFieldContainerId()]=this;
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
 */

ClusterClient::~ClusterClient(void)
{
    ClusterClientMap::iterator i;

    if(_connection)
        delete _connection;
    // remove from client map

    i=_client.find(_clusterWindow.getFieldContainerId());
    if(i!=_client.end())
        _client.erase(i);
}

/*-------------------------------------------------------------------------*/
/*                             class specific                              */

/*! start client
 */

void ClusterClient::start( void )
{
    MFString::iterator i;

    // connect to all servers
    for(i =_clusterWindow->getServers().begin();
        i!=_clusterWindow->getServers().end();
        i++)
    {
        connect(*i);
    }

    // send first sync
    _remoteAspect.sendSync(*_connection,
                           OSG::Thread::getCurrentChangeList());
    Thread::getCurrentChangeList()->clearAll();

    // init client window
    _clusterWindow->clientInit(_clientWindow,_connection);

}

/*! connect to server
 */

void ClusterClient::connect(const string &server)
{
    DgramSocket      serviceSock;
    BinSocketMessage msg;
    string           respServer;
    string           respAddress;
    Bool             retry=true;

    SINFO << "Connect to " << server << endl;
    serviceSock.open();
    while(retry)
    {
        try
        {
            // find server
            do
            {
                msg.clear();
                msg.putString(server);
                serviceSock.sendTo(msg,BroadcastAddress(_servicePort));
            }
            while(serviceSock.waitReadable(2)==false);
            serviceSock.recv(msg);
            msg.getString(respServer);
            msg.getString(respAddress);
            if(respServer == server)
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

/*! start client
 */

Connection *ClusterClient::getConnection(void)
{
    return _connection;
}

WindowPtr ClusterClient::getClientWindow(void)
{
    return _clientWindow;
}

RemoteAspect *ClusterClient::getRemoteAspect(void)
{
    return &_remoteAspect;
}

ClusterClient *ClusterClient::find(ClusterWindowPtr window)
{
    return _client[window.getFieldContainerId()];
}
