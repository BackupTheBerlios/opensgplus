/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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

#ifndef _OSGGEOMETRYCLUSTERED_H_
#define _OSGGEOMETRYCLUSTERED_H_
#ifdef __sgi
#pragma once
#endif

#include <fstream>
#include <OSGConfig.h>
#include <OSGGeometryClusteredBase.h>

OSG_BEGIN_NAMESPACE

/*! \brief GeometryClustered class.
*/
class OSG_GENVISLIB_DLLMAPPING GeometryClustered : public GeometryClusteredBase
{
  private:

    typedef GeometryClusteredBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    CellData*               getPoolEntry (UInt32 i, bool omit=false);
    const CellData*         getPoolEntry (UInt32 i) const;
    CellData*               getPoolEntry (const Pnt3f& eye, 
					  Real32 minDist, Real32 maxDist, 
					  UInt32 i,
					  bool omit=false);
    const CellData*         getPoolEntry (const Pnt3f& eye, 
					  Real32 minDist, Real32 maxDist, 
					  UInt32 i) const;

    /*---------------------------------------------------------------------*/
    /*! \name                      Sync                                    */
    /*! \{                                                                 */

    virtual void changed  (BitVector  whichField, 
			   UInt32     origin    );
    void         onCreate (const GeometryClustered* source=NULL);
    void         handleGL (Window* win, UInt32 idstatus);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Output                                   */
    /*! \{                                                                 */

    virtual void dump(      UInt32     uiIndent = 0, 
                      const BitVector  bvFlags  = 0) const;

    virtual Action::ResultE drawPrimitives (DrawActionBase * action );
    void                    fillPositions  (std::istream& is);
    void                    fillGrid       ();
    void                    fillGridFile   (std::istream& is);

    /*! \}                                                                 */ 

    /*=========================  PROTECTED  ===============================*/
  protected:
    // Variables should all be in GeometryClusteredBase.

    void adjustVolume (Volume& volume);

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    GeometryClustered(void);
    GeometryClustered(const GeometryClustered &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~GeometryClustered(void); 

    /*! \}                                                                 */
    
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class GeometryClusteredBase;

    static void initMethod(void);

    // prohibit default functions (move to 'public' if you need one)

    void operator =(const GeometryClustered &source);
};

typedef GeometryClustered *GeometryClusteredP;

OSG_END_NAMESPACE

#include <OSGGeometryClusteredBase.inl>
#include <OSGGeometryClustered.inl>

#define OSGGEOMETRYCLUSTERED_HEADER_CVSID "@(#)$Id: OSGGeometryClustered.h,v 1.6 2004/12/21 17:44:10 fuenfzig Exp $"

#endif /* _OSGGEOMETRYCLUSTERED_H_ */
