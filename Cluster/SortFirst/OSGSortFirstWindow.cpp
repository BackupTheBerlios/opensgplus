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
#include "OSGSortFirstWindow.h"
#include "OSGViewBufferHandler.h"
#include "OSGConnection.h"
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
                                  UInt32 )
{
    ViewportPtr            svp,cvp;
    TileCameraDecoratorPtr deco;

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

void SortFirstWindow::serverRender( WindowPtr window,UInt32 id,
                                    RenderAction *action        )
{
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
    Inherited::serverRender(window,id,action);
}

/*! send image to client
 */

void SortFirstWindow::serverSwap( WindowPtr window,
                                  UInt32 )
{
    ViewportPtr vp=window->getPort()[0];
    TileCameraDecoratorPtr deco=TileCameraDecoratorPtr::dcast(vp->getCamera());
    
    // send image
    _bufferHandler.send(
        *_connection,
        ViewBufferHandler::RGB,
        deco->getLeft()   * deco->getFullWidth(),
        deco->getBottom() * deco->getFullHeight());

    // test only
    //   window->swap();
}

/*----------------------------- client methods ----------------------------*/

/*! init client window
 */

void SortFirstWindow::clientInit( void )
{
    if(getClientWindow() == NullFC ||
       getPort().size()==0)
        return;

    // get cluster viewport. Currently there is only one viewport
    // fore each window allowed!
    ViewportPtr cvp=getPort()[0];

    // create camera decorator
    TileCameraDecoratorPtr deco = TileCameraDecorator::create();
    beginEditCP(deco);
    deco->setCamera( cvp->getCamera() );
    deco->setSize( 0,0,1,1 );
    deco->setFullWidth ( getWidth() );
    deco->setFullHeight( getHeight() );
    endEditCP(deco);

    // create new client viewport
    ViewportPtr vp = Viewport::create();
    beginEditCP(vp);
    vp->setCamera    ( deco );
    vp->setBackground( cvp->getBackground() );
    vp->setRoot      ( cvp->getRoot() );
    vp->setSize      ( 0,
                       0, 
                       1.0 * getWidth () / getClientWindow()->getWidth (),
                       1.0 * getHeight() / getClientWindow()->getHeight());
    beginEditCP(getClientWindow());
    getClientWindow()->addPort(vp);
    endEditCP(getClientWindow());
    endEditCP(vp);
}

/*! client frame init
 */

void SortFirstWindow::clientPreSync( void )
{
    SortFirstWindowPtr ptr=SortFirstWindowPtr(this);
    if(getClientWindow() == NullFC)
    {
        SFATAL << "No client window given" << endl;
        return;
    }
    
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

    // distribute work
    // replace with load balancing algorithm !!!!!
    int i;
    int num=getServers().size()+1;
    beginEditCP(ptr,
                SortFirstWindow::LeftFieldMask|
                SortFirstWindow::RightFieldMask|
                SortFirstWindow::TopFieldMask|
                SortFirstWindow::BottomFieldMask);
    getLeft()  .resize(num);
    getRight() .resize(num);
    getTop()   .resize(num);
    getBottom().resize(num);
    for(i=0;i<num;i++)
    {
        getTop()   [i]=1;
        getBottom()[i]=0;
        getLeft()  [i]=((double)i)    /num;
        getRight() [i]=((double)(i+1))/num;
    }
    endEditCP(ptr,
              SortFirstWindow::LeftFieldMask|
              SortFirstWindow::RightFieldMask|
              SortFirstWindow::TopFieldMask|
              SortFirstWindow::BottomFieldMask);

    // client viewport
    ViewportPtr vp=getClientWindow()->getPort()[0];
    TileCameraDecoratorPtr deco=TileCameraDecoratorPtr::dcast(vp->getCamera());
    vp->setSize( ((Int32)(getLeft  ()[num-1] * getWidth ())),
                 ((Int32)(getBottom()[num-1] * getHeight())), 
                 ((Int32)(getRight ()[num-1] * getWidth ())),
                 ((Int32)(getTop   ()[num-1] * getHeight())));
    deco->setSize( getLeft()  [num-1],
                   getBottom()[num-1],
                   getRight() [num-1],
                   getTop()   [num-1] );
    deco->setFullWidth ( getWidth() );
    deco->setFullHeight( getHeight() );

    distributeWork();
}

/*! client rendering
 *  
 *  one tile is rendered by the client
 */

void SortFirstWindow::clientRender( RenderAction *action )
{
    Inherited::clientRender(action);
}

/*! show data
 */

void SortFirstWindow::clientSwap( void )
{
    if(getClientWindow()!=NullFC)
    {
        glViewport(0,0,
                   getClientWindow()->getWidth(),
                   getClientWindow()->getHeight());
        glScissor(0,0,
                  getClientWindow()->getWidth(),
                  getClientWindow()->getHeight());
        _bufferHandler.recv(*_connection);
        Inherited::clientSwap();
    }
    else
    {
        SFATAL << "Client window missing" << endl;
    }
}

void SortFirstWindow::distributeWork()
{
    NodePtr   root  =getPort()[0]->getRoot();
    traverseGeometry(root,getPort()[0]);
}

void SortFirstWindow::traverseGeometry(NodePtr np,ViewportPtr port)
{
    MFNodePtr::iterator nodei;
    NodeCorePtr core;
    GeometryPtr geom;
    Vec3f min,max;

    core=np->getCore();
    if(core!=NullFC)
    {
        geom=GeometryPtr::dcast(core);
        if(geom!=NullFC)
        {
            GeoLoad load(np);
            load.updateView(port);
            load.dump();
        }
    }
    for(nodei =np->getMFChildren()->begin();
        nodei!=np->getMFChildren()->end();
        nodei++)
    {
        traverseGeometry(*nodei,port);
    }
}

