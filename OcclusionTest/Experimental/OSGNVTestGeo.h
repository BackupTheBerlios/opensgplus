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

#ifndef _OSGNVTESTGEO_H_
#define _OSGNVTESTGEO_H_

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGSystemDef.h>
#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGDynamicVolume.h>

#include <OpenSG/OSGOcclusionTestBase.h>

OSG_BEGIN_NAMESPACE

//! NVTestGeo
//! \ingroup RenderingBackendLib

class OSG_SYSTEMLIB_DLLMAPPING NVTestGeo:public OcclusionTestBase
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

    NVTestGeo(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~NVTestGeo(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const Int32&);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void start(const Int32&);
    void end(void);
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
    GLuint* _results;

    /*! \brief prohibit default function (move to 'public' if needed) */
    NVTestGeo(const NVTestGeo &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const NVTestGeo &source);
};

OSG_END_NAMESPACE

#include "OpenSG/OSGNVTestGeo.inl"

#endif /* _OSGNVTESTGEO_H_ */
