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
#include <GL/gl.h>
#include <OSGConfig.h>
#include <OSGTileCameraDecorator.h>
#include <OSGViewport.h>
#include "OSGSortFirstWindow.h"
#include "OSGViewBufferHandler.h"
#include "OSGConnection.h"

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
    Inherited()
{
}

//! Copy Constructor

SortFirstWindow::SortFirstWindow(const SortFirstWindow &source) :
    Inherited(source)
{
}

//! Destructor

SortFirstWindow::~SortFirstWindow(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void SortFirstWindow::initMethod (void)
{
}

//! react to field changes

void SortFirstWindow::changed(BitVector, ChangeMode)
{
}

//! output the instance for debug purposes

void SortFirstWindow::dump(      UInt32    , 
                           const BitVector ) const
{
    SLOG << "Dump SortFirstWindow NI" << endl;
}

/*----------------------------- server methods ----------------------------*/

/*! initialize server window
 */

void SortFirstWindow::serverInit( WindowPtr window,
                                  UInt32 ,
                                  Connection *)
{
    ViewportPtr            svp,cvp;
    TileCameraDecoratorPtr deco;
    GLint                  glvp[4];

    // do some checks 
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

    // get server window size. The server window must be the active 
    // window 
    glGetIntegerv( GL_VIEWPORT, glvp );
    window->setSize( glvp[2], glvp[3] );

    // get cluster viewport. Currently there is only one viewport
    // fore each window allowed!
    cvp=getPort()[0];

    // create camera decorator
    deco = TileCameraDecorator::create();
    beginEditCP(deco);
    deco->setCamera( cvp->getCamera() );
    deco->setSize( 0,0,1,1 );
    deco->setFullWidth ( getWidth() );
    deco->setFullHeight( getHeight() );
    endEditCP(deco);

    // create new server viewport
    svp = Viewport::create();
    beginEditCP(svp);
    svp->setCamera    ( deco );
    svp->setBackground( cvp->getBackground() );
    svp->setRoot      ( cvp->getRoot() );
    svp->setSize      ( 0,
                        0, 
                        1.0 * getWidth () / window->getWidth (),
                        1.0 * getHeight() / window->getHeight());
    beginEditCP(window);
    window->addPort(svp);
    endEditCP(window);
    endEditCP(svp);
}

/*! update server window
 */

void SortFirstWindow::serverFrameInit( WindowPtr window,UInt32 id,
                                       Connection *connection,
                                       RemoteAspect *aspect )
{
    // do data sync
    Inherited::serverFrameInit(window,id,connection,aspect);

    ViewportPtr vp=window->getPort()[0];
    TileCameraDecoratorPtr deco=TileCameraDecoratorPtr::dcast(vp->getCamera());

    if(id >= getTop()   .size() ||
       id >= getBottom().size() ||
       id >= getLeft()  .size() ||
       id >= getRight() .size())
    {
        vp->setSize( 0,0,1,1 );
        deco->setSize( 0,0,1,1 );
        SWARNING << "top, bottom, left or right missing for server"
                 << id << endl;
        return;
    }

    beginEditCP(deco);
    // modify viewport size instead of window size
    vp->setSize( 0,
                 0, 
                 ((Int32)(getRight ()[id] * getWidth ())) -
                 ((Int32)(getLeft  ()[id] * getWidth ())),
                 ((Int32)(getTop   ()[id] * getHeight())) -
                 ((Int32)(getBottom()[id] * getHeight())));
    deco->setSize( getLeft()[id],
                   getBottom()[id],
                   getRight()[id],
                   getTop()[id] );
    deco->setFullWidth ( getWidth() );
    deco->setFullHeight( getHeight() );
    endEditCP(deco);

    // compression type
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

/*! send image to client
 */

void SortFirstWindow::serverSwap( WindowPtr window,
                                  UInt32 ,
                                  Connection *connection )
{
    ViewportPtr vp=window->getPort()[0];
    TileCameraDecoratorPtr deco=TileCameraDecoratorPtr::dcast(vp->getCamera());
    
    // send image
    _bufferHandler.send(
        *connection,
        ViewBufferHandler::RGB,
        deco->getLeft()   * deco->getFullWidth(),
        deco->getBottom() * deco->getFullHeight());

    // test only
//    window->swap();
}

/*----------------------------- client methods ----------------------------*/

/*! init client window
 */

void SortFirstWindow::clientInit( WindowPtr window,
                                  Connection * )
{
    ViewportPtr cvp;
    if(window == NullFC ||
       getPort().size()==0)
        return;
    cvp=getPort()[0];

    // duplucate viewport
    ViewportPtr vp;
    vp = Viewport::create();
    beginEditCP(vp);
    vp->setCamera( cvp->getCamera() );
    vp->setBackground( cvp->getBackground() );
    vp->setRoot( cvp->getRoot() );
    vp->setSize( 0,0, 1,1 );
    beginEditCP(window);
    window->addPort(vp);
    endEditCP(window);
    endEditCP(vp);
}

/*! client frame init
 */

void SortFirstWindow::clientFrameInit( WindowPtr window,
                                       Connection *connection,
                                       RemoteAspect *aspect )
{
    SortFirstWindowPtr ptr=SortFirstWindowPtr(this);
    if(window == NullFC)
    {
        SFATAL << "No client window given" << endl;
        return;
    }

    if(getWidth()  != window->getWidth() ||
       getHeight() != window->getHeight())
    {
        beginEditCP(ptr,
                    Window::WidthFieldMask|
                    Window::HeightFieldMask);
        {
            setSize(window->getWidth(),window->getHeight());
        }
        endEditCP(ptr,
                  Window::WidthFieldMask|
                  Window::HeightFieldMask);
    }

    // distribute work
    // replace with load balancing algorithm !!!!!
    int i;
    beginEditCP(ptr,
                SortFirstWindow::LeftFieldMask|
                SortFirstWindow::RightFieldMask|
                SortFirstWindow::TopFieldMask|
                SortFirstWindow::BottomFieldMask);
    getLeft()  .resize(getServers().size());
    getRight() .resize(getServers().size());
    getTop()   .resize(getServers().size());
    getBottom().resize(getServers().size());
    for(i=0;i<getServers().size();i++)
    {
        getTop()   [i]=1;
        getBottom()[i]=0;
        getLeft()  [i]=((double)i)    /getServers().size();
        getRight() [i]=((double)(i+1))/getServers().size();
    }
    endEditCP(ptr,
              SortFirstWindow::LeftFieldMask|
              SortFirstWindow::RightFieldMask|
              SortFirstWindow::TopFieldMask|
              SortFirstWindow::BottomFieldMask);

    Inherited::clientFrameInit( window,connection,aspect );
}

/*! render client window
 *
 *  No rendering on client side
 *
 */

void SortFirstWindow::clientRenderAllViewports( WindowPtr ,
                                                Connection *,
                                                RenderAction * )
{
}

/*! show data
 */

void SortFirstWindow::clientSwap( WindowPtr window,
                                  Connection *connection )
{
    _bufferHandler.recv(*connection);
    Inherited::clientSwap(window,connection);
}





