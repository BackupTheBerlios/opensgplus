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

#ifndef _OSGEXACTOCCUPANCYMAP_H_
#define _OSGEXACTOCCUPANCYMAP_H_ 1

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OpenSG/OSGBaseTypes.h>
#include <OpenSG/OSGSystemDef.h>
#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGDynamicVolume.h>

#include <OpenSG/OSGRasterizer.h>

OSG_BEGIN_NAMESPACE

//! ExactOccupancyMap
//! \ingroup RenderingBackendLib

template<typename buffertype, UInt16 scaleshift=4>
class OSG_SYSTEMLIB_DLLMAPPING ExactOccupancyMap
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

    ExactOccupancyMap(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~ExactOccupancyMap(void);
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */

    void setup(const Int32&, const UInt16&, const UInt16&);
    void setup(const Int32&, Viewport*);

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

    class testPoint{
	    private:
            ExactOccupancyMap<buffertype, scaleshift>* _t;

	    public:
	    testPoint(ExactOccupancyMap<buffertype, scaleshift>* t):_t(t){ };
	    inline bool visitPoint(const Int16& x, const Int16& y, const Real32&){
		return(_t->visitPoint(x,y,0));
	    };
    };
	  
    class renderPoint{
	    private:
            ExactOccupancyMap<buffertype, scaleshift>* _t;

	    public:
	    renderPoint(ExactOccupancyMap<buffertype, scaleshift>* t):_t(t){ };
	    inline bool visitPoint(const Int16& x, const Int16& y, const Real32&){
		return(_t->setPoint(x,y,1));
	    };
    };
    
    testPoint _testPoint;
    renderPoint _renderPoint;
   
    UInt32 _maxtests;
    GLuint* _results;

    Int32 _pixcount;
    Int32 _maxpix;
    UInt32 _w, _h;	// Width and height of viewport
    UInt32 _bufferw;	// Width and height of buffer
    /**************************************************
     * Fragmented Z-Buffer for Occlusion Culling Test *
     **************************************************/
    buffertype* _buffer;

    void clearBuffer(void);
    bool getPoint(const Int32& x, const Int32& y);
    bool boxIsVisible(void);
    bool boxIsHidden(void);
    bool testBox(const Int16* bb_x, const Int16* bb_y, const Real32* bb_z, const char bb_minidx);
    
    bool setPoint(const Int32& x, const Int32& y, const bool&);
    bool visitPoint(const Int32& x, const Int32& y, const bool&);

    void visualize(void);

    /*! \brief prohibit default function (move to 'public' if needed) */
    ExactOccupancyMap(const ExactOccupancyMap &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const ExactOccupancyMap &source);
};

OSG_END_NAMESPACE

#include "OSGExactOccupancyMap.inl"

#endif /* _OSGEXACTOCCUPANCYMAP_H_ */
