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

#ifndef _OSGZBTEST_H_
#define _OSGZBTEST_H_ 1

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OSGBaseTypes.h>
#include <OSGSystemDef.h>
#include <OSGDrawAction.h>
#include <OSGDynamicVolume.h>

#include <OSGOcclusionTypes.h>

#include <OSGOcclusionTestBase.h>

OSG_BEGIN_NAMESPACE

//! OcclusionZBTest
//! \ingroup RenderingBackendLib

template<UInt16 fragwidth, UInt16 fragheight>
class OSG_SYSTEMLIB_DLLMAPPING OcclusionZBTest:public OcclusionTestBase
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

    OcclusionZBTest(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~OcclusionZBTest(void);
    
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

    bool visitPoint(const Int16& x, const Int16& y, const Real32& zz);
    
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

    UInt32 _maxtests;
    GLuint* _results;

    Int32 _pixcount;
    Int32 _maxpix;
    UInt32 _w, _h;	// Width and height of viewport
    /**************************************************
     * Fragmented Z-Buffer for Occlusion Culling Test *
     **************************************************/
    typedef struct {
	bool _dirty;
	bool _unused;
	Real32 _buffer[fragwidth*fragheight];
	UInt32 _reads;
    } _fragment;
    _fragment* _zbuffer;
    Int32 _fr_w;
    Int32 _fr_h;
    Real32 _fr_min;
    Real32 _fr_max;
    Int32 _zb_pos;
    Int32 _glReads;

    void setAllUnused(void);
    void setAllDirty(void);
    Real32& getPoint(const Int32& x, const Int32& y);
    // bool visitPoint(const Real32& zb, const Real32& zz, const Int16& x, const Int16& y);
    UInt32 boxIsVisible(void);
    UInt32 boxIsHidden(void);
    UInt32 testBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx);

    /*! \brief prohibit default function (move to 'public' if needed) */
    OcclusionZBTest(const OcclusionZBTest &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const OcclusionZBTest &source);
};

OSG_END_NAMESPACE

#include "OSGOcclusionZBTest.inl"

#endif /* _OSGZBTEST_H_ */
