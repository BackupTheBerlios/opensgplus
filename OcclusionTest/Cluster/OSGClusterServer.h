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

#ifndef _CLUSTERSERVER_H_
#define _CLUSTERSERVER_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGBaseTypes.h>
#include <OSGThread.h>
#include <OSGClusterDef.h>
#include <OSGFieldContainerPtr.h>
#include <OSGRemoteAspect.h>
#include <OSGClusterWindow.h>
#include <OSGWindow.h>

OSG_BEGIN_NAMESPACE

class Connection;
class ClusterWindow;
class RemoteAspect;
class Thread;
class RenderAction;

class OSG_CLUSTERLIB_DLLMAPPING ClusterServer
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    ClusterServer(WindowPtr     window,
                  const string &serviceName,
                  const string &connectionType="StreamSock",
                  const string &address="",
                  UInt32 servicePort=8437);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~ClusterServer(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   your_category                              */
    /*! \{                                                                 */

    void init              (void);
    void render            (RenderAction *action);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Container Access                             */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Assignment                                */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                   your_category                              */
    /*! \{                                                                 */

    bool windowChanged(FieldContainerPtr& fcp,
                       RemoteAspect *);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    WindowPtr        _window;
    Connection      *_connection;
    string           _address;
    ClusterWindowPtr _clusterWindow;
    RemoteAspect     _aspect;
    string           _serviceName;
    UInt32           _servicePort;
    bool             _serviceAvailable;
    UInt32           _serverId;

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    /*---------------------------------------------------------------------*/
    /*! \name                     Thread                                   */
    /*! \{                                                                 */

    static void *serviceProc(void *arg);

    /*! \}                                                                 */

    /*!\brief prohibit default function (move to 'public' if needed) */
    ClusterServer(const ClusterServer &source);
    /*!\brief prohibit default function (move to 'public' if needed) */
    void operator =(const ClusterServer &source);
};

OSG_END_NAMESPACE

#define OSG_CLUSTERSERVERHEADER_CVSID "@(#)$Id: OSGClusterServer.h,v 1.1 2002/03/22 09:24:59 jsux Exp $"

#endif /* _CLUSTERSERVER_H_ */

