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

#ifndef _CLUSTERCLIENT_H_
#define _CLUSTERCLIENT_H_
#ifdef __sgi
#pragma once
#endif

#include <map>
#include <OSGBaseTypes.h>
#include "OSGClusterDef.h"
#include "OSGClusterWindow.h"
#include "OSGRemoteAspect.h"

OSG_BEGIN_NAMESPACE

class RenderAction;

//! Brief
//! \ingroup baselib

class OSG_CLUSTERLIB_DLLMAPPING ClusterClient
{
    /*==========================  PUBLIC  =================================*/
  public:

    typedef std::map<UInt32,ClusterClient*> ClusterClientMap;

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    ClusterClient(ClusterWindowPtr clusterWindow,
                  WindowPtr        clientWindow=NullFC,
                  const string &connectionType="StreamSock",
                  UInt32 servicePort=8437);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~ClusterClient(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   your_category                              */
    /*! \{                                                                 */

    void start     (void);
    void stop      (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   get                                        */
    /*! \{                                                                 */

    Connection   *getConnection  (void);
    WindowPtr     getClientWindow(void);
    RemoteAspect *getRemoteAspect(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name               static functions                               */
    /*! \{                                                                 */

    static ClusterClient * find(ClusterWindowPtr window);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                   your_category                              */
    /*! \{                                                                 */

    void connect         ( const string &server );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    ClusterWindowPtr _clusterWindow;
    WindowPtr        _clientWindow;
    UInt32           _servicePort;
    Connection      *_connection;
    RemoteAspect     _remoteAspect;

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    /*---------------------------------------------------------------------*/
    /*! \name               class variables                                */
    /*! \{                                                                 */

    static ClusterClientMap        _client;

    /*! \}                                                                 */

    /*!\brief prohibit default function (move to 'public' if needed) */
    ClusterClient(const ClusterClient &source);
    /*!\brief prohibit default function (move to 'public' if needed) */
    void operator =(const ClusterClient &source);
};

OSG_END_NAMESPACE

#define OSG_CLUSTERCLIENTHEADER_CVSID "@(#)$Id:$"

#endif /* _CLUSTERCLIENT_H_ */
