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
#include <OSGPerspectiveCamera.h>
#include <OSGStereoCamera.h>
#include "OSGSimpClustSterWin.h"
#include "OSGConnection.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: OSGSimpClustSterWin.cpp,v 1.3 2002/01/02 17:34:06 marcus Exp $";
    static char cvsid_hpp[] = OSGSCLUSTSTERWIN_HEADER_CVSID;
    static char cvsid_inl[] = OSGSCLUSTSTERWIN_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::SimpClustSterWin
 Clister window for stereo projection with two rendering servers.
 The first server renders the left eye and the second renders the
 right eye.

 This class could be removed, if there are good Camera Decorators
 so the Stereo-Rendering could be integrated to the multidisplay
 rendering. Then the stereo rendering with two servers is a special
 Multidisplay configuration.

*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

SimpClustSterWin::SimpClustSterWin(void) :
    Inherited()
{
}

//! Copy Constructor

SimpClustSterWin::SimpClustSterWin(const SimpClustSterWin &source) :
    Inherited(source)
{
}

//! Destructor

SimpClustSterWin::~SimpClustSterWin(void)
{
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void SimpClustSterWin::initMethod (void)
{
}

//! react to field changes

void SimpClustSterWin::changed(BitVector, ChangeMode)
{
}

//! output the instance for debug purposes

void SimpClustSterWin::dump(      UInt32    , 
                              const BitVector ) const
{
    SLOG << "fov:" << getFov() << " "
         << "eyedistance:" << getEyedistance() << " " 
         << "zeroparallax:" << getZeroparallax() << endl;
}

/*----------------------------- server methods ----------------------------*/

/*! initialise the server rendering frame
 *  
 *  update stereo camera params
 *  
 *  !param window     server render window
 *  !param id         server id
 *  !param action     action
*/

void SimpClustSterWin::serverRender(WindowPtr window,UInt32 id,
                                    RenderAction *action)
{
    // get client cam
    if(window == NullFC ||
       window->getPort().size()==0)
    {
        SWARNING << "No window or no viewport" << endl;
        return;
    }
    ViewportPtr cvp=getPort()[0];
    PerspectiveCameraPtr clientCam=
        PerspectiveCameraPtr::dcast(cvp->getCamera());
    if(clientCam==NullFC)
    {
        SWARNING << "Client has no camera" << endl;
        return;
    }

    // get server cam
    if(getPort().size()==0)
    {
        SWARNING << "Server has no port" << endl;
        return;
    }
    ViewportPtr svp=window->getPort()[0];
    StereoCameraPtr serverCam=
        StereoCameraPtr::dcast(svp->getCamera());
    if(serverCam==NullFC)
    {
        SWARNING << "Server has no camera" << endl;
        return;
    }

    // update camera params
    beginEditCP(serverCam);
    serverCam->setBeacon(clientCam->getBeacon());
    serverCam->setNear(clientCam->getNear());
    serverCam->setFar(clientCam->getFar());
    serverCam->setFov(clientCam->getFov());
    serverCam->setEyedistance(getEyedistance());
    serverCam->setZeroparallax(getZeroparallax());
    serverCam->setWhicheye(id);
    endEditCP(serverCam);

    beginEditCP(svp);
    svp->setSize      ( 0,0, 1,1 );
    endEditCP(svp);

    // server rendering
    Inherited::serverRender(window,id,action);

}

/*! initialize server window
 *
 * Create the server window. The fields of the client camera are copied
 * to a new stereo server camera. 
 */

void SimpClustSterWin::serverInit( WindowPtr window,
                                   UInt32 id )
{
    ViewportPtr            svp,cvp;

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
    if(id >= (getServers().size()) || 
       id > 1)
    {
        SWARNING << "Not enough or more then two servers defined " << endl;
        return;
    }

    // get cluster viewport. Currently there is only one viewport
    // fore each window allowed!
    cvp=getPort()[0];

    // create camera 
    // until there is no working decorator mecanism, only perspective
    // cameras are alowed
    PerspectiveCameraPtr clientCam=
        PerspectiveCameraPtr::dcast(cvp->getCamera());
    if(clientCam==NullFC)
    {
        SWARNING << "Only PerspectiveCamera is supported. "
                 << "Ask Dirk for some nice decorators! " 
                 << "Then ask me to integrate them (Marcus)" << endl;
        return;
    }
    StereoCameraPtr cam=StereoCamera::create();
    beginEditCP(cam);
    cam->setBeacon(clientCam->getBeacon());
    cam->setNear(clientCam->getNear());
    cam->setFar(clientCam->getFar());
    cam->setFov(clientCam->getFov());
    cam->setEyedistance(getEyedistance());
    cam->setZeroparallax(getZeroparallax());
    // first server is left, second is right
    cam->setWhicheye(id);
    endEditCP(cam);

    // create new server viewport
    svp = Viewport::create();
    beginEditCP(svp);
    svp->setCamera    ( cam );
    svp->setBackground( cvp->getBackground() );
    svp->setRoot      ( cvp->getRoot() );
    svp->setSize      ( 0,0, 1,1 );
    beginEditCP(window);
    window->addPort(svp);
    endEditCP(window);
    endEditCP(svp);
}

/*! swap server window
 */

void SimpClustSterWin::serverSwap( WindowPtr window,UInt32 id )
{
    if(getSyncSwap())
    {
        _connection->wait();
    }
    Inherited::serverSwap(window,id);
}

/*----------------------------- client methods ----------------------------*/

/*! init client window
 */

void SimpClustSterWin::clientInit( void )
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

void SimpClustSterWin::clientSwap(void)
{
    // sync all servers
    if(getSyncSwap())
    {
        _connection->signal();
    }
    // show client window 
    Inherited::clientSwap();
}












