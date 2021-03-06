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
#include <OSGTileCameraDecorator.h>
#include <OSGViewport.h>
#include <OSGGeometry.h>
#include <OSGStereoBufferViewport.h>
#include <OSGRenderAction.h>
#include "OSGSortFirstWindow.h"
#include "OSGViewBufferHandler.h"
#include "OSGConnection.h"
#include "OSGRenderNode.h"
#include "OSGGeoLoad.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id:$";
    static char cvsid_hpp[] = OSGSORTFIRSTWINDOW_HEADER_CVSID;
    static char cvsid_inl[] = OSGSORTFIRSTWINDOW_INLINE_CVSID;
}

ViewBufferHandler SortFirstWindow::_bufferHandler;

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::SortFirstWindow
Cluster rendering configuration for sort first image composition
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

SortFirstWindow::SortFirstWindow(void) :
    Inherited(),
    _loadManager(NULL),
    _renderNode(NULL)
{
}

//! Copy Constructor

SortFirstWindow::SortFirstWindow(const SortFirstWindow &source) :
    Inherited(source),
    _loadManager(NULL),
    _renderNode(NULL)
{
}

//! Destructor

SortFirstWindow::~SortFirstWindow(void)
{
    if(_loadManager)
        delete _loadManager;
    if(_renderNode)
        delete _renderNode;
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void SortFirstWindow::initMethod (void)
{
}

//! react to field changes

void SortFirstWindow::changed(BitVector, UInt32)
{
}

//! output the instance for debug purposes

void SortFirstWindow::dump(      UInt32    , 
                           const BitVector ) const
{
    SLOG << "Dump SortFirstWindow NI" << endl;
}

/*----------------------------- server methods ----------------------------*/

/** transfer server cababilities to the client
 *
 **/
void SortFirstWindow::serverInit( WindowPtr serverWindow,
                                  UInt32 id)
{
    UInt32 sync;
    RenderNode renderNode;

    // create cluster node information
    // get performance
    renderNode.determinePerformance(serverWindow);
    renderNode.dump();
    // transfer to client for load balancing
    _connection->putUInt32(id);
    renderNode.copyToBin(*_connection);
    _connection->flush();
    _connection->selectChannel();
    _connection->getUInt32(sync);
}

/** update server window
 *
 * \todo enamble frustum culling if error is removed
 **/
void SortFirstWindow::serverRender( WindowPtr serverWindow,
                                    UInt32 id,
                                    RenderAction *action        )
{
    cout << "server render" << endl;
    TileCameraDecoratorPtr deco;
    ViewportPtr serverPort;
    ViewportPtr clientPort;
    UInt32 sv,cv,regionStart;
    UInt32 vpWidth;
    UInt32 vpHeight;

    // error in frustum culling !!!!!!!!!!!!!!
    action->setFrustumCulling(false);

    // duplicate viewports
    for(cv=0,sv=0;cv<getPort().size();cv++)
    {
        clientPort = getPort()[cv];
        if(serverWindow->getPort().size() <= sv)
        {
            // create new port
            serverPort = StereoBufferViewport::create();
            deco=TileCameraDecorator::create();
            beginEditCP(serverWindow);
            serverWindow->addPort(serverPort);
            serverPort->setCamera(deco);
            endEditCP(serverWindow);
        }
        else
        {
            serverPort = serverWindow->getPort()[sv];
            deco=TileCameraDecoratorPtr::dcast(serverPort->getCamera());
        }

        // duplicate values
        beginEditCP(serverPort);
        regionStart=cv * getServers().size() * 4 + id * 4;
        serverPort->setSize( 
            getRegion()[ regionStart+0 ] + clientPort->getPixelLeft(),
            getRegion()[ regionStart+1 ] + clientPort->getPixelBottom(),
            getRegion()[ regionStart+2 ] + clientPort->getPixelLeft(),
            getRegion()[ regionStart+3 ] + clientPort->getPixelBottom());

        serverPort->setRoot      ( clientPort->getRoot()       );
        serverPort->setBackground( clientPort->getBackground() );
        serverPort->getMFForegrounds()->setValues( clientPort->getForegrounds() );
        endEditCP(serverPort);

        // calculate tile parameters
        vpWidth =clientPort->getPixelWidth();
        vpHeight=clientPort->getPixelHeight();
        beginEditCP(deco);
        deco->setFullWidth ( vpWidth );
        deco->setFullHeight( vpHeight );
        deco->setSize( getRegion()[ regionStart+0 ]/(float)vpWidth,
                       getRegion()[ regionStart+1 ]/(float)vpHeight,
                       getRegion()[ regionStart+2 ]/(float)vpWidth,
                       getRegion()[ regionStart+3 ]/(float)vpHeight );
        deco->setDecoratee( clientPort->getCamera() );
        endEditCP(deco);
        sv++;
    }
    // remove unused ports
    while(serverWindow->getPort().size()>sv)
    {
        serverWindow->subPort(sv);
    }
    // compression type
    if(getCompose())
    {
        if(getCompression().empty())
        {
            _bufferHandler.setImgTransType(NULL);
        }
        else
        {
            _bufferHandler.setImgTransType(getCompression().c_str());
        }
        if(getSubtileSize())
        {
            _bufferHandler.setSubtileSize(getSubtileSize());
        }
    }
#if 1
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
#endif

    // render the viewports
    serverWindow->activate();
    serverWindow->frameInit();
    action->setWindow( serverWindow.getCPtr() );
    for(sv=0;sv<serverWindow->getPort().size();++sv)
    {
        ViewportPtr vp=serverWindow->getPort()[sv];
        vp->render( action );
        // send resulting image
        if(getCompose())
        {
            // send image
            _bufferHandler.send(
                *_connection,
                ViewBufferHandler::RGB,
                vp->getPixelLeft(),
                vp->getPixelBottom(),
                vp->getPixelRight(),
                vp->getPixelTop(),
                0,0);
        }
    }
}

/*! send image to client
 */
void SortFirstWindow::serverSwap( WindowPtr window,
                                  UInt32 )
{
    window->swap();
    if(!getCompose())
    {
        _connection->wait();
    }
}

/*----------------------------- client methods ----------------------------*/

/*! read server cababilities
 */

void SortFirstWindow::clientInit( void )
{
    UInt32 id;
    RenderNode renderNode;

    _loadManager=new GeoLoadManager(getUseFaceDistribution());
    // read all node infos
    for(UInt32 i=0;i<_connection->getChannelCount();++i)
    {
        cout << "read cluster node info" << endl;
        _connection->selectChannel();
        _connection->getUInt32(id);
        renderNode.copyFromBin(*_connection);
        cout << id << endl;
        renderNode.dump();
        _loadManager->addRenderNode(renderNode,id);    
    }
    cout << "sync" << endl;
    // sync servers
    _connection->putUInt32(0);
    _connection->flush();
    cout << "end" << endl;
}

/*! client frame init
 */

void SortFirstWindow::clientPreSync( void )
{
    SortFirstWindowPtr ptr=SortFirstWindowPtr(this);
    if(getCompose())
    {
        // get window size from client window
        if(getClientWindow() != NullFC)
        {
            if(getWidth()  != getClientWindow()->getWidth() ||
               getHeight() != getClientWindow()->getHeight())
            {
                beginEditCP(ptr,
                            Window::WidthFieldMask|
                            Window::HeightFieldMask);
                {
                    setSize(getClientWindow()->getWidth(),
                            getClientWindow()->getHeight());
                }
                endEditCP(ptr,
                          Window::WidthFieldMask|
                          Window::HeightFieldMask);
            }
        }
    }
#if 0
    else
    {
        beginEditCP(ptr,
                    Window::WidthFieldMask|
                    Window::HeightFieldMask);
        {
            setSize(1280,
                    1024);
        }
        endEditCP(ptr,
                  Window::WidthFieldMask|
                  Window::HeightFieldMask);
    }
#endif
    UInt32 i;
    UInt32 cv;
    GeoLoadManager::ResultT region;
    
    beginEditCP(ptr,SortFirstWindow::RegionFieldMask);
    getRegion().clear();
    for(cv=0;cv<getPort().size();cv++)
    {
        _loadManager->update( getPort()[cv]->getRoot() );
        _loadManager->balance(getPort()[cv],
                              false,
                              region);
        for(i=0;i<getServers().size();i++)
        {
            getRegion().push_back(region[4*i+0]);
            getRegion().push_back(region[4*i+1]);
            getRegion().push_back(region[4*i+2]);
            getRegion().push_back(region[4*i+3]);
        }
    }

    endEditCP(ptr,SortFirstWindow::RegionFieldMask);
}

/*! client rendering
 *  
 *  one tile is rendered by the client
 */

void SortFirstWindow::clientRender( RenderAction *  /* action */ )
{
//    Inherited::clientRender(action);
}

/*! show data
 */

void SortFirstWindow::clientSwap( void )
{
    UInt32 cv;
    if(getCompose())
    {
        if(getClientWindow()!=NullFC)
        {
            glDisable(GL_SCISSOR_TEST);
            glViewport(0,0,
                       getClientWindow()->getWidth(),
                       getClientWindow()->getHeight());
            // receive all viewports
            for(cv=0;cv<getPort().size();++cv)
            {
                _bufferHandler.recv(*_connection);
            }
            Inherited::clientSwap();
        }
    }
    else
    {
        _connection->signal();
    }
}





