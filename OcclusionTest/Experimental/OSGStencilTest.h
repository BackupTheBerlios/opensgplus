/*---------------------------------------------------------------------------*\
 *                     OpenSG PLUS Occlusion Test                            *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2002 by the WSI/GRIS Uni Tuebingen              *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: staneker@gris.uni-tuebingen.de                                 *
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

#ifndef _OSGSTENCILTEST_H_
#define _OSGSTENCILTEST_H_

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OSGBaseTypes.h>
#include <OSGSystemDef.h>
#include <OSGDrawAction.h>
#include <OSGDynamicVolume.h>

#include <OSGOcclusionTypes.h>

OSG_BEGIN_NAMESPACE

//! StencilTest
//! \ingroup RenderingBackendLib

class OSG_SYSTEMLIB_DLLMAPPING StencilTest
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                   Statistic                                  */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    StencilTest(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~StencilTest(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const UInt16&, Viewport*, const UInt32 maxpix=0);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void perform(const UInt16&, const OCTestNode*);
    UInt32 result(const UInt16&);
    void exit(void);
    vois visualize(void);
    
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */


    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    GLuint* _stencilbuf;
    Viewport* _port;
    UInt16 _w, _h;
    UInt16 _count;
    UInt16 _maxtests;
    UInt32* _results;
    UInt32 _maxpix;

    /*! \brief prohibit default function (move to 'public' if needed) */
    StencilTest(const StencilTest &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const StencilTest &source);
};

OSG_END_NAMESPACE

#include "OSGStencilTest.inl"

#endif /* _OSGSTENCILTEST_H_ */
