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

#ifndef _OSGRASTERIZER_H_
#define _OSGRASTERIZER_H_ 1

#ifdef __sgi
#pragma once
#endif

#include <GL/gl.h>
#include <OSGBaseTypes.h>
#include <OSGSystemDef.h>
#include <OSGDrawAction.h>
#include <OSGDynamicVolume.h>

OSG_BEGIN_NAMESPACE

//! Rasterizer
//! \ingroup RenderingBackendLib

template<typename T, bool with_depth=1>
class OSG_SYSTEMLIB_DLLMAPPING Rasterizer
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


    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Access                                    */
    /*! \{                                                                 */
    static bool testTriangle(T* t, const Int16& x1, const Int16& y1, const Real32& z1, const Int16& x2, const Int16& y2, const Real32& z2, const Int16& x3, const Int16& y3, const Real32& z3);
    static void setSize(const Int32 s, const Int32 w, const Int32 h){ _size=s; _w=w; _h=h; };
    
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:
    /*---------------------------------------------------------------------*/
    /*! \name                      Member                                  */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:
    static Int32 _size;
    static Int32 _w;
    static Int32 _h;

    static bool raster(T*, Int32& accu1, Int32& f1, const Int32& xmin, const Int32& xmitte, const Int32& ymin, const Int32& ymitte, const Real32& zmin, const Real32& zmitte, Int32& x1, Real32& z1, Int32& dx1, Int32& dy1, Real32& dz1);
    static bool scanline(T*, Int32& x1, const Real32& z1, Int32& x2, const Real32& z2, const Int32& y);
    static bool scanline(T*, Int32& x1, Int32& x2, const Int32& y);
    
    /*! \brief prohibit default function (move to 'public' if needed) */
    Rasterizer(const Rasterizer &source);
    /*! \brief prohibit default function (move to 'public' if needed) */
    void operator =(const Rasterizer &source);
};

OSG_END_NAMESPACE

#include "OSGRasterizer.inl"

#endif /* _OSGRASTERIZER_H_ */
