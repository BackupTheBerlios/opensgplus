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
#include "OSGThread.h"
#include "OSGThreadManager.h"
#include "OSGClusterServer.h"
#include "OSGConnectionFactory.h"
#include "OSGDgramSocket.h"
#include "OSGClusterWindow.h"
#include "OSGBinSocketMessage.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id:$";
    static Char8 cvsid_hpp[] = OSG_CLUSTERSERVERHEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::ClusterServer
    ClusterServer documentation,
 */

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Constructor documentation
 */
ClusterServer::ClusterServer(WindowPtr window,
                             const string &serviceName,
                             const string &connectionType,
                             const string &address,
                             UInt32 servicePort):
    _window(window),
    _clusterWindow(),
    _serviceName(serviceName),
    _servicePort(servicePort),
    _serverId(0),
    _address(address),
    _serviceThread(NULL),
    _needUpdate(false)
{
    _connection = ConnectionFactory::the().create(connectionType);
    if(_connection == NULL)
    {
        SFATAL << "Unknown connection type " << connectionType << endl;
    }
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
 */
ClusterServer::~ClusterServer(void)
{
    if(_serviceThread)
    {
        DgramSocket      s;
        BinSocketMessage msg;
        msg.clear();
        msg.putString("stop");
        msg.putString(_serviceName);
        s.open();
        s.sendTo(msg,BroadcastAddress(_servicePort));
        Thread::join(_serviceThread);
        s.close();
    }
    if(_connection)
        delete _connection;
}

/*-------------------------------------------------------------------------*/
/*                             Class specific                              */


/*! start server
 */

void ClusterServer::init()
{
    OSG::FieldContainerType *fct;

    // bind connection
    _address = _connection->bind(_address);

    // start service proc
    _serviceThread=dynamic_cast<Thread*>(ThreadManager::the()->getThread(NULL));
    _serviceThread->run( serviceProc, 0, (void *) (this) );

    // register interrest for all changed cluster windows
    for(UInt32 i=0;i<OSG::TypeFactory::the()->getNumTypes();++i)
    {
        fct=OSG::FieldContainerFactory::the()->findType(i);
        if(fct && fct->isDerivedFrom(ClusterWindow::getClassType()))
        {
            _aspect.registerChanged(
                *fct,
                osgMethodFunctor2Ptr(this,
                                     &ClusterServer::configChanged));
        }
    }
    // accept incomming connections
    _connection->accept();
}

/*! render server window
 */

void ClusterServer::render(RenderAction *action)
{
    // do we have a cluster window?
    if(_clusterWindow==NullFC)
    {
        do
        {
            // recive 
            _aspect.receiveSync(*_connection);
        }
        while(_clusterWindow==NullFC);
        // get server id
        for(_serverId=0;
            _clusterWindow->getServers()[_serverId] != _serviceName &&
                _serverId<_clusterWindow->getServers().size();
            _serverId++);
        // server connected and cluster window found
        SINFO << "Start server " << _serviceName 
              << " with id " << _serverId << endl;
        // initialize server window 
        _clusterWindow->_connection = _connection;
        _clusterWindow->serverInit(_window,_serverId);
    }
    else
    {
        // sync with render clinet
        _aspect.receiveSync(*_connection);
    }
    _clusterWindow->serverRender( _window,_serverId,action );
    _clusterWindow->serverSwap  ( _window,_serverId );
}

/*! configuration chagne callback
 */

Bool ClusterServer::configChanged(FieldContainerPtr& fcp,
                                  RemoteAspect *)
{
    MFString::iterator i;
    ClusterWindowPtr config=ClusterWindowPtr::dcast(fcp);

    i=config->getServers().find(_serviceName);
    if(i==config->getServers().end())
    {
        SWARNING << "wrong config" << endl;
        return true;
    }
    _needUpdate=true;
    _clusterWindow=config;
    return true;
}

/*! tell address of server requested over multicast
 */

void *ClusterServer::serviceProc(void *arg)
{
    ClusterServer   *server=static_cast<ClusterServer*>(arg);
    BinSocketMessage msg;
    DgramSocket      serviceSock;
    Address          addr;
    string           service;

    SINFO << "Waiting for request of " << server->_serviceName << endl;
    serviceSock.open();
    serviceSock.setReusePort(true);
    serviceSock.bind(AnyAddress(server->_servicePort));
    for(;;)
    {        
        try
        {
            serviceSock.recvFrom(msg,addr);
            service=msg.getString();
            SINFO << "Request for " << service << endl;
            if(service=="stop")
            {
                service=msg.getString();
                if(service == server->_serviceName)
                    return NULL;
            }
            if(service == server->_serviceName)
            {
                msg.clear();
                msg.putString(service);
                msg.putString(server->_address);
                serviceSock.sendTo(msg,addr);
                SINFO << "Response " << server->_address << endl;
            }
        }
        catch(exception &e)
        {
            SLOG << e.what() << endl;
        }
    }
}










