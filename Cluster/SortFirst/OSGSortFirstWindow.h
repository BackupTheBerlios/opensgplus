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

#ifndef _OSGSORTFIRSTWINDOW_H_
#define _OSGSORTFIRSTWINDOW_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>
#include <OSGViewBufferHandler.h>
#include <OSGSortFirstWindowBase.h>

OSG_BEGIN_NAMESPACE

/*! \brief *put brief class description here* 
 */

class OSG_CLUSTERLIB_DLLMAPPING SortFirstWindow : public SortFirstWindowBase
{
  private:

    typedef SortFirstWindowBase Inherited;

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
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name      client window funcitons                                 */
    /*! \{                                                                 */

    virtual void clientInit              ( WindowPtr window,
                                           Connection *connection          );
    virtual void clientFrameInit         ( WindowPtr window,
                                           Connection *connection,
                                           RemoteAspect *aspect            );
    virtual void clientRenderAllViewports( WindowPtr window,
                                           Connection *connection,
                                           RenderAction *action            );
    virtual void clientSwap              ( WindowPtr window,
                                           Connection *connection          );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name      server window funcitons                                 */
    /*! \{                                                                 */

    virtual void serverInit              ( WindowPtr window,UInt32 id,
                                           Connection *connection          );
    virtual void serverFrameInit         ( WindowPtr window,UInt32 id,
                                           Connection *connection,
                                           RemoteAspect *aspect            );
    virtual void serverSwap              ( WindowPtr window,UInt32 id,
                                           Connection *connection          );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    SortFirstWindow(void);
    SortFirstWindow(const SortFirstWindow &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~SortFirstWindow(void); 

    /*! \}                                                                 */
    
    /*==========================  PRIVATE  ================================*/
  private:

    static ViewBufferHandler _bufferHandler;

    friend class FieldContainer;
    friend class SortFirstWindowBase;

    static void initMethod(void);

    // prohibit default functions (move to 'public' if you need one)

    void operator =(const SortFirstWindow &source);
};

typedef SortFirstWindow *SortFirstWindowP;

OSG_END_NAMESPACE

#include <OSGSortFirstWindow.inl>
#include <OSGSortFirstWindowBase.inl>

#define OSGSORTFIRSTWINDOW_HEADER_CVSID "@(#)$Id: $"

#endif /* _OSGSORTFIRSTWINDOW_H_ */
