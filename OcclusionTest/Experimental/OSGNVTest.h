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

#ifndef _OSGNVTEST_H_
#define _OSGNVTEST_H_

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OSGBaseTypes.h>
#include <OSGSystemDef.h>
#include <OSGDrawAction.h>
#include <OSGDynamicVolume.h>

#include <OSGOcclusionTypes.h>
#include <OSGGLEXT.h>

OSG_BEGIN_NAMESPACE

//! NVTest
//! \ingroup RenderingBackendLib

class OSG_SYSTEMLIB_DLLMAPPING NVTest
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

    NVTest(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~NVTest(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const UInt16&, Viewport* v=NULL, const UInt32 maxpix=0);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void perform(const UInt16&, const OCTestNode*);
    UInt32 result(const UInt16&);
    void exit(void);
    void visualize(void);
    
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */


    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    UInt32 _nvExtension;
    UInt32 _funcGenOcclusionQueriesNV;
    UInt32 _funcBeginOcclusionQueryNV;
    UInt32 _funcEndOcclusionQueryNV;
    UInt32 _funcGetOcclusionQueryuivNV;
    void (OSG_APIENTRY* _glGenOcclusionQueriesNV)(GLsizei, GLuint*);
    void (OSG_APIENTRY* _glBeginOcclusionQueryNV)(GLuint);
    void (OSG_APIENTRY* _glEndOcclusionQueryNV)(void);
    void (OSG_APIENTRY* _glGetOcclusionQueryuivNV)(GLuint, GLenum, GLuint*);
    UInt16 _maxtests;
    GLuint* _results;
    bool _extworks;

    /*! \brief prohibit default function (move to 'public' if needed) */
    NVTest(const NVTest &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const NVTest &source);
};

OSG_END_NAMESPACE

#include "OSGNVTest.inl"

#endif /* _OSGNVTEST_H_ */
