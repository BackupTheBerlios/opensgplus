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

#ifndef _OSGCLUSTERWINDOW_H_
#define _OSGCLUSTERWINDOW_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>
#include <OSGClusterWindowBase.h>
#include <OSGStatCollector.h>
#include <OSGStatElemTypes.h>

OSG_BEGIN_NAMESPACE

class Connection;
class ClusterServer;
class RemoteAspect;

class OSG_CLUSTERLIB_DLLMAPPING ClusterWindow : public ClusterWindowBase
{
  private:

    typedef ClusterWindowBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                      Sync                                    */
    /*! \{                                                                 */

    virtual void changed(BitVector  whichField, 
                         ChangeMode from);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Output                                   */
    /*! \{                                                                 */

    virtual void dump(      UInt32     uiIndent = 0, 
                      const BitVector  bvFlags  = 0) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name            GL implementation functions                       */
    /*! \{                                                                 */

    virtual void    (*getFunctionByName ( const Char8 *s ))();

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name      Window system implementation functions                  */
    /*! \{                                                                 */

    virtual void    activate          ( void                        );
    virtual void    deactivate        ( void                        );
    virtual void    swap              ( void                        );
    virtual void    init              ( void                        );
    virtual void    render            ( RenderAction *action = NULL );
    virtual void    renderAllViewports( RenderAction *action = NULL );
    virtual void    frameInit         ( void                        );
    virtual void    frameExit         ( void                        );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Statistics                                 */
    /*! \{                                                                 */

    StatCollector *getStatistics(void                  ) const;
    void           setStatistics(StatCollector * stat  );

    /*! \}                                                                 */

    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name      client window funcitons                                 */
    /*! \{                                                                 */

    virtual void clientInit              ( void                        );
    virtual void clientPreSync           ( void                        );
    virtual void clientRender            ( RenderAction *action        );
    virtual void clientSwap              ( void                        );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name      server window funcitons                                 */
    /*! \{                                                                 */

    virtual void serverInit              ( WindowPtr window,UInt32 id  );
    virtual void serverRender            ( WindowPtr window,UInt32 id,
                                           RenderAction *action        );
    virtual void serverSwap              ( WindowPtr window,UInt32 id  );

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    ClusterWindow(void);
    ClusterWindow(const ClusterWindow &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~ClusterWindow(void); 

    /*! \}                                                                 */
    
    /*---------------------------------------------------------------------*/
    /*! \name               unsynced thread variables                      */
    /*! \{                                                                 */

    Connection      *_connection;
    RemoteAspect    *_remoteAspect;
    bool             _firstFrame;
    StatCollector   *_statistics;

    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class ClusterWindowBase;
    friend class ClusterServer;
    friend class ClusterClient;

    static void initMethod(void);
    // prohibit default functions (move to 'public' if you need one)

    void operator =(const ClusterWindow &source);
};

typedef ClusterWindow *ClusterWindowP;

OSG_END_NAMESPACE

#include <OSGClusterWindow.inl>
#include <OSGClusterWindowBase.inl>

#define OSGCLUSTERWINDOW_HEADER_CVSID "@(#)$Id: $"

#endif /* _OSGCLUSTERWINDOW_H_ */
