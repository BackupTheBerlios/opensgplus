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

#ifndef _OSGSOFTZBTEST_H_
#define _OSGSOFTZBTEST_H_ 1

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OSGBaseTypes.h>
#include <OSGSystemDef.h>
#include <OSGDrawAction.h>
#include <OSGDynamicVolume.h>

#include <OSGOcclusionTestBase.h>

#include <OSGRasterizer.h>

OSG_BEGIN_NAMESPACE

//! SoftZBTest
//! \ingroup RenderingBackendLib

template<UInt16 scaleshift=2>
class OSG_SYSTEMLIB_DLLMAPPING SoftZBTest:public OcclusionTestBase
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

    SoftZBTest(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~SoftZBTest(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const Int32&, Viewport*, const Int32 maxpix=0);

    void frameInit(void);
    void frameExit(void);

    void init(void);
    void perform(const Int32&, const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx);
    void insertBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx);
    UInt32 result(const Int32&);
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

    class testPoint{
	    private:
            SoftZBTest<scaleshift>* _t;

	    public:
	    testPoint(SoftZBTest<scaleshift>* t):_t(t){ };
	    inline bool visitPoint(const Int16& x, const Int16& y, const Real32& z){
		return(_t->visitPoint(x,y,z));
	    };
    };
	  
    class renderPoint{
	    private:
            SoftZBTest<scaleshift>* _t;

	    public:
	    renderPoint(SoftZBTest<scaleshift>* t):_t(t){ };
	    inline bool visitPoint(const Int16& x, const Int16& y, const Real32& z){
		return(_t->setPoint(x,y,z));
	    };
    };
    
    testPoint _testPoint;
    renderPoint _renderPoint;
   
    UInt32 _maxtests;
    UInt32* _results;

    bool _bufdirty;
    Int32 _pixcount;
    Int32 _maxpix;
    UInt32 _w, _h;	// Width and height of viewport
    /**************************************************
     * Fragmented Z-Buffer for Occlusion Culling Test *
     **************************************************/
    Real32* _zbuffer;
    
    Real32 _fr_min;
    Real32 _fr_max;

    void clearZBuffer(void);
    Real32& getPoint(const Int32& x, const Int32& y);
    bool setPoint(const Int32& x, const Int32& y, const Real32& f);
    // void setMaxPix(const Int32& p){ _maxpix=p; };
    bool visitPoint(const Int16& x, const Int16& y, const Real32&);
    UInt32 boxIsVisible(void);
    UInt32 boxIsHidden(void);
    UInt32 testBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx);

    void visualize(void);

    /*! \brief prohibit default function (move to 'public' if needed) */
    SoftZBTest(const SoftZBTest &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const SoftZBTest &source);
};

OSG_END_NAMESPACE

#include "OSGSoftZBTest.inl"

#endif /* _OSGSOFTZBTEST_H_ */
