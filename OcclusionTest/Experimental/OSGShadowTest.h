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

#ifndef _OSGSHADOWTEST_H_
#define _OSGSHADOWTEST_H_

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGSystemDef.h>
#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGDynamicVolume.h>
#include <OpenSG/OSGShadowVolume.h>

#include <OpenSG/OSGOcclusionTestBase.h>

OSG_BEGIN_NAMESPACE

//! ShadowTest
//! \ingroup RenderingBackendLib

#define MAX_SHADOW_VOLUMES 15000

class OSG_SYSTEMLIB_DLLMAPPING ShadowTest:public OcclusionTestBase
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

    ShadowTest(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~ShadowTest(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const Int32&, Viewport*, const Pnt3f&);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void perform(const Int32&, const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx);
    void insertBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx);
    Int32 result(const Int32&);
    void exit(void);
    
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */


    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    UInt32 _maxtests;
    bool* _results;
    ShadowVolumePtr _sv[MAX_SHADOW_VOLUMES];
    Pnt3f _eye;
    UInt16 _lastsv;
    Int32 _w, _h;

    /*! \brief prohibit default function (move to 'public' if needed) */
    ShadowTest(const ShadowTest &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const ShadowTest &source);
};

OSG_END_NAMESPACE

#include "OpenSG/OSGShadowTest.inl"

#endif /* _OSGSHADOWTEST_H_ */
