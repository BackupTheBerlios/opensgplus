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

#ifndef _OSGOCCLUSIONVOLUME_H_
#define _OSGOCCLUSIONVOLUME_H_ 1

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OSGBaseTypes.h>
#include <OSGSystemDef.h>
#include <OSGDrawAction.h>
#include <OSGDynamicVolume.h>
#include <OSGNode.h>

#include <OSGOcclusionTypes.h>

OSG_BEGIN_NAMESPACE

//! OcclusionVolume
//! \ingroup RenderingBackendLib

template<UInt16 volwidth, UInt16 volheight, UInt16 voldepth>
class OSG_SYSTEMLIB_DLLMAPPING OcclusionVolume
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

    OcclusionVolume(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~OcclusionVolume(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const Int32& max, Viewport*, const UInt32 maxpix=0);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void perform(const Int32&, const OCTestNode*);
    UInt32 result(const Int32&);
    void exit(void);

    void visualize(void);

    void generateOcclusionVolume(NodePtr);
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

    Int32 _pixcount;
    Int32 _maxpix;
    UInt32 _w, _h;	// Width and height of viewport

    bool _volume[volwidth*volheight*voldepth];

    /*! \brief prohibit default function (move to 'public' if needed) */
    OcclusionVolume(const OcclusionVolume &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const OcclusionVolume &source);
};

OSG_END_NAMESPACE

#include "OSGOcclusionVolume.inl"

#endif /* _OSGOCCLUSIONVOLUME_H_ */
